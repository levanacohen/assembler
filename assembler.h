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
Function assemblerCreate:
The first Pass.

Input:
    file_in: the input file .as.
    file_name: the name of the input file without extension .as.
    assembler_result_in: control function.
        - ASSEMBLER_SUCCESS: the input is valid.
        - ASSEMBLER_FIRST_PASS_ERROR: otherwise.

Output:
    - Assembler if the output is valid,
    - NULL otherwise.

**/
Assembler assemblerCreate(FILE * file_in, char * file_name, AssemblerResult* assembler_result_in);

/**
Function assemblerRun:
The second Pass.

Input:
    assembler_in: the input file .as.
    fileName: the name of the input file without extension .as.
    assembler_result_in: control function.
        - ASSEMBLER_SUCCESS: the input is valid.
        - ASSEMBLER_SECOND_PASS_ERROR: otherwise.

Output:
    - Assembler if the output is valid,
    - NULL otherwise.

**/
AssemblerResult assemblerRun(Assembler assembler_in, char * fileName);


#endif /* _ASSEMBLER_H */
