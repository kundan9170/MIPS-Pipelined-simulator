#include "mips.h"
#include "pipeline.h"
#include <assert.h>
#include "mips-irix5.h"
#define MIPC_DEBUG
//Define constructors
FD::FD (void) 
{
   _ins = 0;
   _pc = 0;
   MEM_EX_1=0;
   MEM_EX_2=0;
}
FD::~FD (void) {}


Mipc::Mipc (Mem *m) : _l('M')
{
   _mem = m;
   _sys = new MipcSysCall (this);	// Allocate syscall layer

#ifdef MIPC_DEBUG
   _debugLog = fopen("mipc.debug", "w");
   assert(_debugLog != NULL);
#endif
   
   Reboot (ParamGetString ("Mipc.BootROM"));
}

Mipc::~Mipc (void)
{

}

void 
Mipc::MainLoop (void)
{
   LL addr;
   unsigned int ins;	// Local instruction register

   Assert (_boot, "Mipc::MainLoop() called without boot?");
	_gpr[0] = 0;
   _nfetched = 0;
   for(int i = 0 ; i<34 ; ++i){
      _gprCycles[i] = 0;
   }
   while (!_sim_exit) {     
	// @posedge
      AWAIT_P_PHI0;
      bool is_lock = lock_pipeline_sys;
      AWAIT_P_PHI1;
     if (!is_lock) {
        addr = _pc;
        ins = _mem->BEGetWord (addr, _mem->Read(addr & ~(LL)0x7));
#ifdef MIPC_DEBUG
        fprintf(_debugLog, "<%llu> Fetched ins %#x from PC %#x\n", SIM_TIME, ins, _pc);
#endif  
        //add
        _fd->_ins = ins;
        _fd->_pc=addr;
        _nfetched++;
        _pc+=4;
     }
     else {
        PAUSE(1);
     }
   }
   MipcDumpstats();
   Log::CloseLog();
   
#ifdef MIPC_DEBUG
   assert(_debugLog != NULL);
   fclose(_debugLog);
#endif

   exit(0);
}

void
Mipc::MipcDumpstats()
{
  Log l('*');
  l.startLogging = 0;

  l.print ("");
  l.print ("************************************************************");
  l.print ("");
 //_nfetched += (_sys->_num_load + _sys->_num_store);
  l.print ("Number of instructions: %llu", _nfetched);
  l.print("Number of load stall: %llu" , _load_stall);
  l.print ("Number of simulated cycles: %llu", SIM_TIME);
  l.print ("CPI: %.2f", ((double)SIM_TIME)/_nfetched);
#ifdef BYPASS_ENABLED
  l.print("Load-interlock: %d", _load_interlock_cycles);
  l.print("Load-delay interlock stalls: %f", ((double) _load_interlock_cycles)/SIM_TIME);
#endif  
  l.print ("Int Conditional Branches: %llu", _num_cond_br);
  l.print ("Jump and Link: %llu", _num_jal);
  l.print ("Jump Register: %llu", _num_jr);
  l.print ("Number of fp instructions: %llu", _fpinst);
  l.print ("Number of loads: %llu", _num_load);
  l.print ("Number of syscall emulated loads: %llu", _sys->_num_load);
  l.print ("Number of stores: %llu", _num_store);
  l.print ("Number of syscall emulated stores: %llu", _sys->_num_store);

  l.print("-----------------------------Stats---------------------\n");
  l.print("Total Ins    %llu", _nfetched);
  l.print("Percentage Loads    %lf", ((_num_load + _sys->_num_load)/(float)(_nfetched)) * 100.0);
  l.print("Percentage Stores    %lf", ((_num_store+ _sys->_num_store)/(float)(_nfetched)) * 100.0);
  l.print("Percentage CB    %lf", ((_num_cond_br)/(float)(_nfetched)) * 100.0);
  l.print ("");

}

void 
Mipc::fake_syscall (unsigned int ins)
{
   _sys->pc = _pc;
   _sys->quit = 0;
   _sys->EmulateSysCall ();
   if (_sys->quit)
      _sim_exit = 1;
}

/*------------------------------------------------------------------------
 *
 *  Mipc::Reboot --
 *
 *   Reset processor state
 *
 *------------------------------------------------------------------------
 */
void 
Mipc::Reboot (char *image)
{
   FILE *fp;
   Log l('*');

   _boot = 0;

   if (image) {
      _boot = 1;
      printf ("Executing %s\n", image);
      fp = fopen (image, "r");
      if (!fp) {
	 fatal_error ("Could not open `%s' for booting host!", image);
      }
      _mem->ReadImage(fp);
      fclose (fp);

      // Reset state
      _ins = 0;
      _insValid = FALSE;
      _decodeValid = FALSE;
      _execValid = FALSE;
      _memValid = FALSE;
      _insDone = TRUE;

      _num_load = 0;
      _num_store = 0;
      _fpinst = 0;
      _num_cond_br = 0;
      _num_jal = 0;
      _num_jr = 0;

      _lastbd = 0;
      _bd = 0;
      _btaken = 0;
      _btgt = 0xdeadbeef;
      _sim_exit = 0;
      lock_pipeline_data = 0;
      lock_pipeline_sys = 0;
      _nfetched = 0;

      //add
      _fd = new FD();
      _de = new DE();
      _em = new EM();
      _mw = new MW();
      
      _isSyscall = FALSE;
      _de->_opControl = _em->func_sll;
      //end
      _pc = ParamGetInt ("Mipc.BootPC");	// Boom! GO
   }
}

LL
MipcSysCall::GetDWord(LL addr)
{
   _num_load++;      
   return m->Read (addr);
}

void
MipcSysCall::SetDWord(LL addr, LL data)
{
  
   m->Write (addr, data);
   _num_store++;
}

Word 
MipcSysCall::GetWord (LL addr) 
{ 
  
   _num_load++;   
   return m->BEGetWord (addr, m->Read (addr & ~(LL)0x7)); 
}

void 
MipcSysCall::SetWord (LL addr, Word data) 
{ 
  
   m->Write (addr & ~(LL)0x7, m->BESetWord (addr, m->Read(addr & ~(LL)0x7), data)); 
   _num_store++;
}
  
void 
MipcSysCall::SetReg (int reg, LL val) 
{ 
   _ms->_gpr[reg] = val; 
}

LL 
MipcSysCall::GetReg (int reg) 
{
   return _ms->_gpr[reg]; 
}

LL
MipcSysCall::GetTime (void)
{
  return SIM_TIME;
}
