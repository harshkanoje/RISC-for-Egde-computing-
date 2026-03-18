#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "cpu.h"
#include "bus.h"
#include "dram.h"
#include "opcodes.h"

// CPU Initialization ///////

void cpu_init(CPU *cpu)
{
	for (int i = 0; i < 32; i++)
	{
		cpu->regs[i] = 0;
	}

	cpu->regs[2] = DRAM_BASE + DRAM_SIZE;
	cpu->pc = DRAM_BASE;
}

uint32_t cpu_fetch(CPU *cpu)
{
	uint32_t instr = bus_load(&(cpu->bus), cpu->pc, 32);
	return instr;
}

uint32_t cpu_load(CPU *cpu, uint32_t addr, uint32_t size)
{
	return bus_load(&(cpu->bus), addr, size);
}

void cpu_store(CPU *cpu, uint32_t addr, uint32_t size, uint32_t data)
{
	return bus_store(&(cpu->bus), addr, size, data);
}

// Instruction Fetching and Decoding /////////

uint32_t rd(uint32_t instr)
{
	return (instr >> 7) & 0x1f;
}

uint32_t rs1(uint32_t instr)
{
	return (instr >> 15) & 0x1f;
}

uint32_t rs2(uint32_t instr)
{
	return (instr >> 20) & 0x1f;
}

// I-Type
uint32_t Imm_I(uint32_t instr)
{
	return ((int32_t)(instr & 0xfff00000)) >> 20;
}

// S-Type
uint32_t Imm_S(uint32_t instr)
{
	return ((uint32_t)(instr & 0xfe000000)) >> 20 | ((uint32_t)instr & 0x1f) >> 7;
}

// B-Type
// Recheck - Although fully correct
int32_t Imm_B(uint32_t instr)
{
	return (int32_t)(instr & 0x80000000) >> 19 | ((instr & 0x7e000000) >> 20) | ((instr & 0x00000f00) >> 7) | ((instr & 0x00000080) << 4);
}

// U-Type
int32_t Imm_U(uint32_t instr)
{
	return (int32_t)(instr & 0xfffff000);
}

// J-Type
int32_t Imm_J(uint32_t instr)
{
	return ((uint32_t)(instr & 0x80000000) >> 11) | ((instr & 0x7fe00000) >> 20) | ((instr & 0x00100000) >> 9) | (instr & 0x000ff000);
}

// shamt (Shift Amount)
uint32_t shamt(int32_t instr)
{
	return (uint32_t)(Imm_I(instr) & 0x1f);
}

uint32_t csr(uint32_t instr)
{
	return (instr & 0xfff00000) >> 20;
}

uint32_t opcode(uint32_t instr)
{
	return (instr) & 0x1f;
}

// Instruction Execution ////////

// LUI
void LUI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> LUI_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = (int32_t)(instr & 0xfffff000);
}

// AUIPC
void AUIPC_exe(CPU *cpu, uint32_t instr)
{
	printf("==> AUIPC_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_U(instr);
	cpu->regs[rd(instr)] = ((int32_t)cpu->pc + (int32_t)imm) - 4;
}

// ADDI
void ADDI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> ADDI_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] + (int32_t)imm;
}

// SLTI
void SLTI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SLTI_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	cpu->regs[rd(instr)] = ((int32_t)cpu->regs[rs1(instr)] < (int32_t)imm) ? 1 : 0;
}

// SLTIU
void SLTIU_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SLTIU_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	cpu->regs[rd(instr)] = (cpu->regs[rs1(instr)] < imm) ? 1 : 0;
}

// XORI
void XORI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> XORI_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] ^ imm;
}

// ORI
void ORI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> ORI_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] | imm;
}

// ANDI
void ANDI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> ANDI_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] & imm;
}

// SLLI
void SLLI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SLLI_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] << shamt(instr);
}

// SRLI
void SRLI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SRLI_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] >> shamt(instr);
}

// SRAI
void SRAI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SRAI_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = (int32_t)cpu->regs[rs1(instr)] >> shamt(instr);
}

// ADD
void ADD_exe(CPU *cpu, uint32_t instr)
{
	printf("==> ADD_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] + cpu->regs[rs2(instr)];
}

// SUB
void SUB_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SUB_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] - cpu->regs[rs2(instr)];
}

// SLL
void SLL_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SLL_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] << (cpu->regs[rs2(instr)] & 0x1f);
}

// SLT
void SLT_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SLT_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = ((int32_t)cpu->regs[rs1(instr)] < (int32_t)cpu->regs[rs2(instr)]) ? 1 : 0;
}

// SLTU
void SLTU_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SLTU_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = (cpu->regs[rs1(instr)] < cpu->regs[rs2(instr)]) ? 1 : 0;
}

// XOR
void XOR_exe(CPU *cpu, uint32_t instr)
{
	printf("==> XOR_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] ^ cpu->regs[rs2(instr)];
}

// SRL
void SRL_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SRL_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] >> (cpu->regs[rs2(instr)] & 0x1f);
}

// SRA
void SRA_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SRA_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] =
		(int32_t)cpu->regs[rs1(instr)] >> (cpu->regs[rs2(instr)] & 0x1f);
}

// OR
void OR_exe(CPU *cpu, uint32_t instr)
{
	printf("==> OR_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] | cpu->regs[rs2(instr)];
}

// AND
void AND_exe(CPU *cpu, uint32_t instr)
{
	printf("==> AND_exe called, instr=0x%08x\n", instr);
	cpu->regs[rd(instr)] = cpu->regs[rs1(instr)] & cpu->regs[rs2(instr)];
}

// FENCE
void FENCE_exe(CPU *cpu, uint32_t instr)
{
	printf("==> FENCE_exe called, instr=0x%08x\n", instr);
}

// FENCE.i
void FENCEi_exe(CPU *cpu, uint32_t instr)
{
	printf("==> FENCEi_exe called, instr=0x%08x\n", instr);
}

// CSRRW
void CSRRW_exe(CPU *cpu, uint32_t instr)
{
	printf("==> CSRRW_exe called, instr=0x%08x\n", instr);
	uint32_t temp = cpu->csr[csr(instr)];
	cpu->csr[csr(instr)] = cpu->regs[rs1(instr)];

	if (rd(instr))
	{
		cpu->regs[rd(instr)] = temp;
	}
}

// CSRRS
void CSRRS_exe(CPU *cpu, uint32_t instr)
{
	printf("==> CSRRS_exe called, instr=0x%08x\n", instr);
	uint32_t temp = cpu->csr[csr(instr)];
	if (rs1(instr))
	{
		cpu->csr[csr(instr)] = temp | cpu->regs[rs1(instr)];
	}
	cpu->regs[rd(instr)] = temp;
}

// CSRRC
void CSRRC_exe(CPU *cpu, uint32_t instr)
{
	printf("==> CSRRC_exe called, instr=0x%08x\n", instr);
	uint32_t temp = cpu->csr[csr(instr)];
	if (rs1(instr))
	{
		cpu->csr[csr(instr)] = temp & ~cpu->regs[rs1(instr)];
	}
	cpu->regs[rd(instr)] = temp;
}

// CSRRWI
void CSRRWI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> CSRRWI_exe called, instr=0x%08x\n", instr);
	uint32_t temp = rs1(instr);
	if (rd(instr))
	{
		cpu->regs[rd(instr)] = cpu->csr[csr(instr)];
	}
	cpu->csr[csr(instr)] = temp;
}

// CSRRSI
void CSRRSI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> CSRRSI_exe called, instr=0x%08x\n", instr);
	uint32_t temp = cpu->csr[csr(instr)];
	if (rs1(instr))
	{
		cpu->csr[csr(instr)] = temp | rs1(instr);
	}

	if (rd(instr))
	{
		cpu->regs[rd(instr)] = temp;
	}
}

// CSRRCI
void CSRRCI_exe(CPU *cpu, uint32_t instr)
{
	printf("==> CSRRCI_exe called, instr=0x%08x\n", instr);
	uint32_t temp = cpu->csr[csr(instr)];
	if (rs1(instr))
	{
		cpu->csr[csr(instr)] = temp & ~rs1(instr);
	}

	if (rd(instr))
	{
		cpu->regs[rd(instr)] = temp;
	}
}

// ECALL
void ECALL_exe(CPU *cpu, uint32_t instr)
{
	printf("\nECALL encountered\n");

	dump_registers(cpu);

	if (cpu->regs[10] == 0)
		printf("\nTEST PASS\n");
	else
		printf("\nTEST FAIL: %d\n", cpu->regs[10]);

	exit(0);
}

// EBREAK
void EBREAK_exe(CPU *cpu, uint32_t instr)
{
 	printf("EBREAK encountered\n");
 	cpu->running = 0;   // stop execution
	printf("\nTEST PASS\n");

}

// URET
void URET_exe(CPU *cpu, uint32_t instr) {}

// SRET
void SRET_exe(CPU *cpu, uint32_t instr) {}

// MRET
void MRET_exe(CPU *cpu, uint32_t instr) {}

// WFI
void WFI_exe(CPU *cpu, uint32_t instr) {}

// SFENCE.VMA
void SFENCE_VMA_exe(CPU *cpu, uint32_t instr) {}

// LB
void LB_exe(CPU *cpu, uint32_t instr)
{
	printf("==> LB_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	uint32_t addr = cpu->regs[rs1(instr)] + (int32_t)imm;
	cpu->regs[rd(instr)] = (int32_t)(int8_t)cpu_load(cpu, addr, 8);
}

// LH
void LH_exe(CPU *cpu, uint32_t instr)
{
	printf("==> LH_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	uint32_t addr = cpu->regs[rs1(instr)] + (int32_t)imm;
	cpu->regs[rd(instr)] = (int32_t)(int16_t)cpu_load(cpu, addr, 16);
}

// LW
void LW_exe(CPU *cpu, uint32_t instr)
{
	printf("==> LW_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	uint32_t addr = cpu->regs[rs1(instr)] + (int32_t)imm;
	cpu->regs[rd(instr)] = (int32_t)cpu_load(cpu, addr, 32);
}

// LBU
void LBU_exe(CPU *cpu, uint32_t instr)
{
	printf("==> LBU_exe called, instr=0x%08x\n", instr);

	uint32_t imm = Imm_I(instr);
	uint32_t addr = cpu->regs[rs1(instr)] + (int32_t)imm;
	cpu->regs[rd(instr)] = cpu_load(cpu, addr, 8);
}

// LHU
void LHU_exe(CPU *cpu, uint32_t instr)
{
	printf("==> LHU_exe called, instr=0x%08x\n", instr);

	uint32_t imm = Imm_I(instr);
	uint32_t addr = cpu->regs[rs1(instr)] + (int32_t)imm;
	cpu->regs[rd(instr)] = cpu_load(cpu, addr, 16);
}

// SB
void SB_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SB_exe called, instr=0x%08x\n", instr);

	uint32_t imm = Imm_S(instr);
	uint32_t addr = cpu->regs[rs1(instr)] + (int32_t)imm;
	cpu_store(cpu, addr, 8, cpu->regs[rs2(instr)]);
}

// SH
void SH_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SH_exe called, instr=0x%08x\n", instr);

	uint32_t imm = Imm_S(instr);
	uint32_t addr = cpu->regs[rs1(instr)] + (int32_t)imm;
	cpu_store(cpu, addr, 16, cpu->regs[rs2(instr)]);
}

// SW
void SW_exe(CPU *cpu, uint32_t instr)
{
	printf("==> SW_exe called, instr=0x%08x\n", instr);

	uint32_t imm = Imm_S(instr);
	uint32_t addr = cpu->regs[rs1(instr)] + (int32_t)imm;
	cpu_store(cpu, addr, 32, cpu->regs[rs2(instr)]);
}

// JAL
void JAL_exe(CPU *cpu, uint32_t instr)
{
	printf("==> JAL_exe called, instr=0x%08x\n", instr);

	uint32_t imm = Imm_J(instr);
	cpu->regs[rd(instr)] = cpu->pc;
	cpu->pc += imm - 4;
}

// JALR
void JALR_exe(CPU *cpu, uint32_t instr)
{
	printf("==> JALR_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_I(instr);
	uint32_t temp = cpu->pc;
	cpu->pc = (cpu->regs[rs1(instr)] + imm - 4) & 0xfffffffe;
	cpu->regs[rd(instr)] = temp;
}

// BEQ
void BEQ_exe(CPU *cpu, uint32_t instr)
{
	printf("==> BEQ_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_B(instr);
	if (cpu->regs[rs1(instr)] == cpu->regs[rs2(instr)])
	{
		cpu->pc += (int32_t)imm;
	}
}

// BNE
void BNE_exe(CPU *cpu, uint32_t instr)
{
	printf("==> BNE_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_B(instr);
	if (cpu->regs[rs1(instr)] != cpu->regs[rs2(instr)])
	{
		cpu->pc += (int32_t)imm - 4;
	}
}

// BLT
void BLT_exe(CPU *cpu, uint32_t instr)
{
	printf("==> BLT_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_B(instr);
	if ((int32_t)cpu->regs[rs1(instr)] < (int32_t)cpu->regs[rs2(instr)])
	{
		cpu->pc += (int32_t)imm - 4;
	}
}

// BGE
void BGE_exe(CPU *cpu, uint32_t instr)
{
	printf("==> BGE_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_B(instr);
	if ((int32_t)cpu->regs[rs1(instr)] >= (int32_t)cpu->regs[rs2(instr)])
	{
		cpu->pc += (int32_t)imm - 4;
	}
}

// BLTU

void BLTU_exe(CPU *cpu, uint32_t instr)
{
	printf("==> BLTU_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_B(instr);
	if (cpu->regs[rs1(instr)] < cpu->regs[rs2(instr)])
	{
		cpu->pc += imm - 4;
	}
}

// BGEU
void BGEU_exe(CPU *cpu, uint32_t instr)
{
	printf("==> BGEU_exe called, instr=0x%08x\n", instr);
	uint32_t imm = Imm_B(instr);
	if (cpu->regs[rs1(instr)] >= cpu->regs[rs2(instr)])
	{
		cpu->pc += imm - 4;
	}
}

///// CPU Execution /////

uint32_t cpu_execute(CPU *cpu, uint32_t instr)
{
	int opcode = instr & 0x7f;
	int func3 = (instr & 0x00007000) >> 12;
	int func7 = (instr >> 25) & 0x7f;

	cpu->regs[0] = 0;

	switch (opcode)
	{
	case OP_LUI:
		LUI_exe(cpu, instr);
		break;
	case OP_AUIPC:
		AUIPC_exe(cpu, instr);
		break;
	case OP_JAL:
		JAL_exe(cpu, instr);
		break;
	case OP_JALR:
		JALR_exe(cpu, instr);
		break;

	case OP_BRANCH:
		switch (func3)
		{
		case F3_BEQ:
			BEQ_exe(cpu, instr);
			break;
		case F3_BNE:
			BNE_exe(cpu, instr);
			break;
		case F3_BLT:
			BLT_exe(cpu, instr);
			break;
		case F3_BGE:
			BGE_exe(cpu, instr);
			break;
		case F3_BLTU:
			BLTU_exe(cpu, instr);
			break;
		case F3_BGEU:
			BGEU_exe(cpu, instr);
			break;
		}
		break;

	case OP_LOAD:
		switch (func3)
		{
		case F3_LB:
			LB_exe(cpu, instr);
			break;
		case F3_LH:
			LH_exe(cpu, instr);
			break;
		case F3_LW:
			LW_exe(cpu, instr);
			break;
		case F3_LBU:
			LBU_exe(cpu, instr);
			break;
		case F3_LHU:
			LHU_exe(cpu, instr);
			break;
		}
		break;

	case OP_STORE:
		switch (func3)
		{
		case F3_SB:
			SB_exe(cpu, instr);
			break;
		case F3_SH:
			SH_exe(cpu, instr);
			break;
		case F3_SW:
			SW_exe(cpu, instr);
			break;
		}
		break;

	case OP_OP_IMM: // Immediate arithmetic
		switch (func3)
		{
		case F3_ADDI:
			ADDI_exe(cpu, instr);
			break;
		case F3_SLTI:
			SLTI_exe(cpu, instr);
			break;
		case F3_SLTIU:
			SLTIU_exe(cpu, instr);
			break;
		case F3_XORI:
			XORI_exe(cpu, instr);
			break;
		case F3_ORI:
			ORI_exe(cpu, instr);
			break;
		case F3_ANDI:
			ANDI_exe(cpu, instr);
			break;
		case F3_SLLI:
			SLLI_exe(cpu, instr);
			break;
		case F3_SRLI_SRAI:
			switch (func7)
			{
			case F7_SRLI:
				SRLI_exe(cpu, instr);
				break;
			case F7_SRAI:
				SRAI_exe(cpu, instr);
				break;
			}
			break;
		}
		break;

	case OP_OP: // Register-register operations
		switch (func3)
		{
		case F3_ADD_SUB:
			switch (func7)
			{
			case F7_ADD:
				ADD_exe(cpu, instr);
				break;
			case F7_SUB:
				SUB_exe(cpu, instr);
				break;
			}
			break;
		case F3_SLL:
			SLL_exe(cpu, instr);
			break;
		case F3_SLT:
			SLT_exe(cpu, instr);
			break;
		case F3_SLTU:
			SLTU_exe(cpu, instr);
			break;
		case F3_XOR:
			XOR_exe(cpu, instr);
			break;
		case F3_SRL_SRA:
			switch (func7)
			{
			case F7_SRL:
				SRL_exe(cpu, instr);
				break;
			case F7_SRA:
				SRA_exe(cpu, instr);
				break;
			}
			break;
		case F3_OR:
			OR_exe(cpu, instr);
			break;
		case F3_AND:
			AND_exe(cpu, instr);
			break;
		}
		break;

	case OP_SYSTEM:
		switch (func3)
		{
		case F3_PRIV:
			// Extract funct12 for ECALL/EBREAK/WFI
			int funct12 = (instr >> 20) & 0xfff;
			switch (funct12)
			{
			case F12_ECALL:
				ECALL_exe(cpu, instr);
				break;
			case F12_EBREAK:
				EBREAK_exe(cpu, instr);
				break;
			case F12_WFI:
				WFI_exe(cpu, instr);
				break;
			}
			break;
		case F3_CSRRW:
			CSRRW_exe(cpu, instr);
			break;
		case F3_CSRRS:
			CSRRS_exe(cpu, instr);
			break;
		case F3_CSRRC:
			CSRRC_exe(cpu, instr);
			break;
		case F3_CSRRWI:
			CSRRWI_exe(cpu, instr);
			break;
		case F3_CSRRSI:
			CSRRSI_exe(cpu, instr);
			break;
		case F3_CSRRCI:
			CSRRCI_exe(cpu, instr);
			break;
		}
		break;

	case OP_MISC_MEM:
		switch (func3)
		{
		case F3_FENCE:
			FENCE_exe(cpu, instr);
			break;
		}
		break;
	case OP_CUSTOM0: {
   		int rd  = (instr >> 7) & 0x1F;
   		int rs1 = (instr >> 15) & 0x1F;
   		int rs2 = (instr >> 20) & 0x1F;
   		int func3_local = (instr >> 12) & 0x7;
   		int func7_local = (instr >> 25) & 0x7F;

    		if (func3_local == 0x0 && func7_local == 0x01) {
       			printf("==> MAC_exe called, instr=0x%08x\n", instr);

        		cpu->regs[rd] = cpu->regs[rd] +
                        	(cpu->regs[rs1] * cpu->regs[rs2]);

		}

  		 else {
        		printf("Unknown custom instruction: 0x%08x\n", instr);
    		}

    		break;
		}

	default:
		fprintf(stderr, "Unknown instruction: 0x%08x\n", instr);
		return 0;
	}

	return 1;
}

void dump_registers(CPU *cpu)
{
	char *abi[] = {
		"zero",
		"ra",
		"sp",
		"gp",
		"tp",
		"t0",
		"t1",
		"t2",
		"s0",
		"s1",
		"a0",
		"a1",
		"a2",
		"a3",
		"a4",
		"a5",
		"a6",
		"a7",
		"s2",
		"s3",
		"s4",
		"s5",
		"s6",
		"s7",
		"s8",
		"s9",
		"s10",
		"s11",
		"t3",
		"t4",
		"t5",
		"t6",
	};

	printf("\n=== Register Dump ===\n");
	for (int i = 0; i < 8; i++)
	{
		printf("%4s(x%02d): 0x%08x  ", abi[i], i, cpu->regs[i]);
		printf("%4s(x%02d): 0x%08x  ", abi[i + 8], i + 8, cpu->regs[i + 8]);
		printf("%4s(x%02d): 0x%08x  ", abi[i + 16], i + 16, cpu->regs[i + 16]);
		printf("%4s(x%02d): 0x%08x\n", abi[i + 24], i + 24, cpu->regs[i + 24]);
	}
	printf("\nPC: 0x%08x\n", cpu->pc);
}
