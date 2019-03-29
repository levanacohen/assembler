#include "program.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>


void printArray(int * array, int length);
char* skipSpace(char* string_in);
void fromIntToBinary(int number, int binary[NUM_BITS_IN_ARRAY]);
char* saveWord(char* string_in, int n, char * output);
void insertArrayFromArray(int * from, int * to, int length);
void printErrorCommand(char * file_name, int line_number, CommandResult commandResult_in, bool firstError);


void printErrorCommand(char * file_name, int line_number, CommandResult commandResult_in, bool firstError){
    if(firstError){
        printf("error(s) at file '%s':\n", file_name);
    }
    printf("Line %d:\t", line_number);
    switch(commandResult_in){
        case COMMAND_INPUT_NULL:
        case COMMAND_EMPTY_COMMAND:
        case COMMAND_ALLOCATION_ERROR:
        case COMMAND_SUCCESS:
        case COMMAND_LABEL_NOT_DEFINED:
        case COMMAND_ADDRESSING_INVALID:
        case COMMAND_MEMORY_ERROR:
        case COMMAND_STACK_ERROR:
        case COMMAND_OPCODE_TO_BINARY_FAILED:
        case COMMAND_STRING_ERROR:
            break;
        case COMMAND_INVALID_STRUCTURE: printf("the operand is invalid.");
            break;
        case COMMAND_LABEL_INVALID_NAME: printf("invalid name of label.");
            break;
        case COMMAND_ONLY_ONE_DOLAR: printf("syntax error: invalid operand $$. NOTE: for previous copy addressing method you have to insert $$.");
            break;
        case COMMAND_GROUP_INVALID_NUM_OF_OPERANDS: printf("invalid number of operands.");
            break;
        case COMMAND_DATA_INVALID: printf("syntax error: invalid .data syntax.");
            break;
        case COMMAND_STRING_INVALID: printf("syntax error: invalid .string syntax.");
            break;
        case COMMAND_ENTRY_INVALID: printf("syntax error: command .entry invalid.");
            break;
        case COMMAND_EXTERN_INVALID: printf("syntax error: command .extern invalid.");
            break;
        case COMMAND_ADDRESSING_METHODS_ERROR: printf("syntax error: unrecognized parameters. NOTE: the parameters should be addressing using one of the known methods.");
            break;
        case COMMAND_DOLAR_NO_PREV: printf("error: there is no previous operand to copy.");
            break;
        case COMMAND_INVALID_ADDRESSING_FOR_OPCODE: printf("error: addressing method is not suitable to the command opcode.");
            break;
        case COMMAND_OPCDE_NO_TIMES: printf("error: number of times the instruction encoding is not specified.");
            break;
        case COMMAND_POUND_INVALID: printf("error: the number specified for immediate addressing is invalid.");
            break;
        case COMMAND_STRING_EXPECTED_QUOTES: printf("syntax error: expected quotes.");
            break;
        case COMMAND_LABEL_ALONE: printf("syntax error: empty label.");
            break;
        case COMMAND_ENTRY_NO_LABEL: printf("syntax error: entry statement without label.");
            break;
        case COMMAND_EXTERN_NO_LABEL: printf("syntax error: extern statement without label.");
            break;
        case COMMAND_SYMBOL_DECLARED_ENTRY: printf("redeclared label: this label already declared by .entry statement");
            break;
        case COMMAND_SYMBOL_DECLARED_EXTERN: printf("redeclared label: this label already declared by .extern statement");
            break;
        case COMMAND_LABEL_EXPECTED_COLON: printf("syntax error: expected ':' after label");
            break;
        case COMMAND_LABEL_INVALID_NAME_REGISTER: printf("error: name of register can not be used as a label name");
            break;
        case COMMAND_OPCODE_UNRECOGNIZED: printf("error: invalid opcode.");
            break;
    }
    printf("\n");
}

Program programCreate(FILE* file_in, char* file_name ,Memory memoryData_in,
                      Memory memoryCode_in, Stack stack_in, ProgramResult* result_out){
    int errors = 0; /* count errors */
    bool firstError = true;
    Program newProgram = (Program)malloc(sizeof(Program));
    int prevBinary[NUM_BITS_IN_CELL] = {0}; /* save the binary of the first operand, of the previous line, for $$ */
    AddressingMethods prevAddressing = DOLAR_NO_PREV; /* saves the previous addressing method of the previous first operand, of the previous line, for $$.
            first use it initialize to DOLAR_NO_PREV, symbolize that there is no operand of the previous line. */
    int indexCommands = 0; /* count the input lines without white spaces or comments */
    CommandResult command_result_out; /* the result of the commandCreate function, means if the input line was valid */
    int line_number = 0;
    char line_buffer_const[MAX_LINE_LENGTH];
    char * line_buffer = line_buffer_const; /* for fgets. save the next line */
    Command newCommand; /* command to create */
    int prevIndexSymbol = -1; /* the index of the previous symbol in the stack, for $$ addressing method */

    *result_out = PROGRAM_SUCCESS;
    if(!file_in){
        *result_out = PROGRAM_FILE_NULL;
        return NULL;
    }

    /* the loop get line by line, if the Sentence is neither comment ';' nor white spaces,
       it sends it to commandCreate function, that parse the sentence and update the memory and stack */
    while(fgets(line_buffer, MAX_LINE_LENGTH, file_in)){
        ++line_number;
        /* without \n character */
        if(line_buffer[strlen(line_buffer)-1] == END_OF_LINE){
            line_buffer[strlen(line_buffer)-1] = '\0';
        }
        /* skip spaces */
        line_buffer = skipSpace(line_buffer);
        /* an empty sentence, white characters */
        if(!strlen(line_buffer)){
            continue;
        }
        /* comment sentence */
        if(line_buffer[0] == SEMICOLON){
            continue;
        }
        /* create command */
        newCommand = commandCreate(line_buffer, stack_in, memoryData_in,
                        memoryCode_in, prevAddressing ,prevBinary, prevIndexSymbol, line_number, &command_result_out);

        if(!newCommand){
            printErrorCommand(file_name, line_number, command_result_out, firstError);
            errors++;
            firstError = false;
            continue;
        }
        newProgram->all_commands[indexCommands] = newCommand;
        indexCommands++;
        if(indexCommands!=0){
            /* update the variables that saves the values of the previous sentence, for next $$ */
            insertArrayFromArray(newProgram->all_commands[indexCommands-1]->prevBinary, prevBinary, NUM_BITS_IN_CELL);
            prevAddressing = newProgram->all_commands[indexCommands-1]->prevAddressing;
            prevIndexSymbol = newProgram->all_commands[indexCommands-1]->prevIndexSymbol;
        }
    }
    errors?  printf("build file '%s' failed: %d error(s).\n",file_name, errors): printf("file '%s': build success.\n", file_name); /* TODO: DELETE print success */

    return newProgram;
}


ProgramResult programSecondPass(Program program, Memory memoryData_in, Memory memoryCode_in, Stack stack_in, char* label_out){
    int lengthOfData; /* ID */
    int lengthOfCode; /* CD */
    int i, j; /* index */
    int binary[NUM_BITS_IN_CELL]; /* save the binary cell */
    int addressDecimalOfSymbol = -1; /* get the address of the stack at decimal that symbol points on it */
    bool isData = false; /* if the symbol points on data values */
    GuideStatement type; /* entry or extern */
    int lastLineUndef; /* save the line of the last undefined label */
    char* lastLabelUndef; /* save the last undefined label */
    int indexLabel;

    if(!program || !memoryData_in || !memoryCode_in || !stack_in){
        return PROGRAM_INPUT_NULL;
    }
    /* get number of cells of memory data */
    if(memoryGetNumOfCells(memoryData_in, &lengthOfData)!=MEMORY_SUCCESS){
        return PROGRAM_MEMORY_INVALID;
    }
    /* get number of cells of memory code */
    if(memoryGetNumOfCells(memoryCode_in, &lengthOfCode)!=MEMORY_SUCCESS){
        return PROGRAM_MEMORY_INVALID;
    }
    /* Checks if the number of cells together is not greater than SIZE_OF_MEMORY  */
    if(lengthOfCode + lengthOfData > SIZE_OF_MEMORY){
        return PROGRAM_SIZE_MEMORY_OVER;
    }

    /* going through all the memory code cells, and check if symbol undefined */
    for(i=FIRST_ADDRESS; i<FIRST_ADDRESS+lengthOfCode; i++){
        if(memoryGetCellBinaryByAddress(memoryCode_in, i, binary)!=MEMORY_SUCCESS){
            return PROGRAM_GET_CELL_ERROR;
        }
        /* if binary[0]==-1 it means that this cell saves a label that undefined. we must update the it */
        if(binary[0]==-1){
            /* binary[1] saves the index of the request label in the stack */
            indexLabel = binary[1];
            if(stackGetSymbolAddressDecimalByIndex(stack_in, indexLabel, &addressDecimalOfSymbol) != STACK_SUCCESS){
                return PROGRAM_STACK_ERROR;
            }
            /* check if the label defined as entry or extern */
            if(stackGetSymbolTypeByIndex(stack_in, indexLabel, &type)!=STACK_SUCCESS){
                return PROGRAM_STACK_ERROR;
            }
            /* check if the label pointing on address saves data */
            if(stackGetIsDataSymbolByIndex(stack_in, indexLabel, &isData)!=STACK_SUCCESS){
                return PROGRAM_STACK_ERROR;
            }
            if(isData){
                /* Concatenating the data (in memory data) to the code (in memory code) */
                addressDecimalOfSymbol += lengthOfCode;
            }
            /* if addressDecimalOfSymbol not in the range [FIRST_ADDRESS,lengthOfCode+lengthOfData+FIRST_ADDRESS],
                    it means that the symbol's address did not updated, and it follows that the symbol is not set */
            if(!(addressDecimalOfSymbol >= FIRST_ADDRESS && addressDecimalOfSymbol <= lengthOfCode+lengthOfData+FIRST_ADDRESS)){
                if(stack_in->arrayOfSymbol[indexLabel]->type==EXTERN_G){
                    int binary_ext[] = {0,0,0,0,0,0,0,0,0,0,0,1};
                    if(cellUpdateBinary(memoryCode_in->memory_cells[i-FIRST_ADDRESS], binary_ext)!=CELL_SUCCESS){
                        return PROGRAM_CELL_UPDATE_ERROR;
                    }
                    continue;
                }
                if((stackGetNameByIndex(stack_in, indexLabel, &label_out))!=STACK_SUCCESS){
                    return PROGRAM_SYMBOL_ERROR;
                }
                /* if this is the same line and the same label, like the previous error message,
                   we don't need to print error again. binary[2] save the line that the undefined label used. */
                if(lastLineUndef == binary[2] && lastLabelUndef == label_out){
                    continue;
                }
                printf("line %d: undefined label '%s'.\n" , binary[2] , label_out);
                /* update variables */
                lastLineUndef = binary[2];
                lastLabelUndef = label_out;
                continue;
            }
            /* update the cell in the memory to the address of the symbol, at binary representation */
            fromIntToBinary(addressDecimalOfSymbol, binary);
            /* the 2 last bits means it is External coding */
            for(j=0; j<NUM_BITS_IN_ARRAY-2; j++){
                binary[j] = binary[j+2];
            }
            binary[10] = 1;
            binary[11] = 0;
            if(cellUpdateBinary(memoryCode_in->memory_cells[i-FIRST_ADDRESS], binary)!=CELL_SUCCESS){
                return PROGRAM_CELL_UPDATE_ERROR;
            }
        }
    }
    /* going through all the stack cells, and check if symbol that declared by .entry statement undeclared */
    for(j=0; j<stack_in->numOfSymbols; j++){
        if(stack_in->arrayOfSymbol[j]->type==ENTRY_G){
            if(stackGetSymbolAddressDecimalByIndex(stack_in, j, &addressDecimalOfSymbol)!=STACK_SUCCESS){
                return PROGRAM_STACK_ERROR;
            }
            if(addressDecimalOfSymbol<=0){
                /* the symbol undeclared */
                if((stackGetNameByIndex(stack_in, j, &label_out))!=STACK_SUCCESS){
                    return PROGRAM_SYMBOL_ERROR;
                }
                printf("line %d: undefined label '%s'. the label declared at .entry statement.\n", -1*addressDecimalOfSymbol, label_out);
            }
        }
    }
    /* DELETE ME THIS PRINTING!!! */
    printf("\n\n\n\nstack: %d\n\n",stack_in->numOfSymbols);
    for(j=0; j<stack_in->numOfSymbols; j++){
        printf("%d\t%s\t%d\t%d\t%d\n",j,stack_in->arrayOfSymbol[j]->name,
               stack_in->arrayOfSymbol[j]->addressDecimal, stack_in->arrayOfSymbol[j]->type, stack_in->arrayOfSymbol[j]->isData);
    }
    return PROGRAM_SUCCESS;
}
