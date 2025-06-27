#include "decode.h"
#include "pipeline.h"
#include "mips.h"
#define MIPC_DEBUG
//first define classes constructors and copy constructors


DE::DE(FD *fd) 
{
   _ins = fd->_ins;
   _pc = fd->_pc;
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
   float_cycles=100;
   source_cycles_1=100;
   source_cycles_2= 100;
   dest_cycles=0;
   EX_EX_1=0;
   EX_EX_2=0;
   MEM_EX_1=fd->MEM_EX_1;
   MEM_EX_2=fd->MEM_EX_2;
   store_MEM_EX_1=0;
   store_MEM_EX_2=0;
   MEM_MEM_1=0;
   MEM_MEM_2=0;
   _opControl = NULL;
   _memOp = NULL;
}
DE::DE(void) 
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
   _btgt = 0xdeadbeef;
   _btaken = 0;
   _lastbd = 0;
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
   source_cycles_1=100;
   source_cycles_2= 100;
   float_cycles=100;
   dest_cycles=0;
   EX_EX_1=0;
   EX_EX_2=0;
   MEM_EX_1=0;
   MEM_EX_2=0;
   MEM_MEM_1=0;
   MEM_MEM_2=0;
   store_MEM_EX_1 = 0;
   store_MEM_EX_2 = 0;
   _opControl = NULL;
   _memOp = NULL;

}

DE::~DE (void) {}

void 
DE::copy(DE *de) 
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
   _lastbd = de->_lastbd;
   _btgt = de->_btgt;
   _btaken = de->_btaken;
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
   source_cycles_1= de->source_cycles_1;
   source_cycles_2= de->source_cycles_2;
   dest_cycles= de->dest_cycles;
   float_cycles= de->float_cycles;
   EX_EX_1=de->EX_EX_1;
   EX_EX_2=de->EX_EX_2;
   MEM_EX_1=de->MEM_EX_1;
   MEM_EX_2=de->MEM_EX_2;
   MEM_MEM_1=de->MEM_MEM_1;
   MEM_MEM_2=de->MEM_MEM_2;
   store_MEM_EX_1=de->store_MEM_EX_1;
   store_MEM_EX_2=de->store_MEM_EX_2;
}

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
      if (1) {
         AWAIT_P_PHI0;	// @posedge
         if(_mc->lock_pipeline_data){
            _mc->_pc = _mc->_fd->_pc;
            _mc->_fd->_ins = _mc->_de->store_ins;
            _mc->_fd->_pc = _mc->_de->store_pc;
            _mc->lock_pipeline_data = FALSE;
            _mc->_nfetched--;
            _mc->_load_stall++;
         }
         else if(_mc->_isSyscall){
            _mc->_pc = _mc->_fd->_pc;
            _mc->_fd->_ins = 0;
            _mc->_isSyscall= FALSE;
            _mc->_nfetched--;
            _mc->_load_stall++;
         }
         ins = _mc->_fd->_ins;
         DE *de = new DE(_mc->_fd);
         bool prev_ex_mem_control = _mc->_de->_memControl;
         bool prev_ex_write_reg = _mc->_de->_writeREG;
         int prev_ex_decoded_dst = _mc->_de->_decodedDST;
         bool prev_me_write_reg = _mc->_em->_writeREG;
         int prev_me_decoded_dst = _mc->_em->_decodedDST;
         
         AWAIT_P_PHI1;	// @negedge
	
         _mc->_de->copy(de);
         _mc->_de->Dec(_mc,_mc->_em,_mc->_mw,ins);
	      if(_mc->_de->_hiWPort) _mc->_de->_decodedDST=HI;
         else if(_mc->_de->_loWPort) _mc->_de->_decodedDST=LO;
         if(_mc->_de->_hiWPort && _mc->_de->_loWPort) _mc->_de->_decodedDST= HI_LO;
         //EXEX
         if (_mc->_de->rs_num && prev_ex_mem_control == FALSE) {
            if ((prev_ex_decoded_dst==HI_LO && (_mc->_de->rs_num==LO || _mc->_de->rs_num==HI))||(prev_ex_write_reg && prev_ex_decoded_dst == _mc->_de->rs_num)){
		         _mc->_de->EX_EX_1= TRUE;
	}
         }
         if (_mc->_de->rt_num && prev_ex_mem_control == FALSE) {
            if (prev_ex_write_reg && prev_ex_decoded_dst == _mc->_de->rt_num) _mc->_de->EX_EX_2= TRUE;
         }
         //MEMEX
         if (_mc->_de->rs_num) {
            if((prev_me_decoded_dst==HI_LO && (_mc->_de->rs_num==LO || _mc->_de->rs_num==HI))||(prev_me_write_reg && prev_me_decoded_dst == _mc->_de->rs_num)) _mc->_de->MEM_EX_1= TRUE;
         }
         if (_mc->_de->rt_num) {
            if (prev_me_write_reg && prev_me_decoded_dst == _mc->_de->rt_num) _mc->_de->MEM_EX_2= TRUE;
         }
         //No need for MEM-MEM
         _mc->lock_pipeline_data = FALSE;
         if(_mc->_de->_isSyscall){
            _mc->lock_pipeline_sys = TRUE;
            _mc->_isSyscall = TRUE;
         }
         else if(prev_ex_mem_control && prev_ex_write_reg && ((_mc->_de->rs_num!=0 && (_mc->_de->rs_num == prev_ex_decoded_dst)) || (_mc->_de->rt_num!=0 && (_mc->_de->rt_num == prev_ex_decoded_dst)))){ 
            _mc->_de->store_ins = _mc->_de->_ins;
            _mc->_de->store_pc = _mc->_de->_pc;
            _mc->lock_pipeline_data = TRUE;
            _mc->_de->_ins = 0;
            _mc->_de->Dec(_mc , _mc->_em , _mc->_mw , _mc->_de->_ins);
         }
         else if(_mc->_de->_isIllegalOp){
            printf("isIllegalOP\n");
         }
#ifdef MIPC_DEBUG
         fprintf(_mc->_debugLog, "<%llu> Decoded ins %#x\n", SIM_TIME, ins);
#endif
      }
      else {
         PAUSE(1);
      }
   }
}
