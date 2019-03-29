#include "memory.h"
#include <stdio.h>

void insertArrayFromArray(int * from, int * to, int length);

Memory memoryCreate(){
    Memory newMemory = (Memory)malloc(sizeof(struct memory_t));
    if(!newMemory){
        return NULL;
    }
    newMemory->numOfCells = 0;
    return newMemory;
}

MemoryResult memoryAddCell(Memory memory_in, int binary[NUM_OF_BITS_IN_MEMORY], int indexSymbol){
    Cell newCell;
    if (!memory_in){
        return MEMORY_INVALID_MEMORY_INPUT;
    } else if(!binary){
        return MEMORY_INVALID_ARRAY_NULL;
    }

    newCell = cellCreate((memory_in->numOfCells)+FIRST_ADDRESS, indexSymbol);
    if(!newCell){
        return MEMORY_INVALID_MEMORY_INPUT;
    }
    cellUpdateBinary(newCell, binary);
    memory_in->memory_cells[memory_in->numOfCells] = newCell;
    memory_in->numOfCells++;
    return MEMORY_SUCCESS;
}

MemoryResult memoryGetCellBinaryByAddress(Memory memory_in, int addressDecimal, int output[NUM_BITS_IN_CELL]){
    if (!memory_in){
        return MEMORY_INVALID_MEMORY_INPUT;
    } else if (addressDecimal<FIRST_ADDRESS || addressDecimal>LAST_ADDRESS){
        return MEMORY_INVALID_ADDRESS;
    }
    insertArrayFromArray(memory_in->memory_cells[addressDecimal-FIRST_ADDRESS]->binary, output, NUM_BITS_IN_CELL);
    return MEMORY_SUCCESS;
}

MemoryResult memoryGetNumOfCells(Memory memory_in, int* length){
    if (!memory_in){
        return MEMORY_INVALID_MEMORY_INPUT;
    }
    *length = memory_in->numOfCells;
    return MEMORY_SUCCESS;
}

MemoryResult memoryGetIndexSymbolByAddress(Memory memory_in, int addressDecimal, int* addressSymbol_out){
    if (!memory_in){
        return MEMORY_INVALID_MEMORY_INPUT;
    } else if (addressDecimal<FIRST_ADDRESS || addressDecimal>LAST_ADDRESS){
        return MEMORY_INVALID_ADDRESS;
    }
    *addressSymbol_out = memory_in->memory_cells[addressDecimal-FIRST_ADDRESS]->indexSymbol;
    return MEMORY_SUCCESS;
}
