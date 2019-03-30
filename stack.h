
#ifndef _STACK_H
#define _STACK_H

#include "symbol.h"
#define MAX_NUM_OF_SYMBOLS 1000
#define FIRST_ADDRES 100

struct stack_t{
    int numOfSymbols;
    Symbol arrayOfSymbol[MAX_NUM_OF_SYMBOLS];
} ;

typedef struct stack_t* Stack;

typedef enum{
    STACK_SUCCESS, STACK_INVALID_NAME_NULL,
    STACK_INVALID_NAME_VALUE,
    STACK_INPUT_STACK_NULL,
    STACK_INVALID_SYMBOL_NAME_VALUE,
    STACK_SYMBOL_NAME_EXIST,
    STACK_SYMBOL_ERROR,
    STACK_INPUT_NULL,
    STACK_INVALID_ADDRESS,
    STACK_SYMBOL_DONT_FOUND,
    STACK_INDEX_NO_RANGE,
    STACK_INVALID_INDEX,
    STACK_SYMBOL_DECLARED_EXTERN,
    STACK_SYMBOL_DECLARED_ENTRY,
    STACK_REDECLARED_SYMBOL
} StackResult;


/**
Function stackCreate:

output:
    Stack: the created stack.
*/
Stack stackCreate();

/**
Function stackAddSymbol:

input:
    stack_in: an existing stack object.
	symbolName: the name of the new symbol.
	address_in: the address of the new symbol at Decimal.
	isData: if the label pointing on data, it's value will true. false otherwise.
	type: the type of the symbol, entry or extern. else, it's value is -1.

output:
    STACK_SUCCESS: if no error happened.
    STACK_INPUT_STACK_NULL: if stack_in is NULL.
	STACK_INVALID_SYMBOL_NAME_VALUE: if symbolName invalid.
	STACK_SYMBOL_ERROR: if we did not success to create the symbol.
	STACK_SYMBOL_NAME_EXIST: if there is a symbol with that name.
	STACK_SYMBOL_DECLARED_EXTERN: if the symbol declared by .extern and now it is redeclared by .entry statement.
	STACK_SYMBOL_DECLARED_ENTRY: if the symbol declared by .entry and now it is redeclared by .extern statement.
**/
StackResult stackAddSymbol(Stack stack_in, char symbolName[], int address_in, bool isData, GuideStatement type, int* index_out);

/**
Function stackGetSymbolAddressDecimalByName:

input:
    stack_in: an existing stack object.
	name: the name of the requested symbol.
	address_decimal_out: the address of the requested symbol at Decimal.

output:
    STACK_SUCCESS: if no error happened.
    STACK_INPUT_STACK_NULL: if stack_in is NULL.
	STACK_INVALID_NAME_NULL: if name input is NULL.
    STACK_SYMBOL_DONT_FOUND: if there is no symbol with that name.
    STACK_INPUT_NULL: if address_decimal_out is NULL.
*/
StackResult stackGetSymbolAddressDecimalByName(Stack stack_in, char * name, int* address_decimal_out);

/**
Function stackGetSymbolAddressDecimalByIndex:

input:
    stack_in: an existing stack object.
	index: the index of the requested symbol.
	address_decimal_out: the address of the requested symbol at decimal.

output:
    STACK_SUCCESS: if no error happened.
    STACK_INPUT_STACK_NULL: if stack_in is NULL.
	STACK_INPUT_NULL: if address_decimal_out is NULL.
    STACK_INDEX_NO_RANGE: if index not in the range: [0, numOfSymbols].
    STACK_SYMBOL_ERROR: if we did not success to get the index.
*/
StackResult stackGetSymbolAddressDecimalByIndex(Stack stack_in, int index, int* address_decimal_out);

/**
Function stackGetNumOfSymbols:

Input:
    stack_in:  an existing stack object.
    length: a pointer to int which we want to write the result.

Output:
    STACK_SUCCESS: if no error happened.
    STACK_INPUT_STACK_NULL: if stack_in is NULL.
**/
StackResult stackGetNumOfSymbols(Stack stack_in, int* length);

/**
Function stackGetNameByIndex:

Input:
    stack_in:  an existing stack object.
    index: an index of the request label in the stack.
    label_out:  a pointer to a string representing the name of the label.

Output:
    STACK_SUCCESS: if no error happened.
    STACK_INPUT_STACK_NULL: if stack_in is NULL.
    STACK_INVALID_INDEX: the input index is
            invalid meaning not in the range [0, size_of_stack]
**/
StackResult stackGetNameByIndex(Stack stack_in, int index, char ** label_out);

/**
Function stackGetNameByIndex:

Input:
    stack_in:  an existing stack object.
    index: an index of the request label in the stack.
    type:  a pointer to an integer representing the type of the label, entry or extern.
            if the label is not entry or extern, the value will be -1.

Output:
    STACK_SUCCESS: if no error happened.
    STACK_INPUT_STACK_NULL: if stack_in is NULL.
    STACK_INPUT_NULL: if type is NULL.
    STACK_INVALID_INDEX: the input index is
            invalid meaning not in the range [0, size_of_stack]
**/
StackResult stackGetSymbolTypeByIndex(Stack stack_in, int index, GuideStatement* type);

/**
Function stackGetNameByIndex:

Input:
    stack_in:  an existing stack object.
    index: an index of the request label in the stack.
    isData:  a pointer to boolean. if the label pointing on address contain data,
             it's value is true. false otherwise

Output:
    STACK_SUCCESS: if no error happened.
    STACK_INPUT_STACK_NULL: if stack_in is NULL.
    STACK_INPUT_NULL: if isData is NULL.
    STACK_INVALID_INDEX: the input index is
            invalid meaning not in the range [0, size_of_stack]
**/
StackResult stackGetIsDataSymbolByIndex(Stack stack_in, int index, bool* isData);

#endif /*_STACK_H*/
