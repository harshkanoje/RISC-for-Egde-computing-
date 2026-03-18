#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include "bus.h"

typedef struct CPU
{
	uint32_t regs[32];
	uint32_t pc;
	uint32_t csr[4096];
	struct BUS bus;
	int running;
} CPU;


void cpu_init(struct CPU *cpu);
uint32_t cpu_fetch(struct CPU *cpu);
uint32_t cpu_execute(struct CPU *cpu, uint32_t instr);
void dump_registers(struct CPU *cpu);

#endif
