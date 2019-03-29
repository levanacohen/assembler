assembler: command.c main.c assembler.c program.c helpingFunctions.c memory.c cell.c registers.c stack.c symbol.c
	gcc -o assembler -g -Wall -pedantic -ansi command.c -lm main.c assembler.c program.c helpingFunctions.c memory.c cell.c registers.c stack.c symbol.c -I.
