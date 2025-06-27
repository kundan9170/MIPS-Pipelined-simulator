#include "memory.h"
#include "pipeline.h"
#define MIPC_DEBUG
//define the classes
MW::MW (EM *em) {
   _pc = em->_pc;
   _ins = em->_ins;

   _decodedSRC3 = em->_decodedSRC3;
   _decodedDST = em->_decodedDST;
   _writeREG = em->_writeREG;
   _writeFREG = em->_writeFREG;
   _hiWPort = em->_hiWPort;
   _loWPort = em->_loWPort;
   _memControl = em->_memControl;
   _opResultHi = em->_opResultHi;
   _opResultLo = em->_opResultLo;
   _MAR = em->_MAR;
   _subregOperand = em->_subregOperand;

   _isSyscall = em->_isSyscall;
   _isIllegalOp = em->_isIllegalOp;

   _memOp = em->_memOp;
}
//Why not carrying reg numbers???

MW::MW (void) 
{
   _ins = 0;
   _pc = 0;

   _decodedSRC3 = 0;
   _decodedDST = 0;
   _writeREG = FALSE;
   _writeFREG = FALSE;
   _hiWPort = FALSE;
   _loWPort = FALSE;
   _memControl = FALSE;
   _subregOperand = 0;
   _opResultHi = 0;
   _opResultLo = 0;
   _MAR = 0;

   _isSyscall = 0;
   _isIllegalOp = 0;

   _memOp = NULL;
}
MW::~MW (void) {}

void
MW::copy (MW *mw) 
{
   _pc = mw->_pc;
   _ins = mw->_ins;

   _decodedSRC3 = mw->_decodedSRC3;
   _decodedDST = mw->_decodedDST;
   _writeREG = mw->_writeREG;
   _writeFREG = mw->_writeFREG;
   _hiWPort = mw->_hiWPort;
   _loWPort = mw->_loWPort;
   _memControl = mw->_memControl;
   _opResultHi = mw->_opResultHi;
   _opResultLo = mw->_opResultLo;
   _MAR = mw->_MAR;
   _subregOperand = mw->_subregOperand;

   _isSyscall = mw->_isSyscall;
   _isIllegalOp = mw->_isIllegalOp;

   _memOp = mw->_memOp;
}



Memory::Memory (Mipc *mc)
{
   _mc = mc;
}

Memory::~Memory (void) {}

void
Memory::MainLoop (void)
{
   Bool memControl;

   while (1) {
      AWAIT_P_PHI0;	// @posedge
      if (_mc->_execValid) {
        //add
	printf("this is Memory\n");
         MW *mw = new MW(_mc->_em);
         memControl = mw->_memControl;
         if (_mc->_em->rt_num != 0 && _mc->_em->_memControl == TRUE) {
         mw->_subregOperand = _mc->_gprState[_mc->_em->rt_num];
         if (_mc->_em->fr_num != 0)
            mw->_decodedSRC3 = _mc->_fprState[(_mc->_em->fr_num)>>1].l[FP_TWIDDLE^((_mc->_em->fr_num)&1)];
         else
            mw->_decodedSRC3 = _mc->_gprState[_mc->_em->rt_num];
        }
        //end
         AWAIT_P_PHI1;      // @negedge
         //add
         _mc->_mw->copy(mw);
         if (_mc->_mw->_memControl) {
            _mc->_mw->_memOp (_mc, _mc->_mw);
        //end
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Accessing memory at address %#x for ins %#x\n", SIM_TIME, _mc->_mw->_MAR, _mc->_ins);
#endif
         }
         else {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Memory has nothing to do for ins %#x\n", SIM_TIME, _mc->_ins);
#endif
         }
         _mc->_execValid = FALSE;
         _mc->_memValid = TRUE;
      }
      else {
         PAUSE(1);
      }
   }
}
