#ifndef BUS_H
#define BUS_H

#include <stdint.h>
#include "dram.h"

typedef struct BUS{
	struct DRAM dram;
} BUS;

uint32_t bus_load(BUS *bus, uint32_t addr, uint32_t size);
void bus_store(BUS *bus, uint32_t addr, uint32_t size, uint32_t data);

#endif