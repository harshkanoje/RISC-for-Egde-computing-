#include "dram.h"
#include <stdio.h>
#include <stdint.h>

uint32_t dram_load(DRAM *dram, uint32_t addr, uint32_t size){
    uint32_t index = addr - DRAM_BASE;
    
    switch(size) {
        case 8:  // Load byte
            return (uint32_t)dram->mem[index];
            
        case 16: // Load halfword (2 bytes, little-endian)
            return (uint32_t)dram->mem[index] |
                   ((uint32_t)dram->mem[index + 1] << 8);
            
        case 32: // Load word (4 bytes, little-endian)
            return (uint32_t)dram->mem[index] |
                   ((uint32_t)dram->mem[index + 1] << 8) |
                   ((uint32_t)dram->mem[index + 2] << 16) |
                   ((uint32_t)dram->mem[index + 3] << 24);
            
        case 64: // Load doubleword (8 bytes, little-endian)
            return (uint64_t)dram->mem[index] |
                   ((uint64_t)dram->mem[index + 1] << 8) |
                   ((uint64_t)dram->mem[index + 2] << 16) |
                   ((uint64_t)dram->mem[index + 3] << 24) |
                   ((uint64_t)dram->mem[index + 4] << 32) |
                   ((uint64_t)dram->mem[index + 5] << 40) |
                   ((uint64_t)dram->mem[index + 6] << 48) |
                   ((uint64_t)dram->mem[index + 7] << 56);
            
        default:
            fprintf(stderr, "Invalid load size: %d\n", size);
            return 0;
    }
}

void dram_store(DRAM *dram, uint32_t addr, uint32_t size, uint32_t data){
    uint32_t index = addr - DRAM_BASE;
    
    switch(size) {
        case 8:  // Store byte
            dram->mem[index] = (uint8_t)(data & 0xff);
            break;
            
        case 16: // Store halfword (2 bytes, little-endian)
            dram->mem[index]     = (uint8_t)(data & 0xff);
            dram->mem[index + 1] = (uint8_t)((data >> 8) & 0xff);
            break;
            
        case 32: // Store word (4 bytes, little-endian)
            dram->mem[index]     = (uint8_t)(data & 0xff);
            dram->mem[index + 1] = (uint8_t)((data >> 8) & 0xff);
            dram->mem[index + 2] = (uint8_t)((data >> 16) & 0xff);
            dram->mem[index + 3] = (uint8_t)((data >> 24) & 0xff);
            break;
            
        case 64: // Store doubleword (8 bytes, little-endian)
            dram->mem[index]     = (uint8_t)(data & 0xff);
            dram->mem[index + 1] = (uint8_t)((data >> 8) & 0xff);
            dram->mem[index + 2] = (uint8_t)((data >> 16) & 0xff);
            dram->mem[index + 3] = (uint8_t)((data >> 24) & 0xff);
            dram->mem[index + 4] = (uint8_t)((data >> 32) & 0xff);
            dram->mem[index + 5] = (uint8_t)((data >> 40) & 0xff);
            dram->mem[index + 6] = (uint8_t)((data >> 48) & 0xff);
            dram->mem[index + 7] = (uint8_t)((data >> 56) & 0xff);
            break;
            
        default:
            fprintf(stderr, "Invalid store size: %d\n", size);
    }
}
