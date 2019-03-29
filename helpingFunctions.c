#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#define NUM_BITS_IN_ARRAY 12
#define MAX_CHARACTERS_IN_LABEL 30

bool isBinary(int *array){
    int i;
    for (i=0; i<NUM_BITS_IN_ARRAY; i++){
        if (array[i]!=0 && array[i]!=1){
            return false;
        }
    }
    return true;
}


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
    return true;
}
/**
Function: insertArrayFromArray
Input:

Output:
**/
void insertArrayFromArray(int * from, int * to, int length){
    int i;
    for(i = 0; i<length; i++){
        to[i]=from[i];
    }
}

