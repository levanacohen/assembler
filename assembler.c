#include "assembler.h"
#include <stdio.h>
#include <string.h>

#define NUM_OUT 6

void freeProgram(Program program_in);
void fprintArrayBase4(FILE* file, int array[], int length);
void freeMemory(Memory memory_in);
void freeStack(Stack stack_in);
int fromDecimalToBase4(int decimal_in);
void fromBinarytoBase4(int binary[NUM_BITS_IN_CELL], int* base4);
AssemblerResult assemblerMakeFilesOutput(FILE* file_out_ob, FILE* file_out_ent, FILE* file_out_ext,
        Program program, Memory memoryData_in, Memory memoryCode_in, Stack stack_in);


Assembler assemblerCreate(FILE * file_in, char * file_name, AssemblerResult* assembler_result_in){
    ProgramResult result_out;
    Assembler newAssembler = (Assembler) malloc(sizeof(struct assembler_t));
    newAssembler->stack_in = stackCreate();
    newAssembler->memoryCode = memoryCreate();
    newAssembler->memoryData = memoryCreate();
    newAssembler->program_in = programCreate(file_in, file_name, newAssembler->memoryData,
                                             newAssembler->memoryCode,
                                             newAssembler->stack_in, &result_out);

    *assembler_result_in = (result_out==PROGRAM_SUCCESS)? ASSEMBLER_SUCCESS : ASSEMBLER_FIRST_PASS_ERROR;
    return newAssembler;
}

AssemblerResult assemblerRun(Assembler assmbler_in, char * fileName){

    /* create names' files */
    char* fileNameOb  = (char*) malloc(sizeof(char)* (strlen(fileName)+ 4));
    char* fileNameEnt = (char*) malloc(sizeof(char)* (strlen(fileName)+ 5));
    char* fileNameExt = (char*) malloc(sizeof(char)* (strlen(fileName)+ 5));
    ProgramResult result_program_out;

    FILE * file_out_ob, *file_out_ent, *file_out_ext;

    /* create names' files */
    memcpy(fileNameOb , fileName, strlen(fileName)+1);
    memcpy(fileNameEnt, fileName, strlen(fileName)+1);
    memcpy(fileNameExt, fileName, strlen(fileName)+1);

    /* second pass */
    if((result_program_out = programSecondPass(assmbler_in->program_in, assmbler_in->memoryData,
                assmbler_in->memoryCode, assmbler_in->stack_in, fileName))!=PROGRAM_SUCCESS){
        return ASSEMBLER_SECOND_PASS_ERROR;
    }

    /* the file extensions */
    file_out_ob  = fopen(strcat(fileNameOb , ".ob"), "w");
    file_out_ent = fopen(strcat(fileNameEnt, ".ent"), "w");
    file_out_ext = fopen(strcat(fileNameExt, ".ext"), "w");

    free(fileNameOb);
    free(fileNameEnt);
    free(fileNameExt);

    /* printing output */
    if(assemblerMakeFilesOutput(file_out_ob, file_out_ent, file_out_ext, assmbler_in->program_in,
                            assmbler_in->memoryData, assmbler_in->memoryCode, assmbler_in->stack_in)!=ASSEMBLER_SUCCESS){
        return ASSEMBLER_SECOND_PASS_ERROR;
    }

    /* close files */
    fclose(file_out_ent);
    fclose(file_out_ext);
    fclose(file_out_ob);
    return ASSEMBLER_SUCCESS;
}

/**
Function assemblerMakeFilesOutput:

Input:
    file_out_ob: file .ob to output.
    file_out_ent: file .ent to output.
    file_out_ext: file .ext to output.
    program: the program contain all commands.
    memoryData_in: the memory of data.
    memoryCode_in: the memory of code.
    stack_in: the stack contain symbols.

output:
    AssemblerResult: control the function.
*/
AssemblerResult assemblerMakeFilesOutput(FILE* file_out_ob, FILE* file_out_ent, FILE* file_out_ext,
                                         Program program, Memory memoryData_in, Memory memoryCode_in, Stack stack_in){
    int lengthOfData;
    int lengthOfCode;
    int numOfSymbols; /* number of labels */
    int i=0;
    char* label_out; /* for .ext and .ent files */
    int addressDecimal = 0; /* address of cell at decimal */
    int addressBase4 = 0; /* address of cell at base 4 */
    int binary[NUM_BITS_IN_CELL];
    int binaryAtBase4[NUM_OUT];
    int indexSymbol; /* the index of symbol in the stack */
    GuideStatement type;

    /* get number of symbols */
    if(stackGetNumOfSymbols(stack_in, &numOfSymbols)!=STACK_SUCCESS){
        return ASSEMBLER_STACK_ERROR;
    }
    /* get number of cells of memory data */
    if(memoryGetNumOfCells(memoryData_in, &lengthOfData)!=MEMORY_SUCCESS){
        return ASSEMBLER_MEMORY_INVALID;
    }
    /* get number of cells of memory code */
    if(memoryGetNumOfCells(memoryCode_in, &lengthOfCode)!=MEMORY_SUCCESS){
        return PROGRAM_MEMORY_INVALID;
    }
    /* print the length of the memory data and memory code */
    fprintf(file_out_ob, "  %d\t%d\n",fromDecimalToBase4(lengthOfCode), fromDecimalToBase4(lengthOfData));

    /* pass the memory code to print all the cells */
    for(i=FIRST_ADDRESS; i<FIRST_ADDRESS+lengthOfCode; i++){
        if(cellGetAddressBase4(memoryCode_in->memory_cells[i-FIRST_ADDRESS], &addressBase4)!=CELL_SUCCESS){
            return ASSEMBLER_MEMORY_INVALID;
        }
        if(memoryGetCellBinaryByAddress(memoryCode_in, i, binary)!=MEMORY_SUCCESS){
            return ASSEMBLER_GET_CELL_ERROR;
        }
        /* print address */
        fprintf(file_out_ob, "%d\t",addressBase4);

        fromBinarytoBase4(binary, binaryAtBase4);
        /* print the value's cell in the memory */
        fprintArrayBase4(file_out_ob, binaryAtBase4, NUM_OUT);
    }


    /* pass the memory data to print all the cells */
    for(i=FIRST_ADDRESS; i<FIRST_ADDRESS+lengthOfData; i++){
        if(cellGetAddressDecimal(memoryData_in->memory_cells[i-FIRST_ADDRESS], &addressDecimal)!=CELL_SUCCESS){
            return ASSEMBLER_MEMORY_INVALID;
        }
        addressBase4 = fromDecimalToBase4(addressDecimal + lengthOfCode);
        if(memoryGetCellBinaryByAddress(memoryData_in, i, binary)!=MEMORY_SUCCESS){
            return ASSEMBLER_GET_CELL_ERROR;
        }
        /* print address */
        fprintf(file_out_ob, "%d\t",addressBase4);

        fromBinarytoBase4(binary, binaryAtBase4);
        /* print the value's cell in the memory */
        fprintArrayBase4(file_out_ob, binaryAtBase4, NUM_OUT);
    }

    /* prints to .ent and .ext files */
    for(i=0; i<numOfSymbols; i++){
        if(stackGetSymbolTypeByIndex(stack_in, i, &type)!=STACK_SUCCESS){
            return ASSEMBLER_STACK_ERROR;
        }
        /* a label to entry */
        if(type==ENTRY_G){
            int j;
            /* search addresses of cells used the label */
            for(j=FIRST_ADDRESS; j<FIRST_ADDRESS+lengthOfCode; j++){
                /* get the name of the label */
                stackGetNameByIndex(stack_in, i, &label_out);
                /* each cell have field, that save the index of the label that point on it, if exist. */
                if(memoryGetIndexSymbolByAddress(memoryCode_in, j, &indexSymbol)!=MEMORY_SUCCESS){
                    return ASSEMBLER_MEMORY_INVALID;
                }
                /* print the symbol and the address */
                if(indexSymbol==i){
                    fprintf(file_out_ent, "%s\t%d\n", label_out, fromDecimalToBase4(j));
                }
            }
            /* now search in memory data, at the same method */
            for(j=FIRST_ADDRESS; j<FIRST_ADDRESS+lengthOfData; j++){
                stackGetNameByIndex(stack_in, i, &label_out);
                if(memoryGetIndexSymbolByAddress(memoryData_in, j, &indexSymbol)!=MEMORY_SUCCESS){
                    return ASSEMBLER_MEMORY_INVALID;
                }
                if(indexSymbol==i){
                    /* print the symbol and the address */
                    fprintf(file_out_ent, "%s\t%d\n",label_out, fromDecimalToBase4(lengthOfCode+j));
                }
            }
        }
        /* a label to extern */
        if(type==EXTERN_G){
            int j;
            /* search addresses of cells used the label */
            for(j=FIRST_ADDRESS; j<FIRST_ADDRESS+lengthOfCode; j++){
                /* get the name of the label */
                stackGetNameByIndex(stack_in, i, &label_out);
                /* each cell have field, that save the index of the label that point on it, if exist. */
                if(memoryGetIndexSymbolByAddress(memoryCode_in, j, &indexSymbol)!=MEMORY_SUCCESS){
                    return ASSEMBLER_MEMORY_INVALID;
                }
                if(indexSymbol==i){
                /* print the symbol and the address */
                    fprintf(file_out_ext, "%s\t%d\n",label_out, fromDecimalToBase4(j));
                }
            }
            /* now search in memory data, at the same method */
            for(j=FIRST_ADDRESS; j<FIRST_ADDRESS+lengthOfData; j++){
                stackGetNameByIndex(stack_in, i, &label_out);
                if(memoryGetIndexSymbolByAddress(memoryData_in, j, &indexSymbol)!=MEMORY_SUCCESS){
                    return ASSEMBLER_MEMORY_INVALID;
                }
                if(indexSymbol==i){
                    /* print the symbol and the address */
                    fprintf(file_out_ext, "%s\t%d\n",label_out, fromDecimalToBase4(lengthOfCode+j));
                }
            }
        }
    }

    /* free structures */
    freeMemory(memoryCode_in);
    freeMemory(memoryData_in);
    freeProgram(program);
    free(memoryCode_in);
    free(memoryData_in);
    return ASSEMBLER_SUCCESS;
}

/**
Function freeMemory:

Input:
    memory_in: the memory to free its cells.
*/
void freeMemory(Memory memory_in){
    int numCells = 0, i;
    if(memoryGetNumOfCells(memory_in, &numCells)!=MEMORY_SUCCESS){
        return;
    }
    for(i=0; i<numCells; i++){
        free(memory_in->memory_cells[i]);
    }
}

/**
Function freeProgram:

Input:
    program_in: the program to free its cells.
*/
void freeProgram(Program program_in){
    int i, length = program_in->numOfCommands;
    for(i=0; i<length; i++){
        free(program_in->all_commands[i]);
    }
}

/**
Function freeStack:

Input:
    stack_in: the stack to free its cells.
*/
void freeStack(Stack stack_in){
    int numSymbols = 0, i;
    if(stackGetNumOfSymbols(stack_in, &numSymbols)!=STACK_SUCCESS){
        return;
    }
    for(i=0; i<numSymbols; i++){
        free(stack_in->arrayOfSymbol[i]);
    }
}

/**
Function fromBinarytoBase4:
the function get a binary array, convert its number to base 4
and insert the digits to base4Array.

Input:
    binary: the binary array.
    base4Array: the number at base 4.
*/
void fromBinarytoBase4(int binary[NUM_BITS_IN_CELL], int base4Array[NUM_OUT]){
    int i;
    for(i=NUM_BITS_IN_CELL-2; i>=0; i-=2){
        base4Array[i/2] = binary[i+1] + 2*binary[i];
    }
}

/**
Function fprintArrayBase4:
the function get a binary array, convert its number to base 4
and insert the digits to base4Array.

Input:
    file: the file which we want to print.
    array: an array to print it's cells.
    length: the length of array.
*/
void fprintArrayBase4(FILE* file, int array[], int length){
    int i;
    for(i=0; i<length; i++){
        fprintf(file, "%d", array[i]);
    }
    fprintf(file, "\n");
}
