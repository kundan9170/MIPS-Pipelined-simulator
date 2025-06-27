#include "memory.h"
#include "pipeline.h"
#define MIPC_DEBUG
//define the classes
MW::MW (EM *em) {
   _ins = em->_ins;
   _pc = em->_pc;
   store_ins = em->store_ins;
   store_pc = em->store_pc;
   _decodedSRC1 = em->_decodedSRC1;
   _decodedSRC2 = em->_decodedSRC2;
   _decodedSRC3 = em->_decodedSRC3;
   _decodedDST = em->_decodedDST;
   _subregOperand = em->_subregOperand;
   _memory_addr_reg = em->_memory_addr_reg;
   _opResultHi = em->_opResultHi;
   _opResultLo = em->_opResultLo;	
   _memControl = em->_memControl;	
   _writeREG = em->_writeREG;
   _writeFREG = em->_writeFREG;
   _branchOffset = em->_branchOffset;
   _hiWPort = em->_hiWPort;
   _loWPort = em->_loWPort;
   _decodedShiftAmt = em->_decodedShiftAmt;
   _isSyscall = em->_isSyscall;
   _isIllegalOp = em->_isIllegalOp;
   _bd = em->_bd;
   _lastbd = em->_lastbd;
   _btgt = em->_btgt;
   _btaken = em->_btaken;
   is_subreg = em->is_subreg;
   _hi = em->_hi;
   _lo = em->_lo;
   rs_num = em->rs_num;
   rt_num = em->rt_num;
   fr_num = em->fr_num;
   _num_jal = em->_num_jal;
   _num_jr  = em->_num_jr;
   _num_cond_br = em->_num_cond_br;
   _num_load = em->_num_load;
   _num_store = em->_num_store;
   EX_EX_1=em->EX_EX_1;
   EX_EX_2=em->EX_EX_2;
   MEM_EX_1=em->MEM_EX_1;
   MEM_EX_2=em->MEM_EX_2;
   MEM_MEM_1=em->MEM_MEM_1;
   MEM_MEM_2=em->MEM_MEM_2;
   _opControl = NULL;
   _memOp = em->_memOp;
}

MW::MW (void) 
{
  _ins = 0;
   _pc = 0;
   store_ins = 0;
   store_pc = 0;
   _decodedSRC1 = 0;
   _decodedSRC2 = 0;
   _decodedSRC3 = 0;
   _decodedDST = 0;
   _subregOperand = 0;
   _memory_addr_reg = 0;
   _opResultHi = 0;
   _opResultLo = 0;	
   _memControl = 0;	
   _writeREG = 0;
   _writeFREG = 0;
   _branchOffset = 0;
   _hiWPort = 0;
   _loWPort = 0;
   _decodedShiftAmt = 0;
   _isSyscall = 0;
   _isIllegalOp = 0;
   _bd = 0;
   _lastbd = 0;
   _btgt = 0xdeadbeef;
   _btaken = 0;
   is_subreg = 0;
   _hi = 0;
   _lo = 0;
   rs_num = 0;
   rt_num = 0;
   fr_num = 0;
   _num_jal = 0;
   _num_jr  = 0;
   _num_cond_br = 0;
   _num_load = 0;
   _num_store = 0;
   EX_EX_1=0;
   EX_EX_2=0;
   MEM_EX_1=0;
   MEM_EX_2=0;
   MEM_MEM_1=0;
   MEM_MEM_2=0;
   _opControl = NULL;
   _memOp = NULL;
}
MW::~MW (void) {}

void
MW::copy (MW *mw) 
{
   _ins = mw->_ins;
   _pc = mw->_pc;
   store_ins = mw->store_ins;
   store_pc = mw->store_pc;
   _decodedSRC1 = mw->_decodedSRC1;
   _decodedSRC2 = mw->_decodedSRC2;
   _decodedSRC3 = mw->_decodedSRC3;
   _decodedDST = mw->_decodedDST;
   _subregOperand = mw->_subregOperand;
   _memory_addr_reg = mw->_memory_addr_reg;
   _opResultHi = mw->_opResultHi;
   _opResultLo = mw->_opResultLo;	
   _memControl = mw->_memControl;	
   _writeREG = mw->_writeREG;
   _writeFREG = mw->_writeFREG;
   _branchOffset = mw->_branchOffset;
   _hiWPort = mw->_hiWPort;
   _loWPort = mw->_loWPort;
   _decodedShiftAmt = mw->_decodedShiftAmt;
   _isSyscall = mw->_isSyscall;
   _isIllegalOp = mw->_isIllegalOp;
   _bd = mw->_bd;
   _lastbd = mw->_lastbd;
   _btgt = mw->_btgt;
   _btaken = mw->_btaken;
   is_subreg = mw->is_subreg;
   _hi = mw->_hi;
   _lo = mw->_lo;
   rs_num = mw->rs_num;
   rt_num = mw->rt_num;
   fr_num = mw->fr_num;
   _num_jal = mw->_num_jal;
   _num_jr  = mw->_num_jr;
   _num_cond_br = mw->_num_cond_br;
   _num_load = mw->_num_load;
   _num_store = mw->_num_store;
   _opControl = mw->_opControl;
   _memOp = mw->_memOp;
   EX_EX_1=mw->EX_EX_1;
   EX_EX_2=mw->EX_EX_2;
   MEM_EX_1=mw->MEM_EX_1;
   MEM_EX_2=mw->MEM_EX_2;
   MEM_MEM_1=mw->MEM_MEM_1;
   MEM_MEM_2=mw->MEM_MEM_2;
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

   while (TRUE) {
         AWAIT_P_PHI0;
         MW *mw = new MW(_mc->_em);

         
         AWAIT_P_PHI1;      // @negedge
         if (mw->_memControl) {
            mw->_memOp (_mc, mw);
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Accessing memory at address %#x for ins %#x\n", SIM_TIME, mw->_memory_addr_reg, mw->_ins);
#endif
         }
         else {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Memory has nothing to do for ins %#x\n", SIM_TIME, mw->_ins);
#endif
         }
         _mc->_mw->copy(mw);
   }
}
