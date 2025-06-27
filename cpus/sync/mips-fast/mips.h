#ifndef __MIPS_H__
#define __MIPS_H__

#include "sim.h"

class Mipc;
class MipcSysCall;
class SysCall;

//added
class FD;
class DE;
class EM;
class MW;
//end added

//add
#define LO 32
#define HI 33
#define HI_LO 35
#define _EX_ 0
#define _ME_ 1
#define TAKE_EM 1
#define TAKE_MW 2
//end

typedef unsigned Bool;
#define TRUE 1
#define FALSE 0

#if BYTE_ORDER == LITTLE_ENDIAN

#define FP_TWIDDLE 0

#else

#define FP_TWIDDLE 1

#endif

#include "mem.h"
#include "../../common/syscall.h"
#include "queue.h"

// #define MIPC_DEBUG 1

class Mipc : public SimObject {
public:
   Mipc (Mem *m);
   ~Mipc ();
  
   FAKE_SIM_TEMPLATE;

   MipcSysCall *_sys;		// Emulated system call layer

   void dumpregs (void);	// Dumps current register state

   void Reboot (char *image = NULL);
				// Restart processor.
				// "image" = file name for new memory
				// image if any.

   void MipcDumpstats();			// Prints simulation statistics
   //void Dec (unsigned int ins);			// Decoder function
   void fake_syscall (unsigned int ins);	// System call interface

   /* processor state */
   unsigned int _ins;   // instruction register
   Bool         _insValid;      // Needed for unpipelined design
   Bool         _decodeValid;   // Needed for unpipelined design
   Bool		_execValid;	// Needed for unpipelined design
   Bool		_memValid;	// Needed for unpipelined design
   Bool         _insDone;       // Needed for unpipelined design

   

   //add
   FD *_fd;
   DE *_de;
   EM *_em;
   MW *_mw;
   Bool     stop_fetch;
   Bool     stop_decode;
   Bool     lock_pipeline_sys;
   Bool     lock_pipeline_data; 
   //end


   unsigned int 	_gpr[32];
   		// general-purpose integer registers
    
    //add
   int   from_EM[34];
   int from_MW[34];
   unsigned int   _gprCycles[34];
   Bool dest_state[34];
   //end


   union {
      unsigned int l[2];
      float f[2];
      double d;
   } _fpr[16];					// floating-point registers (paired)


//add
union {
      unsigned int l[2];
      float f[2];
      double d;
   } _fprState[16];
   unsigned int   _fprCycles[16];
   //done


   unsigned int _hi, _lo; 			// mult, div destination
   unsigned int	_pc;				// Program counter
   unsigned int _lastbd;			// branch delay state
   unsigned int _boot;				// boot code loaded?
    
   int 		_btaken; 			// taken branch (1 if taken, 0 if fall-through)
   int 		_bd;				// 1 if the next ins is delay slot
   unsigned int	_btgt;				// branch target
    


   Bool		_isSyscall;// 1 if system call
   Bool		_isIllegalOp;		// 1 if illegal opcode
    

    //add
    Bool inter_lock;
    //end



   // Simulation statistics counters

   LL	_nfetched;
   LL	_num_cond_br;
   LL	_num_jal;
   LL	_num_jr;
   LL   _num_load;
   LL   _num_store;
   LL   _fpinst;

   //add
   LL _num_sys;
   LL _load_stall;
   //end

   Mem	*_mem;	// attached memory (not a cache)

   Log	_l;
   int  _sim_exit;		// 1 on normal termination

   //void (*_opControl)(Mipc*, unsigned);
   //void (*_memOp)(Mipc*);


    //add
    unsigned int sc_1, sc_2;
   unsigned int fc;
   unsigned int dc;
   //end

   static void func_syscall (Mipc*, unsigned);
   FILE *_debugLog;

};


// Emulated system call interface

class MipcSysCall : public SysCall {
public:

   MipcSysCall (Mipc *ms) {

      char buf[1024];
      m = ms->_mem;
      _ms = ms;
      _num_load = 0;
      _num_store = 0;
   };

   ~MipcSysCall () { };

   LL GetDWord (LL addr);
   void SetDWord (LL addr, LL data);

   Word GetWord (LL addr);
   void SetWord (LL addr, Word data);
  
   void SetReg (int reg, LL val);
   LL GetReg (int reg);
   LL GetTime (void);

private:

   Mipc *_ms;
};
#endif /* __MIPS_H__ */
