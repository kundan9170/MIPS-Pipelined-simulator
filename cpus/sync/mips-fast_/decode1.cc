#include "decode.h"
#include "pipeline.h"
#define MIPC_DEBUG
//first define classes constructors and copy constructors


DE::DE(FD *fd) 
{
   _ins = fd->_ins;
   _pc = fd->_pc;
}
DE::DE(void) 
{
   _ins = 0;
   ins_prev = 0;
   _pc = 0;
   pc_prev = 0;
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

   _isSyscall = 0;
   _isIllegalOp = 0;

   _bd = 0;
   _btgt = 0xdeadbeef;

   rs_num = 0;
   rt_num = 0;
   fr_num = 0;

   _opControl = NULL;
   _memOp = NULL;
}

DE::~DE (void) {}

void DE::copy(DE *de) 
{
   _ins = de->_ins;
   _pc = de->_pc;
}

//Has the processor in it bas...
Decode::Decode (Mipc *mc)
{
   _mc = mc;
}

Decode::~Decode (void) {}

void
Decode::MainLoop (void)
{
   unsigned int ins;
   while (1) {
      AWAIT_P_PHI0;	// @posedge
	//printf("This is decoder\n");
      if (_mc->_insValid) {
        //add
	printf("This is decoder\n");
         ins = _mc->_fd->_ins;
         DE *de = new DE(_mc->_fd);
        //end
         AWAIT_P_PHI1;	// @negedge
         //add
         //in pipelined we use de as we have copied it to de
         _mc->_de->copy(de);
         _mc->_de->_bd = 0;
         _mc->_de->rs_num = _mc->_de->rt_num = _mc->_de->fr_num = 0;
         _mc->dc= 0;
         //_mc->_interlock = FALSE;
         _mc->_de->Dec(_mc,_mc->_em,_mc->_mw,ins);
        //end
#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins);
#endif
         _mc->_insValid = FALSE;
         _mc->_decodeValid = TRUE;
      }
      else {
         PAUSE(1);
      }
   }
}
