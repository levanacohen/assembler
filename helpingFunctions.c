#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#define NUM_BITS_IN_ARRAY 12
#define MAX_CHARACTERS_IN_LABEL 30
#define NUM_OF_OPCODES 16

/**
Private Function isBinary:

input:
	array: an array to check if it
        representative a binary number.

output:
	1 (true): if array representative a binary number.
	0 (false): if array doesn't representative a binary number
*/
bool isBinary(int *array){
    int i;
    for (i=0; i<NUM_BITS_IN_ARRAY; i++){
        if (array[i]!=0 && array[i]!=1){
            return false;
        }
    }
    return true;
}

/**
Private Function fromDecimalToBase4:

input:
	decimal_in: the number at decimal to converting.

output:
	base4: the number at base 4.
**/
int fromDecimalToBase4(int decimal_in){
    int decimal = decimal_in;
    int remainder =0;
    int digit = 1;
    int base4 = 0;

    for(; decimal ;){

        remainder = decimal%4;
        base4 = remainder*digit + base4;
        decimal/=4;
        digit *= 10;
    }
    return base4;
}

/**
Private Function isNameIsValid:

input:
	name[]: the name for chacking.
	The name should be a maximum of 30 characters,
	all characters in the name are letters or digits,
	the first character is a letter.


output:
	1 (true): if the name is valid.
	0 (false): if the name is not valid.
**/
bool isNameIsValid(char *name) {
    const char *opcodeNames[NUM_OF_OPCODES] = {"mov", "cmp", "add", "sub", "not", "clr", "lea",
                        "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    int length = strlen(name);
    int i;
    if(!name || length>MAX_CHARACTERS_IN_LABEL){
        return false;
    }
    for(i=0; i<length; i++){
        if ( !(isalpha(name[i]) || isdigit(name[i]))){
            return false;
        }
    }
    if (!isalpha(name[0])){
        return false;
    }
    for(i=0; i<NUM_OF_OPCODES; i++){
        if (!strcmp(name, opcodeNames[i])){
            return false;
        }
    }
    return true;
}

/**
Function: insertArrayFromArray

Input:
    from: an array to copy from.
    to: an array which we want to copy.

**/
void insertArrayFromArray(int * from, int * to, int length){
    int i;
    for(i = 0; i<length; i++){
        to[i]=from[i];
    }
}
