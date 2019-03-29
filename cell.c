#include <stdio.h>
#include <stdlib.h>
#include "cell.h"


void insertArrayFromArray(int * from, int * to, int length);
int fromDecimalToBase4(int decimal_in);
bool isBinary(int *array);

Cell cellCreate(int addressDecimal, int indexSymbol){
    Cell newCell = (Cell)malloc(sizeof(struct cell_t));
    if(newCell == NULL){
        return NULL;
    }
    newCell->has_binary = false;
    newCell->addressDecimal = addressDecimal;
    newCell->addressBase4 = fromDecimalToBase4(addressDecimal);
    newCell->indexSymbol = indexSymbol;
    return newCell;
}

CellResult cellUpdateBinary(Cell cell_in, int binary_in[NUM_BITS_IN_CELL]){
    /*Checking integrity of all parameters*/
    if(!cell_in){
        return CELL_INVALID_CELL_IN_INPUT;
    } else if(!binary_in){
        return CELL_INVALID_ARRAY_NULL;
    }
    cell_in->has_binary = true;
    insertArrayFromArray(binary_in, cell_in->binary, NUM_BITS_IN_CELL);

    return CELL_SUCCESS;
}

CellResult cellGetBinary(Cell cell_in, int * output[NUM_BITS_IN_CELL]){
    /*Checking integrity of all parameters*/
    if (!cell_in){
        return CELL_INVALID_CELL_IN_INPUT;
    }
    if (!(cell_in->has_binary)){
        return CELL_NO_BINARY;
    }
    if (!output){
        return CELL_INPUT_NULL;
    }
    *output = cell_in->binary;
    return CELL_SUCCESS;
}

CellResult cellGetAddressDecimal(Cell cell_in, int* output){
    if (!cell_in){
        return CELL_INVALID_CELL_IN_INPUT;
    }
    if (!output){
        return CELL_INPUT_NULL;
    }
    *output = cell_in->addressDecimal;
    return CELL_SUCCESS;
}

CellResult cellGetAddressBase4(Cell cell_in, int* output){
    if (!cell_in){
        return CELL_INVALID_CELL_IN_INPUT;
    }
    if (!output){
        return CELL_INPUT_NULL;
    }
    *output = cell_in->addressBase4;
    return CELL_SUCCESS;
}
