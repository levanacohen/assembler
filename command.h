#ifndef _COMMAND_H
#define _COMMAND_H

#include "stack.h"
#include "memory.h"

#define MAX_CHARACTERS_IN_LABEL 30
#define MAX_NUM_OF_OPERANDS 2
#define NUM_OF_OPCODES 16
#define NUM_BITS_IN_ARRAY 12
#define NUM_OF_BITS_ERA 2
#define NUM_OF_BITS_ADDRESSING_SRC 2
#define NUM_OF_BITS_ADDRESSING_DEST 2
#define NUM_OF_BITS_OPCODE 4
#define NUM_OF_BITS_GROUP 2
#define MASK 1
#define COMMA ','
#define SPACE ' '
#define PLUS_CHAR '+'
#define MINUS_CHAR '-'
#define FILE_TYPE "as"
#define DOT '.'
#define ONE '1'
#define TWO '2'
#define END_OF_STRING '\0'
#define END_OF_LINE '\n'
#define REGISTER_ADDRESSING 'r'
#define DOLAR '$'
#define POUND '#'
#define COLON ':'
#define QUOTES '"'
#define DOLAR_NO_PREV 4
#define NUM_OF_REGISTERS 8
#define SOURCE 1
#define DESTINATION 0
#define ADDRESSING_METHOD_NOT_FOUND -1
#define NUM_OF_ADDRESSING_METHODS 4
#define MAX_WORD_LENGTH 80
#define MAX_LINE_LENGTH 82
#define WHITE_SPACES 32


typedef enum{
    ABSOLUT, EXTERNAL, RELOCATABLE
} EraMethods;

typedef enum{
    IMMIDIATE, DIRECT, PREVIOUS_COPY, DIRECT_REGISTER
} AddressingMethods;

typedef enum{
    MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, RED,
    PRN, JSR, RTS, STOP
} CommandOpcode;

typedef enum{
    NO_OPERNDS, ONE_OPERAND, TWO_OPERANDS
} Group;

typedef enum{
    DATA, STRING, ENTRY, EXTERN
} CommandType;

struct command_t{
    EraMethods era;
    AddressingMethods addressingSrcOperand;
    AddressingMethods addressingDestOperand;
    CommandOpcode opcode;
    Group groupCommand;
    char* label;
    int prevBinary[NUM_BITS_IN_ARRAY];
    AddressingMethods prevAddressing;
    int prevIndexSymbol;
    CommandType commandType_in;
};

typedef struct command_t * Command;

typedef enum{
    COMMAND_SUCCESS,
    COMMAND_INPUT_NULL,
    COMMAND_EMPTY_COMMAND,
    COMMAND_ALLOCATION_ERROR,
    COMMAND_INVALID_STRUCTURE,
    COMMAND_LABEL_INVALID_NAME,
    COMMAND_LABEL_NOT_DEFINED,
    COMMAND_ONLY_ONE_DOLAR,
    COMMAND_ADDRESSING_INVALID,
    COMMAND_GROUP_INVALID_NUM_OF_OPERANDS,
    COMMAND_MEMORY_ERROR,
    COMMAND_STACK_ERROR,
    COMMAND_DATA_INVALID,
    COMMAND_STRING_INVALID,
    COMMAND_ENTRY_INVALID,
    COMMAND_EXTERN_INVALID,
    COMMAND_ADDRESSING_METHODS_ERROR,
    COMMAND_OPCODE_TO_BINARY_FAILED,
    COMMAND_DOLAR_NO_PREV,
    COMMAND_STRING_ERROR,
    COMMAND_INVALID_ADDRESSING_FOR_OPCODE,
    COMMAND_OPCDE_NO_TIMES,
    COMMAND_POUND_INVALID,
    COMMAND_STRING_EXPECTED_QUOTES,
    COMMAND_LABEL_ALONE,
    COMMAND_ENTRY_NO_LABEL,
    COMMAND_EXTERN_NO_LABEL,
    COMMAND_SYMBOL_DECLARED_ENTRY,
    COMMAND_SYMBOL_DECLARED_EXTERN,
    COMMAND_LABEL_EXPECTED_COLON,
    COMMAND_LABEL_INVALID_NAME_REGISTER,
    COMMAND_OPCODE_UNRECOGNIZED,
    COMMAND_SYMBOL_REDECLARED
} CommandResult;

/**
Function commandCreate:

Input:
    command_string:  a string representing a valid command in assembly.
    stack_in:   the program's stack to be updated in the creation process.
    memoryData_in: the program's memory to be updated in the creation process.
                    This memory contains data variables only.
    memoryCode_in:  the program's memory to be updated in the creation process.
                    This memory contains code only.
    prevAddressing_in: the addressing method of the previous command.
    prevBinary_in: the previous command's binary code.
    prevIndexSymbol: the index of the symbol from the previous command.
    line_number: the number of the line in the input file containing the command.
    command_result_out: a command result containing:
                COMMAND_SUCCESS: the input command is valid.
                COMMAND_INVALID_STRUCTURE: the input command structure is invalid.
                COMMAND_LABEL_INVALID_NAME:
                        -Label name must be less than MAX_CHARACTERS_IN_LABEL characters.
                        -The first character must be a letter.
                        -The rest of the characters must be letters or digits.
                COMMAND_ADDRESSING_INVALID: The addressing method is not known.
                COMMAND_GROUP_INVALID_NUM_OF_OPERANDSL: The number of operands must
                                be in the range [0, MAX_NUM_OF_OPERANDS].
                COMMAND_OPCODE_NOT_EXIST: The command name is not in known - meaning, it's
                                not in OPCODES_NAMES.


Output:
    Command:    NULL if string not valid.
                otherwise returns a valid command.
                if returns NULL, returns the error in command_result_out.
**/

Command commandCreate(char* command_string, Stack stack_in, Memory memoryData_in, Memory memoryCode_in,
                      AddressingMethods prevAddressing_in, int* prevBinary_in, int prevIndexSymbol,
                      int line_number, CommandResult* command_result_out);

/**
Function commandFromCommandToBinary:

Input:
    command_in:  a struct contains the command info.
    binary:      The binary form of the command.
Output:
    command_result_out: a command result containing:
                COMMAND_SUCCESS: the input command is valid.
                COMMAND_INVALID_STRUCTURE: the input command structure is invalid.
                COMMAND_LABEL_INVALID_NAME:
                        -Label name must be less than MAX_CHARACTERS_IN_LABEL characters.
                        -The first character must be a letter.
                        -The rest of the characters must be letters or digits.
                COMMAND_ADDRESSING_INVALID: The addressing method is not known.
                COMMAND_GROUP_INVALID_NUM_OF_OPERANDSL: The number of operands must
                                be in the range [0, MAX_NUM_OF_OPERANDS].
                COMMAND_OPCODE_NOT_EXIST: The command name is not in known - meaning, it's
                                not in OPCODES_NAMES.

**/
CommandResult commandFromCommandToBinary(Command command_in, int binary[NUM_BITS_IN_CELL]);


#endif /* _COMMAND_H */
