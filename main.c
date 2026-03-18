#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"
#include "bus.h"
#include "dram.h"

void read_BinFile(CPU *cpu, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Unable to open file %s\n", filename);
        exit(1);
    }
    
    fseek(file, 0, SEEK_END);
    long fileLen = ftell(file);
    rewind(file);

    uint8_t *buffer = malloc(fileLen);
    if (!buffer) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(file);
        exit(1);
    }

    fread(buffer, fileLen, 1, file);
    fclose(file);

    // Copy binary to DRAM via bus
    for (int i = 0; i < fileLen; i++) {
        bus_store(&(cpu->bus), DRAM_BASE + i, 8, buffer[i]);
    }

    free(buffer);
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <binary_file>\n", argv[0]);
        return 1;
    }

    CPU cpu;
    
    cpu_init(&cpu);
    read_BinFile(&cpu, argv[1]);

    printf("Starting execution at PC = 0x%08x\n\n", cpu.pc);

    uint64_t instr_count = 0;
    const uint64_t MAX_INSTRUCTIONS = 100000;

    while (instr_count < MAX_INSTRUCTIONS && cpu.running) {
        uint32_t instr = cpu_fetch(&cpu);

        

        if (instr == 0x00000000) {
            printf("\nEncountered null instruction at PC=0x%08x\n", cpu.pc);
            break;
        }
        
        cpu.pc += 4;

        if (!cpu_execute(&cpu, instr)) {
            printf("\nExecution failed at PC=0x%08x\n", cpu.pc - 4); 
            break;
        }

        instr_count++;
    }

    printf("\nExecuted %lu instructions\n", instr_count);
    dump_registers(&cpu);

    return 0;
}
