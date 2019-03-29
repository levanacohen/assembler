#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "symbol.h"

int fromDecimalToBase4(int decimal_in);
bool isNameIsValid(char *name);

Symbol symbolCreate(char *name, int addressDecimal, bool isData, GuideStatement type, SymbolResult * outputResult){
    Symbol newSymbol;
    if(!outputResult){
        *outputResult = SYMBOL_INPUT_NULL;
        return NULL;
    }
    *outputResult = SYMBOL_SUCCESS;
    /* Checking the input name variable not null */
    if(!name){
        *outputResult = SYMBOL_INVALID_NAME_NULL;
        return NULL;
    }
    /* Checking the name input variable value */
    if(!isNameIsValid(name)){
        *outputResult = SYMBOL_INVALID_NAME_VALUE;
        return NULL;
    }
    /* Allocating the Symbol element */
	newSymbol = (Symbol)malloc(sizeof(struct symbol_t));
	if(newSymbol == NULL){
        *outputResult = SYMBOL_ALLOCATION_ERROR;
        return NULL;
	}
	/* Copying the string */
    if (!strcpy(newSymbol->name, name)){
        *outputResult = SYMBOL_STRCPY_FAIL;
        return NULL;
    }
    newSymbol->addressDecimal = addressDecimal;
    newSymbol->addressBase4 = fromDecimalToBase4(addressDecimal);
    newSymbol->type=type;
    newSymbol->isData = isData;
	return newSymbol;
}

SymbolResult symbolGetName(Symbol symbol_in, char** output){
	if(!symbol_in){
		return SYMBOL_INVALID_SYMBOL_IN_INPUT;
	}
	if(!output || !(*output)){
        return SYMBOL_INPUT_NULL;
	}
	*output = symbol_in->name;
	return SYMBOL_SUCCESS;
}

SymbolResult symbolGetAddressDecimal(Symbol symbol_in, int* output){
	if(!symbol_in){
		return SYMBOL_INVALID_SYMBOL_IN_INPUT;
	}
	if(!output){
        return SYMBOL_INPUT_NULL;
	}
	*output = symbol_in->addressDecimal;
	return SYMBOL_SUCCESS;
}

SymbolResult symbolGetAddressBase4(Symbol symbol_in, int* output){
	if(!symbol_in){
		return SYMBOL_INVALID_SYMBOL_IN_INPUT;
	}
	if(!output){
        return SYMBOL_INPUT_NULL;
	}
	*output = symbol_in->addressBase4;
	return SYMBOL_SUCCESS;
}

SymbolResult symbolUpdateAddress(Symbol symbol_in, int addressDecimal_in){
    if(!symbol_in){
        return SYMBOL_INVALID_SYMBOL_IN_INPUT;
    }
    if((addressDecimal_in<=0) && symbol_in->addressDecimal){
        /* don't update, the symbol already has address */
        return SYMBOL_SUCCESS;
    }
    symbol_in->addressDecimal = addressDecimal_in;
    symbol_in->addressBase4 = fromDecimalToBase4(addressDecimal_in);
    return SYMBOL_SUCCESS;
}
