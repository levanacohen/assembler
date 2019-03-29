#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdlib.h>
#include "cell.h"

#define NUM_OF_BITS_IN_MEMORY 12
#define SIZE_OF_MEMORY 1000
#define FIRST_ADDRESS 100
#define LAST_ADDRESS 1099

struct memory_t{
    int numOfCells;
    Cell memory_cells[SIZE_OF_MEMORY];
};

typedef struct memory_t* Memory;

typedef enum{
    MEMORY_SUCCESS,
    MEMORY_INVALID_MEMORY_INPUT, /*when the memory in the input is NULL*/
    MEMORY_INVALID_ADDRESS, /*when the address is not in the range [FIRST_ADDRESS, LAST_ADDRESS]*/
    MEMORY_INVALID_ARRAY_NULL, /*when array given is NULL*/
    MEMORY_EMPTY_ADDRESS /*when the address given points to empty cell*/
}   MemoryResult;

/**
Function memoryCreate:

output:
    a memory object with no values in it. numOfCells initializes to 0.
**/
Memory memoryCreate();

/**
Function memoryAddCell:

Input:
    memory_in:  an existing memory object.
    binary: The binary value for the new cell.
    indexSymbol: if the cell save address of symbol, it saves the index.

Output:
    MEMORY_SUCCESS: The function creates a new cell and updating it's value to binary.
    MEMORY_INVALID_MEMORY_INPUT: if memory_in is NULL.
    MEMORY_INVALID_ARRAY_NULL: if array binary given is NULL.
**/
MemoryResult memoryAddCell(Memory memory_in, int binary[NUM_OF_BITS_IN_MEMORY], int indexSymbol);

/**
Function memoryGetCellBinaryByAddress:

Input:
    memory_in:  an existing memory object.
    addressDecimal: An address of a cell- an integer in the range [FIRST_ADDRESS, LAST_ADDRESS].
    output: a pointer to array which we want to write the result.

Output:
    MEMORY_SUCCESS: if no error happened.
    MEMORY_INVALID_MEMORY_INPUT: if the memory in the input is NULL.
    MEMORY_INVALID_ADDRESS: if the address is not in the range [FIRST_ADDRESS, LAST_ADDRESS].
    MEMORY_EMPTY_ADDRESS: if the address given points to empty cell.
**/
MemoryResult memoryGetCellBinaryByAddress(Memory memory_in, int addressDecimal, int output[NUM_BITS_IN_CELL]);

/**
Function memoryGetNumOfCells:

Input:
    memory_in:  an existing memory object.
    length: a pointer to int which we want to write the result.

Output:
    MEMORY_SUCCESS: if no error happened.
    MEMORY_INVALID_MEMORY_INPUT: if the memory in the input is NULL.
**/
MemoryResult memoryGetNumOfCells(Memory memory_in, int* length);

/**
Function memoryGetIndexSymbolByAddress:
    return the index of label that pointing on this address.
    if there is no such label, it will return -1.

Input:
    memory_in:  an existing memory object.
    addressDecimal: An address of a cell- an integer in the range [FIRST_ADDRESS, LAST_ADDRESS].
    addressSymbol_out: a pointer to integer, which we want to write the result

Output:
    MEMORY_SUCCESS: if no error happened.
    MEMORY_INVALID_ADDRESS: if the address is not in the range [FIRST_ADDRESS, LAST_ADDRESS].
    MEMORY_INVALID_MEMORY_INPUT: if the memory in the input is NULL.
**/
MemoryResult memoryGetIndexSymbolByAddress(Memory memory_in, int addressDecimal, int* addressSymbol_out);

#endif /* _MEMORY_H */
