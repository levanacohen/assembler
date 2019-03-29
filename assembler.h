#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include "stack.h"
#include "program.h"
#include "memory.h"
#include <stdio.h>

struct assembler_t{
    Stack stack_in;
    Memory memoryCode;
    Memory memoryData;
    Program program_in;
};

typedef struct assembler_t * Assembler;

typedef enum {
    ASSEMBLER_SUCCESS,
    ASSEMBLER_FIRST_PASS_ERROR,
    ASSEMBLER_SECOND_PASS_ERROR,
    ASSEMBLER_GET_CELL_ERROR,
    ASSEMBLER_MEMORY_INVALID,
    ASSEMBLER_STACK_ERROR
}AssemblerResult;

/**

**/
Assembler assemblerCreate(FILE * file_in, char * file_name, AssemblerResult* assembler_result_in);

/**
program second pass
prints error
prints output
create files
**/
AssemblerResult assemblerRun(Assembler assmbler_in, char * fileName);


#endif /* _ASSEMBLER_H */
