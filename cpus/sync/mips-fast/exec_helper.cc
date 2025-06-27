#include <math.h>
#include "mips.h"
#include "opcodes.h"
#include <assert.h>
#include "app_syscall.h"
#include "pipeline.h"
#define MIPC_DEBUG
#define MAGIC_EXEC 0xdeadbeef
/*------------------------------------------------------------------------
 *
 *  Instruction exec 
 *
 *------------------------------------------------------------------------
 */
void
DE::Dec (Mipc *_mc, EM*_em, MW*_mw, unsigned int ins)
{
   MipsInsn i;
   signed int a1, a2;
   unsigned int ar1, ar2, s1, s2, r1, r2, t1, t2;
   LL addr;
   unsigned int val;
   LL value, mask;
   int sa,j;
   Word dummy;

   _isIllegalOp = FALSE;
   _isSyscall = FALSE;

   i.data = ins;
  
#define SIGN_EXTEND_BYTE(x)  do { x <<= 24; x >>= 24; } while (0)
#define SIGN_EXTEND_IMM(x)   do { x <<= 16; x >>= 16; } while (0)

   switch (i.reg.op) {
   case 0:
      // SPECIAL (ALU format)
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = _mc->_gpr[i.reg.rt];
      _decodedDST = i.reg.rd;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      rs_num=i.reg.rs;
      rt_num=i.reg.rt;

      switch (i.reg.func) {
      case 0x20:			// add
      case 0x21:			// addu
         _opControl = _em->func_add_addu;
	 break;

      case 0x24:			// and
         _opControl = _em->func_and;
	 break;

      case 0x27:			// nor
         _opControl = _em->func_nor;
	 break;

      case 0x25:			// or
         _opControl = _em->func_or;
	 break;

      case 0:			// sll
         _opControl = _em->func_sll;
         _decodedShiftAmt = i.reg.sa;
	 break;

      case 4:			// sllv
         _opControl = _em->func_sllv;
	 break;

      case 0x2a:			// slt
         _opControl = _em->func_slt;
	 break;

      case 0x2b:			// sltu
         _opControl = _em->func_sltu;
	 break;

      case 0x3:			// sra
         _opControl = _em->func_sra;
         _decodedShiftAmt = i.reg.sa;
	 break;

      case 0x7:			// srav
         _opControl = _em->func_srav;
	 break;

      case 0x2:			// srl
         _opControl = _em->func_srl;
         _decodedShiftAmt = i.reg.sa;
	 break;

      case 0x6:			// srlv
         _opControl = _em->func_srlv;
	 break;

      case 0x22:			// sub
      case 0x23:			// subu
	 // no overflow check
         _opControl = _em->func_sub_subu;
	 break;

      case 0x26:			// xor
         _opControl = _em->func_xor;
	 break;
        //rs_num = i.reg.rs; //in old not required
      case 0x1a:			// div
         _opControl = _em->func_div;
         _hiWPort = TRUE;
         _loWPort = TRUE;
         _writeREG = FALSE;
         _writeFREG = FALSE;
	 break;

      case 0x1b:			// divu
         _opControl = _em->func_divu;
         _hiWPort = TRUE;
         _loWPort = TRUE;
         _writeREG = FALSE;
         _writeFREG = FALSE;
	 break;

      case 0x10:			// mfhi
         _opControl = _em->func_mfhi;
	      _hi = _mc->_hi;
         rs_num= HI;
         rt_num= 0;
	 break;

      case 0x12:			// mflo
         _opControl = _em->func_mflo;
	      _lo = _mc->_lo;
         rs_num= LO;
         rt_num=0;
	 break;

      case 0x11:			// mthi
         _opControl = _em->func_mthi;
         _hiWPort = TRUE;
         _writeREG = FALSE;
         _writeFREG = FALSE;
         _decodedDST = HI;
	 break;

      case 0x13:			// mtlo
         _opControl = _em->func_mtlo;
         _loWPort = TRUE;
         _writeREG = FALSE;
         _writeFREG = FALSE;
         _decodedDST = HI;
	 break;

      case 0x18:			// mult
         _opControl = _em->func_mult;
         _hiWPort = TRUE;
         _loWPort = TRUE;
         _writeREG = FALSE;
         _writeFREG = FALSE;
         _decodedDST = HI;
	 break;

      case 0x19:			// multu
         _opControl = _em->func_multu;
         _hiWPort = TRUE;
         _loWPort = TRUE;
         _writeREG = FALSE;
          _writeFREG = FALSE;
          _decodedDST = HI;
	 break;

      case 9:			// jalr
         _opControl = _em->func_jalr;
         _btgt = _decodedSRC1;
         break;

      case 8:			// jr
         _opControl = _em->func_jr;
         _writeREG = FALSE;
         _writeFREG = FALSE;
         _btgt = _decodedSRC1;
	 break;

      case 0xd:			// await/break
         _opControl = _em->func_await_break;
         _writeREG = FALSE;
         _writeFREG = FALSE;
	 break;

      case 0xc:			// syscall
         _opControl = NULL; // changed it to NULL from func_syscall
         _writeREG = FALSE;
         _writeFREG = FALSE;
         _isSyscall = TRUE;
	 break;

      default:
	 _isIllegalOp = TRUE;
         _writeREG = FALSE;
         _writeFREG = FALSE;
	 break;
      }
      break;	// ALU format

   case 8:			// addi
   case 9:			// addiu
      // ignore overflow: no exceptions
      _opControl = _em->func_addi_addiu;
      _decodedSRC1 = _mc->_gpr[i.imm.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.imm.rt;
      _writeREG = TRUE;
       _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      rs_num=i.imm.rs;
      rt_num=0;
      break;

   case 0xc:			// andi
      _opControl = _em->func_andi;
      _decodedSRC1 = _mc->_gpr[i.imm.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.imm.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      rs_num=i.imm.rs;
      rt_num=0;
      break;

   case 0xf:			// lui
      _opControl = _em->func_lui;
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.imm.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      break;

   case 0xd:			// ori
      _opControl = _em->func_ori;
      _decodedSRC1 = _mc->_gpr[i.imm.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.imm.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      rs_num= i.imm.rs;
      rt_num=0;
      break;

   case 0xa:			// slti
      _opControl = _em->func_slti;
      _decodedSRC1 = _mc->_gpr[i.imm.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.imm.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      rs_num= i.imm.rs;
      rt_num=0;
      break;

   case 0xb:			// sltiu
      _opControl = _em->func_sltiu;
      _decodedSRC1 = _mc->_gpr[i.imm.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.imm.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      rs_num= i.imm.rs;
      rt_num=0;
      break;

   case 0xe:			// xori
      _opControl = _em->func_xori;
      _decodedSRC1 = _mc->_gpr[i.imm.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.imm.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      rs_num= i.imm.rs;
      rt_num=0;
      break;

   case 4:			// beq
      _opControl = _em->func_beq;
      _decodedSRC1 = _mc->_gpr[i.imm.rs];
      _decodedSRC2 = _mc->_gpr[i.imm.rt];
      _branchOffset = i.imm.imm;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      _branchOffset <<= 16; _branchOffset >>= 14; _bd = 1; _btgt = (unsigned)((signed)_pc+_branchOffset+4);
      rs_num= i.imm.rs;
      rt_num= i.imm.rt;
      break;

   case 1:
      // REGIMM
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _branchOffset = i.imm.imm;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      rs_num=i.reg.rs;
      rt_num=0;

      switch (i.reg.rt) {
      case 1:			// bgez
         _opControl = _em->func_bgez;
         _branchOffset <<= 16; _branchOffset >>= 14; _bd = 1; _btgt = (unsigned)((signed)_pc+_branchOffset+4);
	 break;

      case 0x11:			// bgezal
         _opControl = _em->func_bgezal;
         _decodedDST = 31;
         _writeREG = TRUE;
         _branchOffset <<= 16; _branchOffset >>= 14; _bd = 1; _btgt = (unsigned)((signed)_pc+_branchOffset+4);
     break;

      case 0x10:			// bltzal  (if condition true stores return address PC+8 in ra and jumps to offset)
         _opControl = _em->func_bltzal;
         _decodedDST = 31;
         _writeREG = TRUE;
         _branchOffset <<= 16; _branchOffset >>= 14; _bd = 1; _btgt = (unsigned)((signed)_pc+_branchOffset+4);
     break;

      case 0x0:			// bltz
         _opControl = _em->func_bltz;
         _branchOffset <<= 16; _branchOffset >>= 14; _bd = 1; _btgt = (unsigned)((signed)_pc+_branchOffset+4);
	 break;

      default:
	 _isIllegalOp = TRUE;
	 break;
      }
      break;

   case 7:			// bgtz
      _opControl = _em->func_bgtz;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _branchOffset = i.imm.imm;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      _branchOffset <<= 16; _branchOffset >>= 14; _bd = 1; _btgt = (unsigned)((signed)_pc+_branchOffset+4);
      rs_num=i.reg.rs;
      rt_num=0;
      break;

   case 6:			// blez
      _opControl = _em->func_blez;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _branchOffset = i.imm.imm;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      _branchOffset <<= 16; _branchOffset >>= 14; _bd = 1; _btgt = (unsigned)((signed)_pc+_branchOffset+4);
      rs_num=i.reg.rs;
      rt_num=0;
      break;

   case 5:			// bne
      _opControl = _em->func_bne;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = _mc->_gpr[i.reg.rt];
      _branchOffset = i.imm.imm;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      _branchOffset <<= 16; _branchOffset >>= 14; _bd = 1; _btgt = (unsigned)((signed)_pc+_branchOffset+4);
      rs_num= i.reg.rs;
      rt_num=i.reg.rt;
      break;

   case 2:			// j
      _opControl = _em->func_j;
      _branchOffset = i.tgt.tgt;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      _btgt = ((_pc+4) & 0xf0000000) | (_branchOffset<<2); _bd = 1;
      break;

   case 3:			// jal
      _opControl = _em->func_jal;
      _branchOffset = i.tgt.tgt;
      _decodedDST = 31;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      _btgt = ((_pc+4) & 0xf0000000) | (_branchOffset<<2); _bd = 1;
      break;

   case 0x20:			// lb  
      _opControl = _em->func_lb;
      _memOp = _mw->mem_lb;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num= i.reg.rs;
      rt_num=0;
      break;

   case 0x24:			// lbu
      _opControl = _em->func_lbu;
      _memOp = _mw->mem_lbu;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num = i.reg.rs;
      rt_num= 0;
      break;

   case 0x21:			// lh
      _opControl = _em->func_lh;
      _memOp = _mw->mem_lh;
      _decodedSRC1 =_mc-> _gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num = i.reg.rs;
      rt_num=0;
      break;

   case 0x25:			// lhu
      _opControl = _em->func_lhu;
      _memOp = _mw->mem_lhu;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num = i.reg.rs;
      rt_num=0;
      break;

   case 0x22:			// lwl
      _opControl = _em->func_lwl;
      _memOp = _mw->mem_lwl;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _subregOperand = _mc->_gpr[i.reg.rt];
      _decodedDST = i.reg.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num = i.reg.rs;
      rt_num = i.reg.rt;
      is_subreg = 1;
      break;

   case 0x23:			// lw
      _opControl = _em->func_lw;
      _memOp = _mw->mem_lw;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num = i.reg.rs;
      rt_num=0;
      break;

   case 0x26:			// lwr
      _opControl = _em->func_lwr;
      _memOp = _mw->mem_lwr;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _subregOperand = _mc->_gpr[i.reg.rt];
      _decodedDST = i.reg.rt;
      _writeREG = TRUE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num = i.reg.rs;
      rt_num = i.reg.rt;
      is_subreg = 1;
      break;

   case 0x31:			// lwc1
      _opControl = _em->func_lwc1;
      _memOp = _mw->mem_lwc1;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = FALSE;
      _writeFREG = TRUE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num = i.reg.rs;
      rt_num=0;
      break;

   case 0x39:			// swc1
      _opControl = _em->func_swc1;
      _memOp = _mw->mem_swc1;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num=i.reg.rs;
      fr_num=0;
      break;

   case 0x28:			// sb
      _opControl = _em->func_sb;
      _memOp = _mw->mem_sb;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num=i.reg.rs;
      rt_num=0;
      break;

   case 0x29:			// sh  store half word
      _opControl = _em->func_sh;
      _memOp = _mw->mem_sh;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num=i.reg.rs;
      rt_num=0;
      break;

   case 0x2a:			// swl
      _opControl = _em->func_swl;
      _memOp = _mw->mem_swl;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num=i.reg.rs;
      rt_num=0;
      break;

   case 0x2b:			// sw
      _opControl = _em->func_sw;
      _memOp = _mw->mem_sw;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num=i.reg.rs;
      rt_num=0;
      break;

   case 0x2e:			// swr
      _opControl = _em->func_swr;
      _memOp = _mw->mem_swr;
      _decodedSRC1 = _mc->_gpr[i.reg.rs];
      _decodedSRC2 = i.imm.imm;
      _decodedDST = i.reg.rt;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = TRUE;
      rs_num=i.reg.rs;
      rt_num=0;
      break;

   case 0x11:			// floating-point
      _fpinst++;
      switch (i.freg.fmt) {
      case 4:			// mtc1
         _opControl = _em->func_mtc1;
         _decodedSRC1 = _mc->_gpr[i.freg.ft];
         _decodedDST = i.freg.fs;
         _writeREG = FALSE;
         _writeFREG = TRUE;
         _hiWPort = FALSE;
         _loWPort = FALSE;
         _memControl = FALSE;
         rs_num=i.freg.ft;
	 break;

      case 0:			// mfc1
         _opControl = _em->func_mfc1;
         _decodedSRC1 = _mc->_fpr[(i.freg.fs)>>1].l[FP_TWIDDLE^((i.freg.fs)&1)];
         _decodedDST = i.freg.ft;
         _writeREG = TRUE;
         _writeFREG = FALSE;
         _hiWPort = FALSE;
         _loWPort = FALSE;
         _memControl = FALSE;
         fr_num=i.freg.ft;
	 break;
      default:
         _isIllegalOp = TRUE;
         _writeREG = FALSE;
         _writeFREG = FALSE;
         _hiWPort = FALSE;
         _loWPort = FALSE;
         _memControl = FALSE;
	 break;
      }
      break;
   default:
      _isIllegalOp = TRUE;
      _writeREG = FALSE;
      _writeFREG = FALSE;
      _hiWPort = FALSE;
      _loWPort = FALSE;
      _memControl = FALSE;
      break;
   }
}


/*
 *
 * Debugging: print registers
 *
 */
void 
Mipc::dumpregs (void)
{
   int i;

   printf ("\n--- PC = %08x ---\n", _pc);
   for (i=0; i < 32; i++) {
      if (i < 10)
	 printf (" r%d: %08x (%ld)\n", i, _gpr[i], _gpr[i]);
      else
	 printf ("r%d: %08x (%ld)\n", i, _gpr[i], _gpr[i]);
   }
   printf ("taken: %d, bd: %d\n", _btaken, _bd);
   printf ("target: %08x\n", _btgt);
}

void
EM::func_add_addu (EM *mc, unsigned ins)
{
   mc->_opResultLo = (unsigned) (mc->_decodedSRC1 + mc->_decodedSRC2);
}

void
EM::func_and (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 & mc->_decodedSRC2;
}

void
EM::func_nor (EM *mc, unsigned ins)
{
   mc->_opResultLo = ~(mc->_decodedSRC1 | mc->_decodedSRC2);
}

void
EM::func_or (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 | mc->_decodedSRC2;
}

void
EM::func_sll (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC2 << mc->_decodedShiftAmt;
}

void
EM::func_sllv (EM *mc, unsigned ins)
{
   //printf("unimplemented\n")
   mc->_opResultLo = mc->_decodedSRC2 << (mc->_decodedSRC1 & 0x1f);
}

void
EM::func_slt (EM *mc, unsigned ins)
{
   if (mc->_decodedSRC1 < mc->_decodedSRC2) {
      mc->_opResultLo = 1;
   }
   else {
      mc->_opResultLo = 0;
   }
}

void
EM::func_sltu (EM *mc, unsigned ins)
{
   if ((unsigned)mc->_decodedSRC1 < (unsigned)mc->_decodedSRC2) {
      mc->_opResultLo = 1;
   }
   else {
      mc->_opResultLo = 0;
   }
}

void
EM::func_sra (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC2 >> mc->_decodedShiftAmt;
}

void
EM::func_srav (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC2 >> (mc->_decodedSRC1 & 0x1f);
}

void
EM::func_srl (EM *mc, unsigned ins)
{
   mc->_opResultLo = (unsigned)mc->_decodedSRC2 >> mc->_decodedShiftAmt;
}

void
EM::func_srlv (EM *mc, unsigned ins)
{
   mc->_opResultLo = (unsigned)mc->_decodedSRC2 >> (mc->_decodedSRC1 & 0x1f);
}

void
EM::func_sub_subu (EM *mc, unsigned ins)
{
   mc->_opResultLo = (unsigned)mc->_decodedSRC1 - (unsigned)mc->_decodedSRC2;
}

void
EM::func_xor (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 ^ mc->_decodedSRC2;
}

void
EM::func_div (EM *mc, unsigned ins)
{
   if (mc->_decodedSRC2 != 0) {
      mc->_opResultHi = (unsigned)(mc->_decodedSRC1 % mc->_decodedSRC2);
      mc->_opResultLo = (unsigned)(mc->_decodedSRC1 / mc->_decodedSRC2);
   }
   else {
      mc->_opResultHi = 0x7fffffff;
      mc->_opResultLo = 0x7fffffff;
   }
}

void
EM::func_divu (EM *mc, unsigned ins)
{
   if ((unsigned)mc->_decodedSRC2 != 0) {
      mc->_opResultHi = (unsigned)(mc->_decodedSRC1) % (unsigned)(mc->_decodedSRC2);
      mc->_opResultLo = (unsigned)(mc->_decodedSRC1) / (unsigned)(mc->_decodedSRC2);
   }
   else {
      mc->_opResultHi = 0x7fffffff;
      mc->_opResultLo = 0x7fffffff;
   }
}

void
EM::func_mfhi (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_hi;
}

void
EM::func_mflo (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_lo;
}

void
EM::func_mthi (EM *mc, unsigned ins)
{
   mc->_opResultHi = mc->_decodedSRC1;
}

void
EM::func_mtlo (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1;
}

void
EM::func_mult (EM *mc, unsigned ins)
{
   unsigned int ar1, ar2, s1, s2, r1, r2, t1, t2;
                                                                                
   ar1 = mc->_decodedSRC1;
   ar2 = mc->_decodedSRC2;
   s1 = ar1 >> 31; if (s1) ar1 = 0x7fffffff & (~ar1 + 1);
   s2 = ar2 >> 31; if (s2) ar2 = 0x7fffffff & (~ar2 + 1);
                                                                                
   t1 = (ar1 & 0xffff) * (ar2 & 0xffff);
   r1 = t1 & 0xffff;              // bottom 16 bits
                                                                                
   // compute next set of 16 bits
   t1 = (ar1 & 0xffff) * (ar2 >> 16) + (t1 >> 16);
   t2 = (ar2 & 0xffff) * (ar1 >> 16);
                                                                                
   r1 = r1 | (((t1+t2) & 0xffff) << 16); // bottom 32 bits
   r2 = (ar1 >> 16) * (ar2 >> 16) + (t1 >> 16) + (t2 >> 16) +
            (((t1 & 0xffff) + (t2 & 0xffff)) >> 16);
                                                                                
   if (s1 ^ s2) {
      r1 = ~r1;
      r2 = ~r2;
      r1++;
      if (r1 == 0)
         r2++;
   }
   mc->_opResultHi = r2;
   mc->_opResultLo = r1;
}

void
EM::func_multu (EM *mc, unsigned ins)
{
   unsigned int ar1, ar2, s1, s2, r1, r2, t1, t2;
                                                                                
   ar1 = mc->_decodedSRC1;
   ar2 = mc->_decodedSRC2;
                                                                                
   t1 = (ar1 & 0xffff) * (ar2 & 0xffff);
   r1 = t1 & 0xffff;              // bottom 16 bits
                                                                                
   // compute next set of 16 bits
   t1 = (ar1 & 0xffff) * (ar2 >> 16) + (t1 >> 16);
   t2 = (ar2 & 0xffff) * (ar1 >> 16);
                                                                                
   r1 = r1 | (((t1+t2) & 0xffff) << 16); // bottom 32 bits
   r2 = (ar1 >> 16) * (ar2 >> 16) + (t1 >> 16) + (t2 >> 16) +
            (((t1 & 0xffff) + (t2 & 0xffff)) >> 16);
                            
   mc->_opResultHi = r2;
   mc->_opResultLo = r1;                                                    
}

void
EM::func_jalr (EM *mc, unsigned ins)
{
   mc->_btaken = 1;
   mc->_num_jal++;
   mc->_opResultLo = mc->_pc + 8;
   //add
   //mc->is_branch = 1;
   mc->_btgt = mc->_decodedSRC1;
   //end
}

void
EM::func_jr (EM *mc, unsigned ins)
{
   mc->_btaken = 1;
   mc->_num_jr++;
   //add
   //mc->is_branch = 1;
   mc->_btgt = mc->_decodedSRC1;
   //end
}

void
EM::func_await_break (EM *mc, unsigned ins)
{
}

void
Mipc::func_syscall (Mipc *mc, unsigned ins)
{  

   mc->fake_syscall (ins);
}

void
EM::func_addi_addiu (EM *mc, unsigned ins)
{
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_opResultLo = (unsigned) (mc->_decodedSRC1 + mc->_decodedSRC2);
}

void
EM::func_andi (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 & mc->_decodedSRC2;
}

void
EM::func_lui (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC2 << 16;
}

void
EM::func_ori (EM *mc, unsigned ins)
{

   mc->_opResultLo = mc->_decodedSRC1 | mc->_decodedSRC2;
}

void
EM::func_slti (EM *mc, unsigned ins)
{
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   if (mc->_decodedSRC1 < mc->_decodedSRC2) {
      mc->_opResultLo = 1;
   }
   else {
      mc->_opResultLo = 0;
   }
}

void
EM::func_sltiu (EM *mc, unsigned ins)
{
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   if ((unsigned)mc->_decodedSRC1 < (unsigned)mc->_decodedSRC2) {
      mc->_opResultLo = 1;
   }
   else {
      mc->_opResultLo = 0;
   }
}

void
EM::func_xori (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1 ^ mc->_decodedSRC2;
}

void
EM::func_beq (EM *mc, unsigned ins)
{
   //printf("beq\n");
   //exit(0);
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 == mc->_decodedSRC2) ? 1: 0;
}

void
EM::func_bgez (EM *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = !(mc->_decodedSRC1 >> 31);
}

void
EM::func_bgezal (EM *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = !(mc->_decodedSRC1 >> 31);
   mc->_opResultLo = mc->_pc + 8;
}

void
EM::func_bltzal (EM *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 >> 31);
   mc->_opResultLo = mc->_pc + 8;
}

void
EM::func_bltz (EM *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 >> 31);
}

void
EM::func_bgtz (EM *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 > 0);
}

void
EM::func_blez (EM *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 <= 0);
}

void
EM::func_bne (EM *mc, unsigned ins)
{
   mc->_num_cond_br++;
   mc->_btaken = (mc->_decodedSRC1 != mc->_decodedSRC2);
}

void
EM::func_j (EM *mc, unsigned ins)
{
   mc->_btaken = 1;
}

void
EM::func_jal (EM *mc, unsigned ins)
{
   mc->_num_jal++;
   mc->_btaken = 1;
   mc->_opResultLo = mc->_pc + 8;
}

void
EM::func_lb (EM *mc, unsigned ins)
{
   signed int a1;

   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_lbu (EM *mc, unsigned ins)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_lh (EM *mc, unsigned ins)
{
   signed int a1;
                                                                                
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_lhu (EM *mc, unsigned ins)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_lwl (EM *mc, unsigned ins)
{
   signed int a1;
   unsigned s1;
                                                                                
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_lw (EM *mc, unsigned ins)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1 + mc->_decodedSRC2);
}

void
EM::func_lwr (EM *mc, unsigned ins)
{
   unsigned ar1, s1;
                                                                                
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_lwc1 (EM *mc, unsigned ins)
{
   mc->_num_load++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_swc1 (EM *mc, unsigned ins)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_sb (EM *mc, unsigned ins)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_sh (EM *mc, unsigned ins)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_swl (EM *mc, unsigned ins)
{
   unsigned ar1, s1;
                                                                                
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_sw (EM *mc, unsigned ins)
{
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_swr (EM *mc, unsigned ins)
{
   unsigned ar1, s1;
                                                                                
   mc->_num_store++;
   SIGN_EXTEND_IMM(mc->_decodedSRC2);
   mc->_memory_addr_reg = (unsigned)(mc->_decodedSRC1+mc->_decodedSRC2);
}

void
EM::func_mtc1 (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1;
}

void
EM::func_mfc1 (EM *mc, unsigned ins)
{
   mc->_opResultLo = mc->_decodedSRC1;
}



void
MW::mem_lb (Mipc *mc, MW *mw)
{
   signed int a1;

   a1 = mc->_mem->BEGetByte(mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
   SIGN_EXTEND_BYTE(a1);
   mw->_opResultLo = a1;
}

void
MW::mem_lbu (Mipc *mc, MW *mw)
{
   mw->_opResultLo = mc->_mem->BEGetByte(mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
}

void
MW::mem_lh (Mipc *mc, MW *mw)
{
   signed int a1;

   a1 = mc->_mem->BEGetHalfWord(mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
   SIGN_EXTEND_IMM(a1);
   mw->_opResultLo = a1;
}

void
MW::mem_lhu (Mipc *mc, MW *mw)
{
   mw->_opResultLo = mc->_mem->BEGetHalfWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
}

void
MW::mem_lwl (Mipc *mc, MW *mw)
{
   signed int a1;
   unsigned s1;

   a1 = mc->_mem->BEGetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
   s1 = (mw->_memory_addr_reg & 3) << 3;
   mw->_opResultLo = (a1 << s1) | (mw->_subregOperand & ~(~0UL << s1));
}

void
MW::mem_lw (Mipc *mc , MW *mw)
{
   mw->_opResultLo = mc->_mem->BEGetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
}

void
MW::mem_lwr (Mipc *mc, MW *mw)
{
   unsigned ar1, s1;

   ar1 = mc->_mem->BEGetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
   s1 = (~mw->_memory_addr_reg & 3) << 3;
   mw->_opResultLo = (ar1 >> s1) | (mw->_subregOperand & ~(~(unsigned)0 >> s1));
}

void
MW::mem_lwc1 (Mipc *mc, MW *mw)
{
   mw->_opResultLo = mc->_mem->BEGetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
}

void
MW::mem_swc1 (Mipc *mc, MW *mw)
{
   mc->_mem->Write(mw->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7), mc->_fpr[mw->_decodedDST>>1].l[FP_TWIDDLE^(mw->_decodedDST&1)]));
}

void

MW::mem_sb (Mipc *mc, MW *mw)
{
   mc->_mem->Write(mw->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetByte (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7), mc->_gpr[mw->_decodedDST] & 0xff));
}

void
MW::mem_sh (Mipc *mc, MW *mw)
{
   mc->_mem->Write(mw->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetHalfWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7), mc->_gpr[mw->_decodedDST] & 0xffff));
}

void
MW::mem_swl (Mipc *mc , MW *mw)
{
   unsigned ar1, s1;

   ar1 = mc->_mem->BEGetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
   s1 = (mw->_memory_addr_reg & 3) << 3;
   ar1 = (mc->_gpr[mw->_decodedDST] >> s1) | (ar1 & ~(~(unsigned)0 >> s1));
   mc->_mem->Write(mw->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7), ar1));
}

void
MW::mem_sw (Mipc *mc , MW *mw)
{
   mc->_mem->Write(mw->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7), mc->_gpr[mw->_decodedDST]));
}

void
MW::mem_swr (Mipc *mc , MW *mw)
{
   unsigned ar1, s1;

   ar1 = mc->_mem->BEGetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7));
   s1 = (~mw->_memory_addr_reg & 3) << 3;
   ar1 = (mc->_gpr[mw->_decodedDST] << s1) | (ar1 & ~(~0UL << s1));
   mc->_mem->Write(mw->_memory_addr_reg & ~(LL)0x7, mc->_mem->BESetWord (mw->_memory_addr_reg, mc->_mem->Read(mw->_memory_addr_reg & ~(LL)0x7), ar1));
}
