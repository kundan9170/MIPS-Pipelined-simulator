#include "executor.h"
#include "pipeline.h"
//define the class...
//add
#define MIPC_DEBUG
EM::EM (DE *de) 
{
   _ins = de->_ins;
   _pc = de->_pc;

   _decodedSRC1 = de->_decodedSRC1;
   _decodedSRC2 = de->_decodedSRC2;
   _decodedSRC3 = de->_decodedSRC3;
   _decodedDST = de->_decodedDST;		
   _writeREG = de->_writeREG;
   _writeFREG = de->_writeFREG;
   _hiWPort = de->_hiWPort;
   _loWPort = de->_loWPort;
   _memControl = de->_memControl;
   _decodedShiftAmt = de->_decodedShiftAmt;
   _branchOffset = de->_branchOffset;
   _subregOperand = de->_subregOperand;
   
   _isSyscall = de->_isSyscall;
   _isIllegalOp = de->_isIllegalOp;

   _bd = de->_bd;
   _btgt = de->_btgt;
   _btaken = 0;
   //Why no rs_num?????
   rt_num = de->rt_num;
   fr_num = de->fr_num;

   _opControl = de->_opControl;
   _memOp = de->_memOp;
}
EM::EM (void) 
{
   _ins = 0;
   _pc = 0;

   _decodedSRC1 = 0;
   _decodedSRC2 = 0;
   _decodedSRC3 = 0;
   _decodedDST = 0;
   _writeREG = FALSE;
   _writeFREG = FALSE;
   _hiWPort = FALSE;
   _loWPort = FALSE;
   _memControl = FALSE;
   _decodedShiftAmt = 0;
   _branchOffset = 0;
   _subregOperand = 0;
   _opResultHi = 0;
   _opResultLo = 0;
   _MAR = 0;

   _isSyscall = 0;
   _isIllegalOp = 0;

   _bd = 0;
   _btgt = 0xdeadbeef;
   _hi = 0;
   _lo = 0;
   _btaken = 0;

   _opControl = NULL;
   _memOp = NULL;

   rt_num = 0;
   fr_num= 0;
   //Again why no rs_num???

   _num_jal = 0;
   _num_jr = 0;
   _num_cond_br = 0;
   _num_load = 0;
   _num_store = 0;
}
EM::~EM (void) {}

void
EM::copy (EM *em) 
{
   _ins = em->_ins;
   _pc = em->_pc;

   _decodedSRC1 = em->_decodedSRC1;
   _decodedSRC2 = em->_decodedSRC2;
   _decodedSRC3 = em->_decodedSRC3;
   _decodedDST = em->_decodedDST;		
   _writeREG = em->_writeREG;
   _writeFREG = em->_writeFREG;
   _hiWPort = em->_hiWPort;
   _loWPort = em->_loWPort;
   _memControl = em->_memControl;
   _decodedShiftAmt = em->_decodedShiftAmt;
   _branchOffset = em->_branchOffset;
   _subregOperand = em->_subregOperand;
   
   _isSyscall = em->_isSyscall;
   _isIllegalOp = em->_isIllegalOp;

   _bd = em->_bd;
   _btgt = em->_btgt;
   _btaken = em->_btaken;
   _opControl = em->_opControl;
   _memOp = em->_memOp;

   rt_num = em->rt_num;
   fr_num = em->fr_num;

   _hi = em->_hi;
   _lo = em->_lo;
}
//end



Exe::Exe (Mipc *mc)
{
   _mc = mc;
}

Exe::~Exe (void) {}

void
Exe::MainLoop (void)
{
   unsigned int ins;
   Bool isSyscall, isIllegalOp;

   while (1) {
      AWAIT_P_PHI0;	// @posedge
      if (_mc->_decodeValid) {
	printf("This is executor\n");  
         /*ins = _mc->_ins;
         isSyscall = _mc->_isSyscall;
         isIllegalOp = _mc->_isIllegalOp;*/
         //add
         EM *em= new EM(_mc->_de);
         // if (_mc->_de->rs_num != 0)
         // em->_decodedSRC1 = _mc->_gprState[_mc->_de->rs_num];
         // if (_mc->_de->rt_num != 0 && _mc->_de->_memControl == FALSE)
         //    em->_decodedSRC2 = _mc->_gprState[_mc->_de->rt_num];
         // em->_hi = _mc->_gprState[HI];
         // em->_lo = _mc->_gprState[LO];
         // if (_mc->_de->fr_num != 0)
         //    em->_decodedSRC1 = _mc->_fprState[(_mc->_de->fr_num)>>1].l[FP_TWIDDLE^((_mc->_de->fr_num)&1)];

         // if (!em->_isIllegalOp && !em->_isSyscall && em->_bd == 1)       // Instruction is a branch instruction
         // {
         //    em->_opControl(em, ins);
         //    if (em->_btaken) {
         //       _mc->_pc = em->_btgt;
         //    }
         // }
         
         //end
         AWAIT_P_PHI1;	// @negedge
         
         //add
         _mc->_em->copy(em);
         unsigned int ins;
         ins=_mc->_em->_ins;
         isSyscall=_mc->_em->_isSyscall;
         isIllegalOp=_mc->_em->_isIllegalOp;
         //end
         if (!isSyscall && !isIllegalOp) {
            _mc->_em->_opControl(_mc->_em,ins);
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Executed ins %#x\n", SIM_TIME, ins);
#endif	
		//printf("This is in ...... executor\n");
         }
         else if (isSyscall) {
		printf("This is syscall in executor\n");
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Deferring execution of syscall ins %#x\n", SIM_TIME, ins);
#endif
         }
         else {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Illegal ins %#x in execution stage at PC %#x\n", SIM_TIME, ins, _mc->_pc);
#endif
         }
         _mc->_decodeValid = FALSE;
         _mc->_execValid = TRUE;
        //Changed rhs to _em-> stuff
	// removed _lastbd , didnt know why it was used
         if (!isIllegalOp && !isSyscall) {
		printf("This is what we wanted to see.....\n");
            if (_mc->_em->_btaken)
            {
               _mc->_pc = _mc->_em->_btgt;
            }
            else
            {
               _mc->_pc = _mc->_em->_pc + 4;
		printf("This is update PC in exec stage new PC: %d",_mc->_pc);
            }
            //i_mc->_em->_lastbd = _mc->_em->_bd;
         }
      }
      else {
         PAUSE(1);
      }
   }
}
