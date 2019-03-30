#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#define FILE_EXTENSION ".as"
#define MAX_LENGTH_FILE_NAME 64

int main(int args, char *argv[]){
    AssemblerResult result_assembler;
    Assembler newAssembler;
    int count=0;
	FILE* file;
	if(args == 1){
		printf("\nfile Name(s) is needed\n");
		exit(1);
	}

	while(++count != args){ /*read arguments number of files entered*/
        char s2[MAX_LENGTH_FILE_NAME+1];
        /* copying the file name into s variable*/
        memcpy(s2, argv[count], strlen(argv[count])+1);
        /* updating s to contain the full name, including .as*/
        strcat(s2, FILE_EXTENSION);

        /* Opening the file number count */
        file = fopen(s2, "r");
        if(!file){
            printf("an error appear when trying read file %s\n", argv[count]);
            continue;
        }

        /* first pass */
        newAssembler = assemblerCreate(file, argv[count], &result_assembler);
        if(result_assembler!=ASSEMBLER_SUCCESS){
            continue;
        }

        /* second pass */
        if(assemblerRun (newAssembler, argv[count]) != ASSEMBLER_SUCCESS){
            continue;
        }
        fclose(file);
        free(newAssembler);
	}
    return 0;
}
