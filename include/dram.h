#ifndef DRAM_H
#define DRAM_H

#include <stdint.h>

#define DRAM_SIZE (1024*1024)
#define DRAM_BASE 0x80000000

typedef struct DRAM {
	uint8_t mem[DRAM_SIZE];
} DRAM;

uint32_t dram_load(DRAM *dram, uint32_t addr, uint32_t size);

void dram_store(DRAM *dram, uint32_t addr, uint32_t size, uint32_t data);

#endif


