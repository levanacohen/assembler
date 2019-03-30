#include "stack.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

bool isNameIsValid(char *name);

Stack stackCreate(){
    Stack newStack = (Stack)malloc(sizeof(struct stack_t)) ;
	if(newStack == NULL){
		return NULL;
	}
    newStack->numOfSymbols = 0;
    return newStack;
}

StackResult stackAddSymbol(Stack stack_in, char symbolName[], int address_in, bool isData, GuideStatement type, int* index_out){
    int i; /* to the loop 'for' */
    SymbolResult outputR = SYMBOL_SUCCESS;
    SymbolResult *outputResult = &outputR;
    if(!stack_in){
        return STACK_INPUT_STACK_NULL;
    }
    /* Checks if there is a symbol with that name */
    for (i=0; i<stack_in->numOfSymbols; i++){
        if (!(strcmp((stack_in->arrayOfSymbol[i]->name), symbolName))){
            if(stack_in->arrayOfSymbol[i]->addressDecimal && (stack_in->arrayOfSymbol[i]->addressDecimal >= FIRST_ADDRES)
                 && address_in >= FIRST_ADDRES){
                /* error: redeclared symbol */
                return STACK_REDECLARED_SYMBOL;
            }
            if(symbolUpdateAddress(stack_in->arrayOfSymbol[i], address_in)!=SYMBOL_SUCCESS){
                return STACK_SYMBOL_ERROR;
            }
            *index_out = i;
            if(type==EXTERN_G || type==ENTRY_G) {
                /* it invalid to define a label by .entry and .extern statement together */
                if(stack_in->arrayOfSymbol[i]->type!=-1 && stack_in->arrayOfSymbol[i]->type!=type){
                    if(type==ENTRY_G) return STACK_SYMBOL_DECLARED_EXTERN;
                    else return STACK_SYMBOL_DECLARED_ENTRY;
                }
                stack_in->arrayOfSymbol[i]->type=type;
            }
            /* if the label pointing on data (.string or .data) */
            if(isData){
                stack_in->arrayOfSymbol[i]->isData = true;
            }
            return STACK_SUCCESS;
        }
    }
    /* create new symbol in the stack */
    stack_in->arrayOfSymbol[stack_in->numOfSymbols] = symbolCreate(symbolName, address_in, isData, type, outputResult);
    *index_out = stack_in->numOfSymbols;
    if(*outputResult != SYMBOL_SUCCESS){
        if(*outputResult == SYMBOL_INVALID_NAME_VALUE){
            return STACK_INVALID_SYMBOL_NAME_VALUE;
        }else{
            return STACK_SYMBOL_ERROR;
        }
    }
    stack_in->numOfSymbols++;
    return STACK_SUCCESS;
}


StackResult stackGetSymbolAddressDecimalByName(Stack stack_in, char * name, int* address_decimal_out){
    int i;
    if(!stack_in){
        return STACK_INPUT_STACK_NULL;
    }
    if(!name){
        return STACK_INVALID_NAME_NULL;
    }
    if (!address_decimal_out){
        return STACK_INPUT_NULL;
    }
    /* search the label */
    for (i=0; i<stack_in->numOfSymbols; i++){
        if (!(strcmp((stack_in->arrayOfSymbol[i]->name), name))){
            *address_decimal_out = stack_in->arrayOfSymbol[i]->addressDecimal;
            return STACK_SUCCESS;
        }
    }
    return STACK_SYMBOL_DONT_FOUND;
}


StackResult stackGetSymbolAddressDecimalByIndex(Stack stack_in, int index, int* address_decimal_out){
    if(!stack_in){
        return STACK_INPUT_STACK_NULL;
    }
    if(!address_decimal_out){
        return STACK_INPUT_NULL;
    }
    if(index >= stack_in->numOfSymbols || index<0){
        return STACK_INDEX_NO_RANGE;
    }
    if(symbolGetAddressDecimal(stack_in->arrayOfSymbol[index], address_decimal_out)!=SYMBOL_SUCCESS){
        return STACK_SYMBOL_ERROR;
    }
    return STACK_SUCCESS;
}

StackResult stackGetNumOfSymbols(Stack stack_in, int* length){
    if (!stack_in){
        return STACK_INPUT_STACK_NULL;
    }
    if(!length){
        return STACK_INPUT_NULL;
    }
    *length = stack_in->numOfSymbols;
    return STACK_SUCCESS;
}

StackResult stackGetNameByIndex(Stack stack_in, int index, char ** label_out){
    if(!stack_in){
        return STACK_INPUT_STACK_NULL;
    }
    if(index < 0 || index >= stack_in->numOfSymbols){
        return STACK_INVALID_INDEX;
    }
    *label_out = stack_in->arrayOfSymbol[index]->name;
    return STACK_SUCCESS;
}


StackResult stackGetSymbolTypeByIndex(Stack stack_in, int index, GuideStatement* type){
    if(!stack_in){
        return STACK_INPUT_STACK_NULL;
    }
    if(index < 0 || index >= stack_in->numOfSymbols){
        return STACK_INVALID_INDEX;
    }
    if(!type){
        return STACK_INPUT_NULL;
    }
    *type = stack_in->arrayOfSymbol[index]->type;
    return STACK_SUCCESS;
}

StackResult stackGetIsDataSymbolByIndex(Stack stack_in, int index, bool* isData){
    if(!stack_in){
        return STACK_INPUT_STACK_NULL;
    }
    if(index < 0 || index >= stack_in->numOfSymbols){
        return STACK_INVALID_INDEX;
    }
    if(!isData){
        return STACK_INPUT_NULL;
    }
    *isData = stack_in->arrayOfSymbol[index]->isData;
    return STACK_SUCCESS;
}
