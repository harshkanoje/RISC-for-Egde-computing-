#include "bus.h"

uint32_t bus_load(BUS *bus, uint32_t addr, uint32_t size){
	return dram_load(&(bus->dram), addr, size);
}

void bus_store(BUS *bus, uint32_t addr, uint32_t size, uint32_t data){
	dram_store(&(bus->dram), addr, size, data);
}