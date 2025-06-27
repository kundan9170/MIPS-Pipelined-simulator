#include "wb.h"
#include "pipeline.h"
#define MIPC_DEBUG
Writeback::Writeback (Mipc *mc)
{
   _mc = mc;
}

Writeback::~Writeback (void) {}

void
Writeback::MainLoop (void)
{
   unsigned int ins;
   unsigned int pc; //added
   Bool writeReg;
   Bool writeFReg;
   Bool loWPort;
   Bool hiWPort;
   Bool isSyscall;
   Bool isIllegalOp;
   unsigned decodedDST;
   unsigned opResultLo, opResultHi;

   while (1) {
      AWAIT_P_PHI0;	// @posedge
      // Sample the important signals
      //changed locations
      if (_mc->_memValid) {
        printf("This is writeback: %d \n", _mc->_pc); 
	writeReg = _mc->_mw->_writeREG;
         writeFReg = _mc->_mw->_writeFREG;
         loWPort = _mc->_mw->_loWPort;
         hiWPort = _mc->_mw->_hiWPort;
         decodedDST = _mc->_mw->_decodedDST;
         opResultLo = _mc->_mw->_opResultLo;
         opResultHi = _mc->_mw->_opResultHi;
         isSyscall = _mc->_mw->_isSyscall;
         isIllegalOp = _mc->_mw->_isIllegalOp;
         ins = _mc->_mw->_ins;
         pc= _mc->_mw->_pc;
         AWAIT_P_PHI1;       // @negedge
         if (isSyscall) {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> SYSCALL! Trapping to emulation layer at PC %#x\n", SIM_TIME, _mc->_mw->_pc);
#endif      
		printf("we are in syscall of wb stage\n");
            //what are these????
		// added a fake sys call
            _mc->fake_syscall(ins);
            _mc->_pc += 4;
         }
         else if (isIllegalOp) {
            printf("Illegal ins %#x at PC %#x. Terminating simulation!\n", ins, _mc->_mw->_pc);
#ifdef MIPC_DEBUG
            fclose(_mc->_debugLog);
#endif
            printf("Register state on termination:\n\n");
            _mc->dumpregs();
            exit(0);
         }
         else {
            if (writeReg) {
               _mc->_gpr[decodedDST] = opResultLo;
#ifdef MIPC_DEBUG
               fprintf(_mc->_debugLog, "<%llu> Writing to reg %u, value: %#x\n", SIM_TIME, decodedDST, opResultLo);
#endif
            }
            else if (writeFReg) {
               _mc->_fpr[(decodedDST)>>1].l[FP_TWIDDLE^((decodedDST)&1)] = opResultLo;
#ifdef MIPC_DEBUG
               fprintf(_mc->_debugLog, "<%llu> Writing to freg %u, value: %#x\n", SIM_TIME, decodedDST>>1, opResultLo);
#endif
            }
            else if (loWPort || hiWPort) {
               if (loWPort) {
                  _mc->_lo = opResultLo;
#ifdef MIPC_DEBUG
                  fprintf(_mc->_debugLog, "<%llu> Writing to Lo, value: %#x\n", SIM_TIME, opResultLo);
#endif
               }
               if (hiWPort) {
                  _mc->_hi = opResultHi;
#ifdef MIPC_DEBUG
                  fprintf(_mc->_debugLog, "<%llu> Writing to Hi, value: %#x\n", SIM_TIME, opResultHi);
#endif
               }
            }
         }
         _mc->_gpr[0] = 0;
         _mc->_memValid = FALSE;
         _mc->_insDone = TRUE;
      }
      else {
         PAUSE(1);
      }
   }
}
