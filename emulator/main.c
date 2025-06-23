/**************************************************************
 *
 *                     main.c
 *
 *     	Assignment: um
 *     	Authors:  Harrison Tun (htun01), Arman Kassam (akassa01)
 *     	Date:     4/9/25
 *
 *      Driver file for the UM program
 *
 **************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "um.h"
#include <time.h>

/********** open_file ********
*
* Reads and opens a file from the command line
*
* Parameters:
* int argc		the number of arguments on the command line
* char *argv[] 		
*
* Return: an initialized file pointer
*
* Expects
*      	argc is 2 and argv[1] is a valid filename
* Notes:
*      	EXIT FAILURE if file is NULL, if there is an improper read,
*	or 
************************/
static FILE *open_file(int argc, char *argv[]) 
{
	FILE *fp;
        /* quit if too many args, if 1 use stdin, if 2 create file pointer */
        if (argc != 2) {
                fprintf(stderr, "Usage: ./um [input file]\n");
                exit(EXIT_FAILURE);
        } else {
                fp = fopen(argv[1], "rb");
                if (fp == NULL || ferror(fp)) {
                        fprintf(stderr, "Invalid file\n");
                        exit(EXIT_FAILURE);
                }
        }
        return fp;
}

/********** main ********
*
* driver file for the UM program
*
* Parameters:
*      	int argc         	the number of arguments on the command line
*	char *argv[]		the array of strings on the command line
*
* Return: 0 to indicate program success
*
* Expects
*	argc == 2, argv[1] is a valid file
* Notes:
*      	allocates and frees the file memory as well as the UM itself 
************************/
int main(int argc, char *argv[]) 
{
	FILE* input = open_file(argc, argv);
	UM *um = UM_initialize(input);
        fclose(input);

	while (!um->halted) {
		UM_execute(um);  
	}

	UM_free(um);

	
	return 0;
}