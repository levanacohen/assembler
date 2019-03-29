#ifndef _SYMBOL_H
#define _SYMBOL_H
#include <stdbool.h>
#define MAX_CHARACTERS_IN_LABEL 30

typedef enum{
    ENTRY_G, EXTERN_G
} GuideStatement;

struct symbol_t{
    char name[MAX_CHARACTERS_IN_LABEL];
    int addressDecimal;
    int addressBase4;
    bool isData;
    GuideStatement type;
};

typedef struct symbol_t *Symbol;


typedef enum {
    SYMBOL_SUCCESS,
    SYMBOL_INVALID_NAME_NULL,
    SYMBOL_INVALID_NAME_VALUE,
    SYMBOL_ALLOCATION_ERROR,
    SYMBOL_STRCPY_FAIL,
    SYMBOL_INVALID_SYMBOL_IN_INPUT,
    SYMBOL_INPUT_NULL
} SymbolResult;


/**
Function symbolCreate:

input:
	*name: the name of the new symbol.
	addressDecimal: The address that the new symbol pointing on it. the address of the new symbol at Decimal.
	isData: if the label pointing on data, it's value will true. false otherwise.
	type: the type of the symbol, entry or extern. else, it's value is -1.
	outputResult: Control the function. this argument is updated according to the function:
        SYMBOL_SUCCESS: if no error happened.
        SYMBOL_INVALID_NAME_NULL: if the name transported as a parameter is NULL.
        SYMBOL_INVALID_NAME_VALUE: if the name invalid, According to the 'isNameIsValid' function.
        SYMBOL_ALLOCATION_ERROR: if Allocation failed.
        SYMBOL_STRCPY_FAIL: if the function 'strcpy' failed.

output:
	Symbol: the new created symbol.
	NULL: if the operation failed.
**/
Symbol symbolCreate(char *name, int addressDecimal, bool isData, GuideStatement type, SymbolResult * outputResult);

/**
Function symbolGetName:

input:
	symbol_in: the symbol to get his name.
	output: a pointer to string which we want to write the result.

output:
    SYMBOL_SUCCESS: if no error happened.
	SYMBOL_INVALID_SYMBOL_IN_INPUT: if symbol is NULL.
	SYMBOL_INPUT_NULL: if output is NULL.
**/
SymbolResult symbolGetName(Symbol symbol_in, char** output);

/**
Function symbolGetAddressDecimal:

input:
	symbol_in: the symbol to read from.
	output: a pointer to which we want to write the result.

output:
    SYMBOL_SUCCESS: if no error happened.
	SYMBOL_INVALID_SYMBOL_IN_INPUT: if symbol is NULL.
	SYMBOL_INPUT_NULL: if output is NULL.
**/
SymbolResult symbolGetAddressDecimal(Symbol symbol_in, int* output);


/**
Function symbolGetAddressBase4:

input:
	symbol_in: the symbol to read from.
	output: a pointer to which we want to write the result.

output:
    SYMBOL_SUCCESS: if no error happened.
	SYMBOL_INVALID_SYMBOL_IN_INPUT: if symbol is NULL.
	SYMBOL_INPUT_NULL: if output is NULL.
**/
SymbolResult symbolGetAddressBase4(Symbol symbol_in, int* output);

/**
Function symbolUpdateAddress:

input:
	symbol_in: the symbol to read from.
	addressDecimal_in:  The address of the label.

output:
    SYMBOL_SUCCESS: if no error happened.
	SYMBOL_INVALID_SYMBOL_IN_INPUT: if symbol is NULL.
**/
SymbolResult symbolUpdateAddress(Symbol symbol_in, int addressDecimal_in);

#endif /*_SYMBOL_H*/

