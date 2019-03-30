#ifndef _PROGRAM_H
#define _PROGRAM_H

#include "command.h"
#include <stdio.h>
#define NEW_LINE '\n'
#define SEMICOLON ';'
#define MAX_NUM_OF_LINES 1000
#define MAX_NUM_OF_LINES_TO_COMMAND 6


struct program_t{
    Command all_commands [MAX_NUM_OF_LINES];
    int numOfCommands;
};

typedef struct program_t* Program;

typedef enum{
    PROGRAM_SUCCESS,
    PROGRAM_INPUT_NULL,
    PROGRAM_MEMORY_INVALID,
    PROGRAM_GET_CELL_ERROR,
    PROGRAM_OPCODE_ERROR,
    PROGRAM_SIZE_MEMORY_OVER,
    PROGRAM_STACK_ERROR,
    PROGRAM_SYMBOL_DIDNT_DEFINED,
    PROGRAM_CELL_UPDATE_ERROR,
    PROGRAM_SYMBOL_ERROR,
    PROGRAM_FILE_NULL,
    PROGRAM_FIRST_PASS_FAIL,
    PROGRAM_SECOND_PASS_FAIL
} ProgramResult;

/**
Function programCreate:

Input:
    file_in: the input file.
    file_name: the name of input file.
    memoryData_in: the memory of data.
    memoryCode_in: the memory of code.
    stack_in: the stack contain symbols.
    result_out: control the function.

Output:
    - Program:    The program struct returned to the assembler.
    - NULL:   if the input is invalid.
**/
Program programCreate(FILE* file_in, char* file_name ,Memory memoryData_in,
                      Memory memoryCode_in, Stack stack_in, ProgramResult* result_out);


/**
Function programSecondPass:

Input:
    program: the input file.
    memoryData_in: the memory of data.
    memoryCode_in: the memory of code.
    stack_in: the stack contain symbols.
    label_out: a symbol.

Output:
    - Program:    The program struct returned to the assembler.
    - NULL:   if the input is invalid.
**/
ProgramResult programSecondPass(Program program, Memory memoryData_in, Memory memoryCode_in, Stack stack_in, char* label_out);

#endif /* _PROGRAM_H */
