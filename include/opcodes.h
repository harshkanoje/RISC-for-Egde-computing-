#ifndef OPCODES_H
#define OPCODES_H

// PRIMARY OPCODES (bits 6-0)
#define OP_LOAD      0x03    
#define OP_LOAD_FP   0x07    
#define OP_MISC_MEM  0x0F    
#define OP_OP_IMM    0x13    
#define OP_AUIPC     0x17    
#define OP_STORE     0x23    
#define OP_STORE_FP  0x27    
#define OP_OP        0x33    
#define OP_LUI       0x37    
#define OP_BRANCH    0x63    
#define OP_JALR      0x67    
#define OP_JAL       0x6F    
#define OP_SYSTEM    0x73    
#define OP_CUSTOM0  0x0B   

// FUNCT3 VALUES (bits 14-12)

// LOAD (0x03)
#define F3_LB        0x0
#define F3_LH        0x1
#define F3_LW        0x2
#define F3_LBU       0x4
#define F3_LHU       0x5

// STORE (0x23)
#define F3_SB        0x0
#define F3_SH        0x1
#define F3_SW        0x2

// BRANCH (0x63)
#define F3_BEQ       0x0
#define F3_BNE       0x1
#define F3_BLT       0x4
#define F3_BGE       0x5
#define F3_BLTU      0x6
#define F3_BGEU      0x7

// OP_IMM (0x13) - Immediate arithmetic
#define F3_ADDI      0x0
#define F3_SLLI      0x1
#define F3_SLTI      0x2
#define F3_SLTIU     0x3
#define F3_XORI      0x4
#define F3_SRLI_SRAI 0x5
#define F3_ORI       0x6
#define F3_ANDI      0x7

// OP (0x33) - Register-register operations
#define F3_ADD_SUB   0x0
#define F3_SLL       0x1
#define F3_SLT       0x2
#define F3_SLTU      0x3
#define F3_XOR       0x4
#define F3_SRL_SRA   0x5
#define F3_OR        0x6
#define F3_AND       0x7

// MISC_MEM (0x0F) - Memory ordering
#define F3_FENCE     0x0
#define F3_FENCE_I   0x1

// SYSTEM (0x73) - System instructions
#define F3_PRIV      0x0     
#define F3_CSRRW     0x1
#define F3_CSRRS     0x2
#define F3_CSRRC     0x3
#define F3_CSRRWI    0x5
#define F3_CSRRSI    0x6
#define F3_CSRRCI    0x7


// FUNCT7 VALUES (bits 31-25)

// OP (0x33) - Register operations
#define F7_ADD       0x00
#define F7_SUB       0x20
#define F7_SRL       0x00
#define F7_SRA       0x20

// OP_IMM (0x13) - Shift immediates
#define F7_SRLI      0x00
#define F7_SRAI      0x20
#define F7_SLLI      0x00


// FUNCT12 VALUES (bits 31-20)
#define F12_ECALL    0x000
#define F12_EBREAK   0x001
#define F12_URET     0x002
#define F12_SRET     0x102
#define F12_MRET     0x302
#define F12_WFI      0x105
#define F12_SFENCE_VMA 0x009

#endif
