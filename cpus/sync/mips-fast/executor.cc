#include "executor.h"
#include "mips.h"
#include "pipeline.h"
#define MIPC_DEBUG
//define the class...
//add
EM::EM (DE *de) 
{
   _ins = de->_ins;
   _pc = de->_pc;
   store_ins = de->store_ins;
   store_pc = de->store_pc;
   _decodedSRC1 = de->_decodedSRC1;
   _decodedSRC2 = de->_decodedSRC2;
   _decodedSRC3 = de->_decodedSRC3;
   _decodedDST = de->_decodedDST;
   _subregOperand = de->_subregOperand;
   _memory_addr_reg = de->_memory_addr_reg;
   _opResultHi = de->_opResultHi;
   _opResultLo = de->_opResultLo;	
   _memControl = de->_memControl;	
   _writeREG = de->_writeREG;
   _writeFREG = de->_writeFREG;
   _branchOffset = de->_branchOffset;
   _hiWPort = de->_hiWPort;
   _loWPort = de->_loWPort;
   _decodedShiftAmt = de->_decodedShiftAmt;
   _isSyscall = de->_isSyscall;
   _isIllegalOp = de->_isIllegalOp;
   _bd = de->_bd;
   _btgt = de->_btgt;
   _btaken = de->_btaken;
   _lastbd = de->_lastbd;
   is_subreg = de->is_subreg;
   _hi = de->_hi;
   _lo = de->_lo;
   rs_num = de->rs_num;
   rt_num = de->rt_num;
   fr_num = de->fr_num;
   _num_jal = de->_num_jal;
   _num_jr  = de->_num_jr;
   _num_cond_br = de->_num_cond_br;
   _num_load = de->_num_load;
   _num_store = de->_num_store;
   _opControl = de->_opControl;
   _memOp = de->_memOp;
   EX_EX_1=de->EX_EX_1;
   EX_EX_2=de->EX_EX_2;
   MEM_EX_1=de->MEM_EX_1;
   MEM_EX_2=de->MEM_EX_2;
   MEM_MEM_1=de->MEM_MEM_1;
   MEM_MEM_2=de->MEM_MEM_2;
}
EM::EM (void) 
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
EM::~EM (void) {}

void
EM::copy (EM *em) 
{
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
   _opControl = em->_opControl;
   _memOp = em->_memOp;
   EX_EX_1=em->EX_EX_1;
   EX_EX_2=em->EX_EX_2;
   MEM_EX_1=em->MEM_EX_1;
   MEM_EX_2=em->MEM_EX_2;
   MEM_MEM_1=em->MEM_MEM_1;
   MEM_MEM_2=em->MEM_MEM_2;
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
      if (1) {
         AWAIT_P_PHI0;	// @posedge
        EM *em= new EM(_mc->_de);
         unsigned int ins;
         ins=em->_ins;
         isSyscall=em->_isSyscall;
         isIllegalOp=em->_isIllegalOp;
         if(isSyscall == FALSE){
            //EX-EX and MEM-EX bypass
            if(em->rs_num!=0){
               if (em->MEM_EX_1) {
                  em->_decodedSRC1 = _mc->_mw->_opResultLo;
                  if(em->rs_num == HI) em->_hi = _mc->_mw->_opResultHi;
                  if(em->rs_num == LO) em->_lo = _mc->_mw->_opResultLo;
               }
               if (em->EX_EX_1) {
                  em->_decodedSRC1 = _mc->_em->_opResultLo;
                  if(em->rs_num == HI) em->_hi = _mc->_em->_opResultHi;
                  if(em->rs_num == LO) em->_lo = _mc->_em->_opResultLo;
               }
            }
            if(em->rt_num!=0){
               if (em->MEM_EX_2) {
                  if(em->is_subreg)em->_subregOperand = _mc->_mw->_opResultLo;
                  else{
                     em->_decodedSRC2 = _mc->_mw->_opResultLo;
                  }
               }
               if (em->EX_EX_2) {
                  if(em->is_subreg)em->_subregOperand = _mc->_em->_opResultLo;
                  else{
                     em->_decodedSRC2 = _mc->_em->_opResultLo;
                  }
               }
            }
         }
         if (!isSyscall && !isIllegalOp) {
            em->_opControl(em,ins);
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Executed ins %#x\n", SIM_TIME, ins);
#endif
         }
         else if (isSyscall) {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Deferring execution of syscall ins %#x\n", SIM_TIME, ins);
#endif
         }
         else {
#ifdef MIPC_DEBUG
            fprintf(_mc->_debugLog, "<%llu> Illegal ins %#x in execution stage at PC %#x\n", SIM_TIME, ins, _mc->_pc);
            
#endif
         }
         if(!isIllegalOp && !isSyscall){
            if(em->_btaken){
               // #ifdef MIPC_DEBUG
               // fprintf(_mc->_debugLog , "branch target %x\n" , em->_btgt);
               // #endif
            _mc->_pc = em->_btgt;
            }	
         }

         

         AWAIT_P_PHI1;	// @negedge	
         _mc->_em->copy(em);
      }
      else {
         PAUSE(1);
      }
   }
}
