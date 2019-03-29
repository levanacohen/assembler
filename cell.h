#ifndef _CELL_H
#define _CELL_H

#include <stdbool.h>

#define NUM_BITS_IN_CELL 12

struct cell_t{
    int binary [NUM_BITS_IN_CELL];
    int addressDecimal;
    int addressBase4;
    bool has_binary;
    int indexSymbol; /* if symbol pointing on this address, it saves the index of label */
} ;

typedef struct cell_t* Cell;


typedef enum {
    CELL_SUCCESS, CELL_INVALID_ADDRESS,
    CELL_INVALID_ARRAY_NULL, CELL_NO_BINARY, CELL_INPUT_NO_BINARY,
    CELL_INVALID_CELL_IN_INPUT, CELL_BINARY_ALREADY_EXIST, CELL_INPUT_NULL
} CellResult;

/**
Function cellCreate:

Input:
    addressDecimal: the Decimal value of the current cell.
    indexSymbol: if symbol pointing on this address, it saves the index of label.
        else, its value is -1.

Output:
    a cell containing a pointer to the memory and has address
    (decimal and at base 4).
**/
Cell cellCreate(int addressDecimal, int indexSymbol);

/**
Function cellUpdateBinary:

Input:
    cell_in: the cell to be updated.
    binary: the value of the cell meaning what the cell
            should contain.

Output:
    CELL_SUCCESS: if the update was successful.
    CELL_INVALID_CELL_IN_INPUT: if cell_in already contain binary or cell_in
        is null.
    CELL_INVALID_ARRAY_NULL: if binary is NULL.
    CELL_BINARY_ALREADY_EXIST: the cell's binary already been updated.
    CELL_INPUT_NO_BINARY: if binary contain invalid values in it.
**/
CellResult cellUpdateBinary(Cell cell_in, int binary[NUM_BITS_IN_CELL]);

/**
Function cellGetBinary:

Input:
    cell_in: the cell to read from.
    output: an array to which we want to write the result.

Output:
    CELL_SUCCESS: if no error happened.
    CELL_INVALID_CELL_IN_INPUT: if cell is null.
    CELL_NO_BINARY: if the cell does not already has binary in it.
    CELL_INPUT_NULL: if output is null.
**/
CellResult cellGetBinary(Cell cell_in, int* output[NUM_BITS_IN_CELL]);

/**
Function cellGetAddressDecimal:

Input:
    cell_in: the cell to read from.
    output: an pointer to which we want to write the result.

Output:
    CELL_SUCCESS: if no error happened.
    CELL_INVALID_CELL_IN_INPUT: if cell is null.
    CELL_INPUT_NULL: if output is null.
**/
CellResult cellGetAddressDecimal(Cell cell_in, int* output);

/**
Function cellGetAddressBase4:

Input:
    cell_in: the cell to read from.
    output: an pointer to which we want to write the result.

Output:
    CELL_SUCCESS: if no error happened.
    CELL_INVALID_CELL_IN_INPUT: if cell is null.
    CELL_INPUT_NULL: if output is null.
**/
CellResult cellGetAddressBase4(Cell cell_in, int* output);

#endif /* _CELL_H */
