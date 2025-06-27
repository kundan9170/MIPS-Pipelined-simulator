#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "mem.h"
#include "mips.h"
#include "../../common/syscall.h"
class Mipc;
class FD;
class DE;
class EM;
class MW;

typedef unsigned Bool;
typedef unsigned long long LL;

class FD {
public:
    unsigned int _ins;
    unsigned int _pc;
    FD ();
    ~FD ();
};

class DE {
public:
    unsigned int _ins, ins_prev;
    unsigned int _pc, pc_prev;


    signed int	_decodedSRC1, _decodedSRC2, _decodedSRC3;	// Reg fetch output (source values)
   unsigned	_decodedDST;			// Decoder output (dest reg no)
   unsigned 	_subregOperand;			// Needed for lwl and lwr
   //unsigned	_MAR;				// Memory address register
   //unsigned	_opResultHi, _opResultLo;	// Result of operation
   Bool 	_memControl;			// Memory instruction?
   Bool		_writeREG, _writeFREG;		// WB control
   signed int	_branchOffset;
   Bool 	_hiWPort, _loWPort;		// WB control
   unsigned	_decodedShiftAmt;
    
    Bool		_isSyscall;			// 1 if system call
    Bool		_isIllegalOp;			// 1 if illegal opcode

    int 		_bd;				// 1 if the next ins is delay slot
    unsigned int	_btgt;				// branch target

    //new
    unsigned int rs_num, rt_num;
    unsigned int fr_num;
    //end


    void (*_opControl)(EM*, unsigned);
    void (*_memOp)(Mipc*, MW*);

    DE(FD *fd);
    DE();
    ~DE();

    //new
    void Dec (Mipc* mc, EM* em, MW* mw, unsigned int ins);	// Decoder function
    void copy (DE *de);
    //end
};

class EM{
public:

    unsigned int _ins;
    unsigned int _pc;


    signed int	_decodedSRC1, _decodedSRC2, _decodedSRC3;	// Reg fetch output (source values)
   unsigned	_decodedDST;			// Decoder output (dest reg no)
   unsigned 	_subregOperand;			// Needed for lwl and lwr
   unsigned	_MAR;				// Memory address register
   unsigned	_opResultHi, _opResultLo;	// Result of operation
   Bool 	_memControl;			// Memory instruction?
   Bool		_writeREG, _writeFREG;		// WB control
   signed int	_branchOffset;
   Bool 	_hiWPort, _loWPort;		// WB control
   unsigned	_decodedShiftAmt;
    
    Bool		_isSyscall;			// 1 if system call
    Bool		_isIllegalOp;			// 1 if illegal opcode

    int 		_bd;				// 1 if the next ins is delay slot
    unsigned int	_btgt;				// branch target
    unsigned int _hi, _lo;
    int _btaken;

    //new
    unsigned int rt_num;
    unsigned int fr_num;
    //end
    LL	_num_jal;
   LL	_num_jr;
   LL	_num_cond_br;
   LL   _num_load;
   LL   _num_store;

    void (*_opControl)(EM*, unsigned);
    void (*_memOp)(Mipc*, MW*);

    EM(DE *de);
    EM();
    ~EM();
    void copy (EM *em);
    //added syscall
    //void fake_syscall(unsigned int ins);
    // EXE stage definitions

    static void func_add_addu (EM*, unsigned);
    static void func_and (EM*, unsigned);
    static void func_nor (EM*, unsigned);
    static void func_or (EM*, unsigned);
    static void func_sll (EM*, unsigned);
    static void func_sllv (EM*, unsigned);
    static void func_slt (EM*, unsigned);
    static void func_sltu (EM*, unsigned);
    static void func_sra (EM*, unsigned);
    static void func_srav (EM*, unsigned);
    static void func_srl (EM*, unsigned);
    static void func_srlv (EM*, unsigned);
    static void func_sub_subu (EM*, unsigned);
    static void func_xor (EM*, unsigned);
    static void func_div (EM*, unsigned);
    static void func_divu (EM*, unsigned);
    static void func_mfhi (EM*, unsigned);
    static void func_mflo (EM*, unsigned);
    static void func_mthi (EM*, unsigned);
    static void func_mtlo (EM*, unsigned);
    static void func_mult (EM*, unsigned);
    static void func_multu (EM*, unsigned);
    static void func_jalr (EM*, unsigned);
    static void func_jr (EM*, unsigned);
    static void func_await_break (EM*, unsigned);
    static void func_syscall (EM*, unsigned);
    static void func_addi_addiu (EM*, unsigned);
    static void func_andi (EM*, unsigned);
    static void func_lui (EM*, unsigned);
    static void func_ori (EM*, unsigned);
    static void func_slti (EM*, unsigned);
    static void func_sltiu (EM*, unsigned);
    static void func_xori (EM*, unsigned);
    static void func_beq (EM*, unsigned);
    static void func_bgez (EM*, unsigned);
    static void func_bgezal (EM*, unsigned);
    static void func_bltzal (EM*, unsigned);
    static void func_bltz (EM*, unsigned);
    static void func_bgtz (EM*, unsigned);
    static void func_blez (EM*, unsigned);
    static void func_bne (EM*, unsigned);
    static void func_j (EM*, unsigned);
    static void func_jal (EM*, unsigned);
    static void func_lb (EM*, unsigned);
    static void func_lbu (EM*, unsigned);
    static void func_lh (EM*, unsigned);
    static void func_lhu (EM*, unsigned);
    static void func_lwl (EM*, unsigned);
    static void func_lw (EM*, unsigned);
    static void func_lwr (EM*, unsigned);
    static void func_lwc1 (EM*, unsigned);
    static void func_swc1 (EM*, unsigned);
    static void func_sb (EM*, unsigned);
    static void func_sh (EM*, unsigned);
    static void func_swl (EM*, unsigned);
    static void func_sw (EM*, unsigned);
    static void func_swr (EM*, unsigned);
    static void func_mtc1 (EM*, unsigned);
    static void func_mfc1 (EM*, unsigned);
};

class MW {
public:
    unsigned int _pc;
    unsigned int _ins;

    signed int  _decodedSRC3;
    unsigned	_decodedDST;			// Decoder output (dest reg no)
    Bool		_writeREG, _writeFREG;		// WB control
    Bool 	_hiWPort, _loWPort;		// WB control
    Bool 	_memControl;			// Memory instruction?
    unsigned	_opResultHi, _opResultLo;	// Result of operation
    unsigned	_MAR;				// Memory address register
    unsigned 	_subregOperand;			// Needed for lwl and lwr

    Bool		_isSyscall;			// 1 if system call
    Bool		_isIllegalOp;			// 1 if illegal opcode

    void (*_memOp)(Mipc*, MW*);

    MW (EM* em);
    MW ();
    ~MW ();

    void copy (MW *mw);

    // MEM stage definitions

   static void mem_lb (Mipc* mc, MW*);
   static void mem_lbu (Mipc* mc, MW*);
   static void mem_lh (Mipc* mc, MW*);
   static void mem_lhu (Mipc* mc, MW*);
   static void mem_lwl (Mipc* mc, MW*);
   static void mem_lw (Mipc* mc, MW*);
   static void mem_lwr (Mipc* mc, MW*);
   static void mem_lwc1 (Mipc* mc, MW*);
   static void mem_swc1 (Mipc* mc, MW*);
   static void mem_sb (Mipc* mc, MW*);
   static void mem_sh (Mipc* mc, MW*);
   static void mem_swl (Mipc* mc, MW*);
   static void mem_sw (Mipc* mc, MW*);
   static void mem_swr (Mipc* mc, MW*);
};

#endif
