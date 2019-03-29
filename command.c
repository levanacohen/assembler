#include "command.h"
#include "symbol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>


bool isValidAddressing(AddressingMethods op, bool isSourceOp, CommandOpcode commandOpcode_in);
void insertArrayFromArray(int * from, int * to, int length);
CommandResult createBinaryForRegisters(int r1, int r2, int binary[NUM_BITS_IN_CELL]);
void fromIntToBinary(int number, int binary[NUM_BITS_IN_ARRAY]);
void fromCharToBinary(char ch, int binary[NUM_BITS_IN_ARRAY]);
bool isNameIsValid(char *name);
void printArray(int * array, int length);
char * getNextWord(char * string_in, char ** restOfTheSentence);
char * skipSpace(char* string_in);
char* saveWord(char* string_in, int n);
int findIndexOfRegister(int binary[NUM_BITS_IN_ARRAY]);
int insertBinaryFromPlace(int binary[NUM_BITS_IN_CELL], int bcd, int highLimit, int lowLimit);
CommandResult commandRecognizeAddressingMethods(Command command_in, char* word_in, AddressingMethods* methodsAddress,
                                Memory memoryCode_in, AddressingMethods* currAddressing_out, int * currBinary_out,
                                int * register_out, bool isSecondOperand, int prevRegister, bool isSourceOperand,
                                Stack stack_in, int line_number);
CommandResult _commandImaginaryCommandString(char ** pointerRef, char ** stringArray);
CommandResult _commandImaginaryCommandData(char * pointerRef,
                                 int * dataArray, int * lengthOfData);


Command commandCreate(char* command_string, Stack stack_in, Memory memoryData_in, Memory memoryCode_in,
                      AddressingMethods prevAddressing_in, int * prevBinary_in, int prevIndexSymbol,
                      int line_number, CommandResult* command_result_out){

    char* pointer; /* save the sentence and going throw it */
    char* word = ""; /* save word word in the sentence */
    int memoryAddressDecimal_data; /* the next address in the memory code */
    int memoryAddressDecimal_code; /* the next address in the memory data */
    Command newCommand;
    int i, index = -1;
    char* comma = NULL; /* pointer on the comma between 2 operands (if there) */
    char * restOfSentence = "";
    int labelAddress;
    int dataArrayLength;
    AddressingMethods methodsA; /* addressing methods of operands */
    int binaryArray[NUM_BITS_IN_ARRAY] = {0}; /* an array to update the memory */
    bool isTwice = false; /* if we need to encode the operation twice */
    int commandBinary[NUM_BITS_IN_CELL] = {0}; /* The first word structure in an Action Statement */
    int firstRegister = 8, secondRegister = -1;
    char* lastCharAtWord; /* for an action sentence, to check if specified 1 or 2, i.e. mov2 */
    int currBinaryOperand[NUM_BITS_IN_ARRAY] = {0}; /* saves this coding of first operand, for $$ at next line */
    AddressingMethods currAddressingOperand = IMMIDIATE; /* the addressing method of the first operand */
    bool noTimes = false; /* if the times to encode the operation no specified */
    bool isLabel = false; /* if there is label */

    Cell commandCell;
    const char *opcodeNames[NUM_OF_OPCODES] = {"mov", "cmp", "add", "sub", "not", "clr", "lea",
                        "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

    printf("\n\ncommand_string:\t %s\n", command_string);  /* DELETE ME */
    if(!command_result_out || !command_string){
        *command_result_out = COMMAND_INPUT_NULL;
        return NULL;
    }
    if(!stack_in){
        *command_result_out =  COMMAND_STACK_ERROR;
        return NULL;
    }
    if(!memoryData_in || !memoryCode_in){
        *command_result_out =  COMMAND_MEMORY_ERROR;
        return NULL;
    }

    /* get contemporary addresses memory  */
    if(memoryGetNumOfCells(memoryCode_in, &memoryAddressDecimal_code) != MEMORY_SUCCESS){
        *command_result_out = COMMAND_MEMORY_ERROR;
        return NULL;
    }
    if(memoryGetNumOfCells(memoryData_in, &memoryAddressDecimal_data) != MEMORY_SUCCESS){
        *command_result_out = COMMAND_MEMORY_ERROR;
        return NULL;
    }
    /* Create a command */
    newCommand = (Command)malloc(sizeof(struct command_t));
    if(!newCommand){
        *command_result_out = COMMAND_ALLOCATION_ERROR;
        return NULL;
    }
    /* update data */
    newCommand->label = (char *) malloc(sizeof(int)* MAX_CHARACTERS_IN_LABEL);
    newCommand->prevAddressing = prevAddressing_in;
    newCommand->prevIndexSymbol = prevIndexSymbol;
    insertArrayFromArray(prevBinary_in, newCommand->prevBinary, NUM_BITS_IN_CELL);

    /* Indicates the beginning of the line. this pointer will advance word word */
    pointer = command_string;
    word = getNextWord(pointer, &pointer);
    if(!word){
        *command_result_out = COMMAND_EMPTY_COMMAND;
        return newCommand;
    }
    /* Check if this is label */
    if(word[(strlen(word)-1)] == COLON){
        bool isData = false; /* means if it is .data or .string statement */
        isLabel = true;

        /*the ':' character is not part of the label*/
        word[(strlen(word)-1)] = END_OF_STRING;
        if(!isNameIsValid(word)){
            *command_result_out = COMMAND_LABEL_INVALID_NAME;
            return NULL;
        }

        /* name of register cannot be a label name */
        if(strlen(word)>=2 && *word==REGISTER_ADDRESSING && isdigit(word[2])
           && atoi(word+2)<NUM_OF_REGISTERS && atoi(word+2)>=0){
            *command_result_out = COMMAND_LABEL_INVALID_NAME_REGISTER;
            return NULL;
        }

        /* goes to the next word */
        strcpy(newCommand->label, word);
        free(word);
        word = getNextWord(pointer, &pointer);
        if(!word){
            *command_result_out = COMMAND_LABEL_ALONE;
            return NULL;
        }

        labelAddress = FIRST_ADDRESS + memoryAddressDecimal_code;

        if(!(strcmp(word, ".data")&&strcmp(word, ".string"))){
            /*if this is data, the address calculates differently*/
            labelAddress = labelAddress + memoryAddressDecimal_data-memoryAddressDecimal_code;
            isData = true;

        }
        /* create a symbol in the stack */
        if(stackAddSymbol(stack_in, newCommand->label, labelAddress, isData, -1, &index)!=STACK_SUCCESS){
            *command_result_out = COMMAND_STACK_ERROR;
            return NULL;
        }
        free(newCommand->label);
    }
    /* Checks if this .data Statement */
    if(!strcmp(word, ".data")){
        /* an array saves the numbers at .data statement */
        int * dataArray = (int*) malloc(sizeof(int) * strlen(pointer));
        newCommand->commandType_in = DATA;

        if(_commandImaginaryCommandData(pointer,dataArray, &dataArrayLength)!=COMMAND_SUCCESS){
            free(dataArray);
            *command_result_out = COMMAND_DATA_INVALID;
            return NULL;
        }

        /* add the data values */
        for(i=0; i<dataArrayLength; i++){
            fromIntToBinary(dataArray[i], binaryArray);
            if(memoryAddCell(memoryData_in, binaryArray, index)!=MEMORY_SUCCESS){
                *command_result_out = COMMAND_MEMORY_ERROR;
                return NULL;
            }
            /* the symbol gets the address of the first data, after, it initialize to -1 */
            if(i==0){
                index = -1;
            }
        }
        free(dataArray);

        return newCommand;
    }
    /* Checks if this .string Statement */
    if(!strcmp(word, ".string")){
        char * stringValue; /* points on the first value */
        newCommand->commandType_in = STRING;
        if(_commandImaginaryCommandString(&pointer, &stringValue)!=COMMAND_SUCCESS){
            *command_result_out = COMMAND_STRING_INVALID;
            return NULL;
        }
        for(i=0; i<=strlen(stringValue); i++){  /* including the \0 character */
            fromCharToBinary(stringValue[i], binaryArray); /* Ascii representation of the character */
            if(memoryAddCell(memoryData_in, binaryArray, index)!=MEMORY_SUCCESS){
                *command_result_out = COMMAND_MEMORY_ERROR;
                return NULL;
            }
            /* the symbol gets the address of the first character, after, it initialize to -1 */
            if(i==0){
                index = -1;
            }
        }
        free(stringValue);
        return newCommand;
    }
    /* Check if this .entry Statement */
    if(!strcmp(word, ".entry")){
        StackResult result_stack;
        /*get the symbol to entry*/
        free(word);
        word = getNextWord(pointer, &pointer);
        if(!word){
            *command_result_out = COMMAND_ENTRY_NO_LABEL;
            return newCommand;
        }

        if(!isNameIsValid(word)){
            *command_result_out = COMMAND_LABEL_INVALID_NAME;
            return NULL;
        }
        /* we insert the number of the line in the address field */
        if((result_stack = stackAddSymbol(stack_in, word, -1*line_number, false, ENTRY_G, &index))!=STACK_SUCCESS){
            if(result_stack==STACK_SYMBOL_DECLARED_EXTERN){
                /* if the symbol already declared in .extern statement, it invalid */
                *command_result_out = COMMAND_SYMBOL_DECLARED_EXTERN;
                return NULL;
            }
            *command_result_out = COMMAND_STACK_ERROR;
            return NULL;
        }

        /*if there is another word in a sentence, it unnecessary*/
        if (getNextWord(pointer, &pointer)){
            *command_result_out = COMMAND_ENTRY_INVALID;
            return NULL;
        }
        newCommand->commandType_in = ENTRY;
        *command_result_out = COMMAND_SUCCESS;
        return newCommand;
    }
    /* Checks if this .extern Statement */
    if(!strcmp(word, ".extern")){
        StackResult result_stack;
        /*get the symbol to extern*/
        free(word);
        word = getNextWord(pointer, &pointer);
        if(!word){
            *command_result_out = COMMAND_EXTERN_NO_LABEL;
            return NULL;
        }

        if(!isNameIsValid(word)){
            *command_result_out = COMMAND_LABEL_INVALID_NAME;
            return NULL;
        }
        if ((result_stack = stackAddSymbol(stack_in, word, 3, false, EXTERN_G, &index)) != STACK_SUCCESS){
            if(result_stack==STACK_SYMBOL_DECLARED_ENTRY){
                /* if the symbol already declared in .entry statement, it invalid */
                *command_result_out = COMMAND_SYMBOL_DECLARED_ENTRY;
                return NULL;
            }
            *command_result_out = COMMAND_STACK_ERROR;
            return NULL;
        }
        /*If there is another word in a sentence, it unnecessary*/
        if (getNextWord(pointer, &pointer)){
            *command_result_out = COMMAND_EXTERN_INVALID;
            return NULL;
        }
        newCommand->commandType_in = EXTERN;
        *command_result_out = COMMAND_SUCCESS;
        return newCommand;
    }

    /* Checks if this an action Statement */
    lastCharAtWord = word+strlen(word)-1;
    if(*lastCharAtWord == ONE || *lastCharAtWord == TWO){
        if(*lastCharAtWord == TWO){
            isTwice = true;
        }
        *lastCharAtWord = END_OF_STRING;
    }
    else{
        noTimes = true;
        if(lastCharAtWord+1){
            lastCharAtWord[1] = END_OF_STRING;
        }
    }

    for(i=0; i<NUM_OF_OPCODES; i++){
        if (!strcmp(word, opcodeNames[i])){
            CommandResult res = COMMAND_SUCCESS;
            if(noTimes){ /* if the times to encode the operation no specified */
                *command_result_out = COMMAND_OPCDE_NO_TIMES;
                return NULL;
            }
            /*Encoding type of command is absolute*/
            newCommand->era = ABSOLUT;
            /*Insert a command binary to the memory. at first it's value is not important*/
            if(memoryAddCell(memoryCode_in, commandBinary, index)!= MEMORY_SUCCESS){
                *command_result_out = COMMAND_MEMORY_ERROR;
                return NULL;
            }

            commandCell = memoryCode_in->memory_cells[memoryAddressDecimal_code];
            newCommand->opcode = i;
            free(word);

            word = getNextWord(pointer, &restOfSentence);

            /* check if there is comma, and intactness in relation to the command */
            if(!pointer){

                if(i!=RTS && i!=STOP){
                    /* in another opcode, must be at least 1 operand. else it invalid */
                    *command_result_out = COMMAND_GROUP_INVALID_NUM_OF_OPERANDS;
                    return NULL;
                }
            }
            else{
                comma = strchr(pointer, COMMA);
            }
            if(i<=SUB || i==LEA){  /* these opcodes require two operands */

                char* firstOperand;
                char* secondOperand;
                if(!comma){   /* must be a comma */
                    *command_result_out = COMMAND_GROUP_INVALID_NUM_OF_OPERANDS;
                    return NULL;
                }
                secondOperand = getNextWord(comma+1, &restOfSentence);

                /*  checks if there is comma (that separates between 2 operands),
                    and if there is no another comma.
                    and if there is no another word. */
                if(!secondOperand || !comma || strchr(comma+1, COMMA) || getNextWord(restOfSentence, &restOfSentence)){
                    *command_result_out = COMMAND_GROUP_INVALID_NUM_OF_OPERANDS;
                    return NULL;
                }

                firstOperand = comma[-1]==SPACE? word : saveWord(pointer, comma-pointer);
                newCommand->groupCommand = TWO_OPERANDS;
                /* checks addressing methods of the first operand */
                if((res = commandRecognizeAddressingMethods(newCommand, firstOperand, &methodsA, memoryCode_in, &currAddressingOperand,
                                                     currBinaryOperand, &firstRegister, false, 8, true, stack_in, line_number)) != COMMAND_SUCCESS){
                    *command_result_out = res;
                    return NULL;
                }
                newCommand->addressingSrcOperand = methodsA;
                free(word);
                if(comma[-1]==SPACE){
                    free(firstOperand);
                }
                /* checks addressing methods of the second operand */
                if((res = commandRecognizeAddressingMethods(newCommand, secondOperand, &methodsA, memoryCode_in, &currAddressingOperand,
                                                     currBinaryOperand, &secondRegister, true, firstRegister, false, stack_in, line_number)) != COMMAND_SUCCESS){
                    *command_result_out = res;
                    return NULL;
                }
                if(secondOperand){
                    free(secondOperand);
                }
                newCommand->addressingDestOperand = methodsA;

            }
            else if(i>=NOT && i<=JSR && i!=LEA){ /* these opcodes require 1 operand */
                /*  checks if there is only 1 operand,
                    if there is no comma, (because there is only 1 argument)
                    and if there is no another words in this Sentence */

                if(!word || comma || getNextWord(restOfSentence, &restOfSentence)){
                    *command_result_out = COMMAND_GROUP_INVALID_NUM_OF_OPERANDS;
                    return NULL;
                }
                newCommand->groupCommand = ONE_OPERAND;
                /* checks addressing methods of the operand */
                if((res = commandRecognizeAddressingMethods(newCommand, word, &methodsA, memoryCode_in, &currAddressingOperand,
                                                     currBinaryOperand, &firstRegister, false, 8, false, stack_in, line_number)) != COMMAND_SUCCESS){
                    *command_result_out = res;
                    return NULL;
                }
                newCommand->addressingSrcOperand = 0; /* the value of addressingSrcOperand doesn't matter */
                newCommand->addressingDestOperand = methodsA;

            }
            else{  /* i=14 or i=15 */
                /* checks if there are no arguments  */

                if(word){
                    *command_result_out = COMMAND_GROUP_INVALID_NUM_OF_OPERANDS;
                    return NULL;
                }
                newCommand->groupCommand = NO_OPERNDS;
                /* the value of addressingSrcOperand and addressingDestOperand is not important */
                newCommand->addressingSrcOperand = 0;
                newCommand->addressingDestOperand = 0;
                newCommand->prevAddressing = DOLAR_NO_PREV;
            }

            break; /* loop 'for' */
        }
    }

    if(i==16) {
        /* if the opcode didn't recognized */
        if(isLabel){
            *command_result_out = COMMAND_OPCODE_UNRECOGNIZED;
        } else /* if 1 or 2 didn't specified, and there is no label,
                apparently forgotten the colon after label name*/
            *command_result_out = COMMAND_LABEL_EXPECTED_COLON;
        return NULL;
    }
    /* Translating the data of the first word to bits */
    if(commandFromCommandToBinary(newCommand, commandBinary)!=COMMAND_SUCCESS){
        *command_result_out = COMMAND_OPCODE_TO_BINARY_FAILED;
        return NULL;
    }
    /* update the first word */
    if(cellUpdateBinary(commandCell, commandBinary)!=CELL_SUCCESS){
        *command_result_out = COMMAND_MEMORY_ERROR;
        return NULL;
    }
    /* if we need to encoding the statement twice, i.e. mov2 */
    if(isTwice){
        int indexSymbol;
        int binary_out[NUM_BITS_IN_CELL] = {0};
        /* add the first word to the memory */
        if( memoryAddCell(memoryCode_in, commandBinary, -1)!=MEMORY_SUCCESS){
            *command_result_out = COMMAND_MEMORY_ERROR;
            return NULL;
        }
        if((newCommand->groupCommand) != NO_OPERNDS){
            /* at least 1 operand, add the first operand again */
            memoryGetCellBinaryByAddress(memoryCode_in, memoryAddressDecimal_code+FIRST_ADDRESS+1, binary_out);
            memoryGetIndexSymbolByAddress(memoryCode_in, memoryAddressDecimal_code+FIRST_ADDRESS+1, &indexSymbol);
            memoryAddCell(memoryCode_in, binary_out, indexSymbol);
        }
        if(newCommand->groupCommand == TWO_OPERANDS &&(newCommand->addressingDestOperand!=DIRECT_REGISTER
                                    || newCommand->addressingSrcOperand!=DIRECT_REGISTER)){
            /* 2 operands, add the second operand to the memory again */
            memoryGetCellBinaryByAddress(memoryCode_in, memoryAddressDecimal_code+FIRST_ADDRESS+2, binary_out);
            memoryGetIndexSymbolByAddress(memoryCode_in, memoryAddressDecimal_code+FIRST_ADDRESS+2, &indexSymbol);
            memoryAddCell(memoryCode_in, binary_out, indexSymbol);
        }
    }
    /* update the first operand, for next $$ */
    newCommand->prevAddressing = currAddressingOperand;
    insertArrayFromArray(currBinaryOperand, newCommand->prevBinary, NUM_BITS_IN_CELL);
    *command_result_out = COMMAND_SUCCESS;
    return newCommand;
}


/**
Function: insertBinaryFromPlace
The function convert the bcd value into a binary value into the binary array between the
highLimit'th cell to the lowLimit'th cell.

Input:
    binary: an array that represent a cell in the memory, it stores a number of register.
    bcd: a integer.
    highLimit: an index in the binary array.
    lowLimit: an index in the binary array.

Output:
    1: in success
    0: otherwise
**/

int insertBinaryFromPlace(int binary[NUM_BITS_IN_CELL], int bcd, int highLimit, int lowLimit){
    int i;
    if(bcd>NUM_OF_OPCODES-1 || bcd<0 || !binary){
        return 0;
    }
    /* convert to binary, and insert to binary array */
    for(i=lowLimit; i>=highLimit; i--){
        binary[i] = bcd%2;
        bcd = bcd/2;
    }
    return 1;
}

/**
Function commandFromCommandToBinary:

Input:
    command_in: The command to produce memory cell based on the data stored in it.
    binary: An array that represents a cell in the memory, a command structure.

Output:
    COMMAND_SUCCESS: if no error happened.
    COMMAND_OPCODE_TO_BINARY_FAILED: if insertBinaryFromPlace function failed.
**/

CommandResult commandFromCommandToBinary(Command command_in, int binary[NUM_BITS_IN_CELL]){
    if(insertBinaryFromPlace(binary, command_in->era, 10, 11) /* at first word, bits 10,11 for ERA */
        && insertBinaryFromPlace(binary, command_in->addressingDestOperand, 8, 9)
        && insertBinaryFromPlace(binary, command_in->addressingSrcOperand, 6, 7)
        && insertBinaryFromPlace(binary, command_in->opcode, 2, 5)
        && insertBinaryFromPlace(binary, command_in->groupCommand, 0, 1)  ){
            return COMMAND_SUCCESS;
        }else {
            return COMMAND_OPCODE_TO_BINARY_FAILED;
        }
}


/**
Function commandRecognizeAddressingMethods:
the function recognize the operand and checks for validity.

Input:
    command_in: The command to produce memory cell based on the data stored in it.
    word_in: The operand.
    methodsAddress: the addressing method.
    memoryCode_in: the memory containing only the code parts.
    currAddressing_out: the current addressing method.
    currBinary_out: an integer array contains 1/0 represent the operand's encoding.
    register_out: if it is the first operand, it will store the register's index.
    isSecondOperand: a boolean contain TRUE if it is the second operand. FALSE otherwise.
    prevRegister: the value of the previous register if exist.
    isSourceOperand: a boolean contain TRUE if it is the source operand. FALSE otherwise.
    stack_in: a pointer to the stack.
    line_number: the number of the line in the input file containing the command.

Output:
    COMMAND_SUCCESS: if no error happened.
    COMMAND_MEMORY_ERROR
    COMMAND_OPCODE_TO_BINARY_FAILED: if insertBinaryFromPlace function failed.
    TODO: COMPLETE
**/
CommandResult commandRecognizeAddressingMethods(Command command_in, char* word_in, AddressingMethods* methodsAddress,
                                Memory memoryCode_in, AddressingMethods* currAddressing_out, int * currBinary_out,
                                int * register_out, bool isSecondOperand, int prevRegister, bool isSourceOperand,
                                Stack stack_in, int line_number){
    int binary [NUM_BITS_IN_ARRAY] = {0}; /* Coding operands to a binary number */
    int registerBinary [NUM_BITS_IN_ARRAY] = {0}; /* Coding registers to a binary number */

    if(!memoryCode_in){
        return COMMAND_MEMORY_ERROR;
    }
    if(!word_in || !methodsAddress || !command_in){
        return COMMAND_INPUT_NULL;
    }
    /* direct register addressing */
    if (word_in[0]== REGISTER_ADDRESSING){
        /* checks if the number of the register in the range [0,7] */
        int indexThisRegister;
        if (isdigit(word_in[1]) && 0<=(indexThisRegister=atoi(word_in+1)) && indexThisRegister<=7){
            /* if there is another letters at this operand, it can be a symbol.*/
            if(word_in[2]==SPACE || word_in[2]==END_OF_STRING){
                *methodsAddress = DIRECT_REGISTER;
                /* if the opcode require 2 operands and it is the first operand, haven't update it,
                because if the second operand register, the encoding will be different. */
                if(isSecondOperand || (command_in->groupCommand==ONE_OPERAND)){
                    if(!isValidAddressing(*methodsAddress,isSourceOperand, command_in->opcode)){
                        return COMMAND_INVALID_ADDRESSING_FOR_OPCODE;
                    }
                    if(createBinaryForRegisters(prevRegister, indexThisRegister, registerBinary)!= COMMAND_SUCCESS) {
                        return COMMAND_ADDRESSING_METHODS_ERROR;
                    }
                    memoryAddCell(memoryCode_in, registerBinary, -1);
                }
                *register_out = indexThisRegister;
                if(!isSecondOperand){
                    /* update currBinary_out, prevIndexSymbol and currAddressing_out for $$ */
                    if(createBinaryForRegisters(8 , indexThisRegister, currBinary_out)!= COMMAND_SUCCESS) {
                        return COMMAND_ADDRESSING_METHODS_ERROR;
                    }
                    *currAddressing_out = DIRECT_REGISTER;
                    command_in->prevIndexSymbol = -1; /* no label */
                }
                return COMMAND_SUCCESS;
            }
        }
    }


    /* add the previous register */
    if(isSecondOperand && prevRegister != NUM_OF_REGISTERS){
        /* if the addressing method is previous copy (by $$), and the first operand was register,
           we have to combine them. */
        if(word_in[0]==DOLAR && word_in[1]==DOLAR && (word_in[2]==SPACE || word_in[2]==END_OF_STRING)){
            if(command_in->prevAddressing == DIRECT_REGISTER){
                /* find the index of the first register of the previous command */
                int firstRegOfPrevCommand = findIndexOfRegister(command_in->prevBinary);
                if(createBinaryForRegisters(prevRegister, firstRegOfPrevCommand, registerBinary)!= COMMAND_SUCCESS) {
                    return COMMAND_ADDRESSING_METHODS_ERROR;
                }
                *methodsAddress = DIRECT_REGISTER;
                if(!isValidAddressing(*methodsAddress,isSourceOperand, command_in->opcode)){
                    return COMMAND_INVALID_ADDRESSING_FOR_OPCODE;
                }
                if(memoryAddCell(memoryCode_in, registerBinary, -1)!=MEMORY_SUCCESS){
                    return COMMAND_MEMORY_ERROR;
                }
                return COMMAND_SUCCESS;
            }
        }
        /* the second operand is not register or previous copy. add the first register */
        if(createBinaryForRegisters(prevRegister, NUM_OF_REGISTERS, registerBinary)!= COMMAND_SUCCESS) {
            return COMMAND_ADDRESSING_METHODS_ERROR;
        }
        if(!isValidAddressing(*methodsAddress,isSourceOperand, command_in->opcode)){
                return COMMAND_INVALID_ADDRESSING_FOR_OPCODE;
        }
        if(memoryAddCell(memoryCode_in, registerBinary, -1)!=MEMORY_SUCCESS){
            return COMMAND_MEMORY_ERROR;
        }
        if(!isSecondOperand){
            command_in->prevIndexSymbol = -1; /* no label */
        }
    }

    /* immidiate addressing */
    if (word_in[0] == POUND){
        bool isNegative = false; /* if the number is negative */
        int num = 0, i=1;
        if(word_in[1]==PLUS_CHAR || word_in[1]==MINUS_CHAR){
            i++;
            if(word_in[1]==MINUS_CHAR){
                isNegative = true;
            }
        }
        for(; i<strlen(word_in); i++){
            if(!isdigit(word_in[i])){
                return COMMAND_POUND_INVALID;
            }
            num = num*10 + (int)(word_in[i]-'0');
        }
        if(isNegative){
            num*=-1;
        }

        fromIntToBinary(num, binary);
        /* at immediate addressing,  the 2 first bits are 00 */
        for(i=0; i<NUM_BITS_IN_ARRAY-2; i++){
            binary[i] = binary[i+2];
        }
        binary[10] = binary[11] = 0;
        *methodsAddress = IMMIDIATE;
        if(!isValidAddressing(*methodsAddress,isSourceOperand, command_in->opcode)){
            return COMMAND_INVALID_ADDRESSING_FOR_OPCODE;
        }
        memoryAddCell(memoryCode_in, binary, -1);
        if(!isSecondOperand){
            command_in->prevIndexSymbol = -1;
            *currAddressing_out = IMMIDIATE;
            insertArrayFromArray(binary, currBinary_out, NUM_BITS_IN_CELL);
        }
        return COMMAND_SUCCESS;
    }

    /* Check for direct addressing */
    if (isNameIsValid(word_in)){
        int index = 0;
        *methodsAddress = DIRECT;
        binary[0] = -1; /* Notes that this label. at second pass, we will go over
        all the memory cells, that their left cell is -1 */
        if(stackAddSymbol(stack_in, word_in, 0, false, -1, &index)!=STACK_SUCCESS){
            return COMMAND_STACK_ERROR;
        }
        binary[1] = index; /* index of label in the stack */
        binary[2] = line_number; /* this line */
        if(!isSecondOperand){
            command_in->prevIndexSymbol = index; /* index of label */
            *currAddressing_out = DIRECT;
            insertArrayFromArray(binary, currBinary_out, NUM_BITS_IN_CELL);
        }
        if(memoryAddCell(memoryCode_in, binary, index)!=MEMORY_SUCCESS){
            return COMMAND_MEMORY_ERROR;
        }
        return COMMAND_SUCCESS;
    }

    /* checks for previous copy addressing */
    if (word_in[0]==DOLAR){
        if(word_in[1]!=DOLAR){
            /* only 1 $ */
            return COMMAND_ONLY_ONE_DOLAR;
        }
        if(word_in[2]!=SPACE && word_in[2]!=END_OF_STRING){
            return COMMAND_ADDRESSING_METHODS_ERROR;
        }
        /* If the previous command did not have operands, or is it the first command */
        if(command_in->prevAddressing == DOLAR_NO_PREV){
            return COMMAND_DOLAR_NO_PREV;
        }
        /* if the addressing method of previous operand is an immediate register and it is the first operand from 2. */
        if((command_in->prevAddressing == DIRECT_REGISTER) && (!isSecondOperand) && isSourceOperand){
            /* recovery operand and sending it to this function again */
            int prevRegister = findIndexOfRegister(command_in->prevBinary);
            char firstOperand[3];
            firstOperand[0] = REGISTER_ADDRESSING;
            firstOperand[1] = (char)prevRegister + '0';
            firstOperand[2] = END_OF_STRING;
            return commandRecognizeAddressingMethods(command_in, firstOperand, methodsAddress, memoryCode_in,
                currAddressing_out, currBinary_out, register_out, false, prevRegister, isSourceOperand, stack_in, line_number);
        }
        *methodsAddress = command_in->prevAddressing;
        *currAddressing_out = command_in->prevAddressing;
        if(!isValidAddressing(*methodsAddress,isSourceOperand, command_in->opcode)){
            return COMMAND_INVALID_ADDRESSING_FOR_OPCODE;
        }
        memoryAddCell(memoryCode_in, command_in->prevBinary, command_in->prevIndexSymbol);
        insertArrayFromArray(command_in->prevBinary, currBinary_out, NUM_BITS_IN_CELL);
        return COMMAND_SUCCESS;
    }
    return COMMAND_LABEL_INVALID_NAME;
}

/**
Function findIndexOfRegister:

Input:
    binary: an array that representation a cell in the memory, it is stored a number of register.

Output:
    indexOfRegister: The index of the register that stored in the memory.
**/
int findIndexOfRegister(int binary[NUM_BITS_IN_ARRAY]){
    int i;
    int indexOfRegister = 0;
    for(i=9; i>6; i--){
        indexOfRegister += binary[i]*pow(2,9-i);
    }
    return indexOfRegister;
}


/**
Function fromIntToBinary:

Input:
    number: The number to convert to binary.
    binary: An array which insert the binary representation of the number.

Output:
    binary: The number at base 2.
**/
void fromIntToBinary(int number, int binary[NUM_BITS_IN_ARRAY]){
    int i;
    /* if the absolute value of number greater than 2^11, Put the rightmost bits */
    for (i = NUM_BITS_IN_ARRAY-1; i >=0; i--) {
        (binary)[i] = ((MASK & number) ? 1 : 0);
        number = number >> 1;
    }
}

/**
Function fromCharToBinary:

Input:
    ch:  The character to convert to binary by ascii value.

Output:
    binary:  A ascii value of the character ch at base 2.
**/
void fromCharToBinary(char ch, int binary[NUM_BITS_IN_ARRAY]){
    int ascii = (int) ch;
    fromIntToBinary(ascii, binary);
}

/**
Function _commandImaginaryCommandString:
    check if .string statement is valid.

Input:
    newCommand:     The command given to the structure input.
    pointerRef:     a reference to a pointer points to the rest of the command.
    stringArray:  a pointer to a string to inert to the memory.

Output:
    CommandResult:
    COMMAND_SUCCESS: if no error happened.
    COMMAND_STRING_INVALID: if pointerRef string invalid.
    COMMAND_STRING_EXPECTED_QUOTES: if there are no quotes around the string.
**/
CommandResult _commandImaginaryCommandString(char ** pointerRef, char ** stringArray){
    char* pointer = *pointerRef; /* pointer on the beginning of the string */
    char* first_quotes = pointer;
    char* second_quotes;
    char* ch; /* for the loop */
    char* word;
    char * output ;

    if(first_quotes[0]!=QUOTES){
        return COMMAND_STRING_EXPECTED_QUOTES;
    }

    second_quotes = strchr(first_quotes+1, QUOTES);
    if(!second_quotes){
        return COMMAND_STRING_EXPECTED_QUOTES;
    }

    /* if there is another characters, it invalid */
    word = getNextWord(second_quotes+1, &pointer);
    if(word){
        free(word);
        return COMMAND_STRING_INVALID;
    }

    /* only seen ASCII characters or spaces (' ') are legal */
    for(ch=first_quotes+1; ch<second_quotes ;ch++){
        if(!isgraph(*ch) && *ch!=SPACE){
            return COMMAND_STRING_INVALID;
        }
    }

    output = (char*)malloc(sizeof(char) * (second_quotes - first_quotes -1));
    if(!output){
        return COMMAND_STRING_ERROR;
    }
    strncpy(output ,first_quotes+1, second_quotes - first_quotes-1);

    *(output+(second_quotes-first_quotes)-1) = END_OF_STRING;
    *stringArray = output;
    return COMMAND_SUCCESS;
}

/**
Function _commandImaginaryCommandData:
    check if .string statement is valid, and insert the numbers to dataArray.

Input:
    pointer:  The command given to the structure input.
    dataArray:  a pointer to an integers array created from the data's value.
    lengthOfData:  a pointer to an integer which we want to write how many numbers introduced.

Output:
    CommandResult:
    COMMAND_SUCCESS: if no error happened.
    COMMAND_DATA_INVALID: if pointerRef string input (the rest of the command) invalid.
**/
CommandResult _commandImaginaryCommandData(char * pointer,
                                 int dataArray[MAX_LINE_LENGTH], int * lengthOfData){
    int num, i,
        index = 0; /* index of dataArray */
    int length = strlen(pointer);
    bool comma = true, /* if was comma before the number */
        isPlus = true, /* positive number */
        isSignLastChar = false; /* last sign was '+' or '-' */
    *lengthOfData = 0;
    for (i=0; i<length; i++){
        /* if was characters or spaces between sign (- +) and number it invalid */
        if(isSignLastChar && !isdigit(pointer[i])){
            return COMMAND_DATA_INVALID;
        }
        if(pointer[i] == MINUS_CHAR || pointer[i] == PLUS_CHAR){
            isSignLastChar = true;
            isPlus = (pointer[i] == PLUS_CHAR) ? true : false;
            continue;
        }
        else{ /* the character is neither '-' nor '+' */
            isSignLastChar = false;
        }
        if(isdigit(pointer[i])){
            if(!comma){ /* did not was comma after previous number */
                return COMMAND_DATA_INVALID;
            }
            comma = false;
            num = atoi(pointer+i);
            if(!isPlus){
                num*= -1;
                isPlus = true;
            }
            dataArray[index] = num;
            index++;
            /* skip all the rest digits */
            for(; isdigit(pointer[i+1]); i++);
            continue;
        }
        if(pointer[i]==COMMA){
            if(comma){ /* Two consecutive commas */
                return COMMAND_DATA_INVALID;
            }
            comma = true;
            continue;
        }
        if(pointer[i]==SPACE){
            continue;
        }
        /* another character is invalid */
        return COMMAND_DATA_INVALID;
    }
    *lengthOfData = index;
    return COMMAND_SUCCESS;
}



/**
Function getNextWord:

Input:
    string_in:  a string to get the first word from.
    restOfTheSentence:  a pointer to the input sentence without the first word to output.

Output:
    returns a string which represent the first word of the input sentence.
    returns NULL if there are no other words in the sentence.
    returns NULL if there are more then one space in one place.

**/
char * getNextWord(char * string_in, char ** restOfTheSentence){
    int i;
    char* output;
    char * pointer = string_in;
    if(!string_in || strlen(string_in)==0){
        return NULL;
    }
    /* skip spaces */
    for(i=0; i<strlen(string_in); i++){
        if(string_in[i] == SPACE || string_in[i]<WHITE_SPACES){
            pointer = &string_in[i];
            break;
        }
    }
    /* The last word of the sentence */
    if(i==strlen(string_in)){
        *restOfTheSentence = NULL;
        return string_in;
    }
    pointer = string_in + i;
    /* The first character is space*/
    if(pointer - string_in == 0){
        return getNextWord(&string_in[1], restOfTheSentence);
    }

    /*The first character is not space*/
    output = saveWord(string_in, pointer - string_in);
    if(*pointer != '\0'){
        *restOfTheSentence = pointer + 1;
        *restOfTheSentence = skipSpace(*restOfTheSentence);
    }
    else{
        **restOfTheSentence = '\0';
    }
    return output; /**/
}


/**
Function saveWord:

Input:
    string_in:  a string to get the first n characters from.
    restOfTheSentence:  the number of characters to copy from string_in.

Output:
    returns a string which represent the first n characters from string_in.
    returns NULL if n is smaller than the length of string_in.
    returns NULL if allocation failed.

**/
char* saveWord(char* string_in, int n){
    char * output;
    if(n>strlen(string_in)){
        return NULL;
    }
    output = (char *)malloc(sizeof(char)*(n+1));
    if(!output){
        return NULL;
    }

    memcpy( output, string_in, n );
    output[n] = '\0';
    return output;
}

/**
Function skipSpace:

Input:
    string_in:  a string to skip on the white space from.

Output:
    returns a string which represent the string_in without the first white space.
    returns NULL if n is smaller than the length of string_in.
    returns NULL if allocation failed.
**/

char * skipSpace(char* string_in){
    if(!string_in){
        return NULL;
    }
    while(string_in){
    /* the 32 first characters of ASCII table represent white characters */
        if((*string_in!='\0') && ((isspace(*string_in))||(*string_in <= 32))){
            string_in++;
        }else{
            break;
        }
    }
    return string_in;
}

/**

Function createBinaryForRegisters:

Input:
    r1: the source operand, an index of register. if the value is NUM_OF_REGISTERS it symbolizes
                that the source operand is not register, or that there is no source operand.
    r2: the destination operand, an index of register. if the value is
                NUM_OF_REGISTERS it symbolizes that the destination operand is not register.
    binary: a binary array which we want to write the binary representation of the registers.

Output:
    COMMAND_SUCCESS: if no error happened.
    COMMAND_ADDRESSING_INVALID: if r1 or r2 not in the range [0,NUM_OF_REGISTERS-1].
**/
CommandResult createBinaryForRegisters(int r1, int r2, int binary[NUM_BITS_IN_CELL]){
    int bcd[NUM_OF_REGISTERS] = {0, 1, 10, 11, 100, 101, 110, 111}; /* binary representation */
    int i, val;
    int indexHigh_r1 = 0, indexLow_r1 = 4; /* bits 0-4 for r1 */
    int indexHigh_r2 = 5, indexLow_r2 = 9; /* bits 5-9 for r2 */

    for(i=0; i<NUM_BITS_IN_ARRAY; i++){
        binary[i] = 0;
    }

    if(!(r1 <= NUM_OF_REGISTERS && r1 >=0)){
        return COMMAND_ADDRESSING_INVALID;
    }else if(r1 < NUM_OF_REGISTERS && r1 >=0){
        val = bcd[r1];
        /* encoding the index of operand */
        for(i = indexLow_r1; i>=indexHigh_r1; i--){
            binary[i] = val%10;
            val = val/10;
        }
    }

    if(!(r2 <= NUM_OF_REGISTERS && r2 >=0)){
        return COMMAND_ADDRESSING_INVALID;
    }else if(r2 < NUM_OF_REGISTERS && r2 >=0){
        val = bcd[r2];
        /* encoding the index of operand */
        for(i = indexLow_r2; i>=indexHigh_r2; i--){
            binary[i] = val%10;
            val = val/10;
        }
    }
    return COMMAND_SUCCESS;
}

/**

Function isValidAddressing:

Input:
    op: the number of the addressing method.
    isSourceOp: if the operand is source operand, the value is true, false otherwise.
    commandOpcode_in: the number of the opcode.

Output:
    return true if the addressing method valid for the operand, otherwise return false.

**/
bool isValidAddressing(AddressingMethods op, bool isSourceOp, CommandOpcode commandOpcode_in){
    int  indexTable = DESTINATION;

        /*{{ADDRESSING_DST}, {ADDRESSING_SRC}} */
        int valid_addressing_for_op [NUM_OF_OPCODES][MAX_NUM_OF_OPERANDS][NUM_OF_ADDRESSING_METHODS] = {
          {{-1, 1,-1,3},{0,1,2,3}}, /* MOV */
          {{0,1,2,3},{0,1,2,3}}, /* CMP */
          {{-1, 1,-1,3},{0,1,2,3}}, /* ADD */
          {{-1, 1,-1,3},{0,1,2,3}}, /* SUB */
          {{-1, 1,-1,3},{-1,-1,-1,-1}}, /* NOT */
          {{-1, 1,-1,3},{-1,-1,-1,-1}}, /* CLR */
          {{-1, 1,-1,3},{-1,1,-1,-1}}, /* LEA */
          {{-1, 1,-1,3},{-1,-1,-1,-1}}, /* INC */
          {{-1,1,-1, 3},{-1,-1,-1,-1}}, /* DEC */
          {{-1,1,2,3},{-1,-1,-1,-1}}, /* JMP */
          {{-1,1,2,3},{-1,-1,-1,-1}}, /* BNE */
          {{-1,1,2,3},{-1,-1,-1,-1}}, /* RED */
          {{0,1,2,3},{-1,-1,-1,-1}}, /* PRN */
          {{-1,1,-1,-1},{-1,-1,-1,-1}}, /* JSR */
          {{-1,-1,-1,-1},{-1,-1,-1,-1}}, /* RTS */
          {{-1,-1,-1,-1},{-1,-1,-1,-1}}, /* STOP */
        };

    if(isSourceOp){
        indexTable = SOURCE;
    }
    if(valid_addressing_for_op[commandOpcode_in][indexTable][op]== ADDRESSING_METHOD_NOT_FOUND){
        return false;
    }
    return true;
}
