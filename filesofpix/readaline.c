/**************************************************************
 *
 *                     readaline.c
 *
 *     Assignment: filesofpix
 *     Authors:  Harrison Tun (htun01) and Brendan Roy (broy02)
 *     Date:     1/25/25
 *
 *     Summary:
 *     This file implements the 'readaline' function, which reads 
 *     a single line of text from a given file and dynamically 
 *     allocates memory to store the line. The function handles 
 *     memory resizing to accommodate lines of arbitrary length. If the line 
 *     is empty or reaches EOF without reading any characters, 
 *     it sets the output pointer to NULL and returns 0. Additional helper 
 *     functions, such as 'allocateMemory' and 'resize', are used 
 *     for memory management.
 *     
 *
 **************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
void *allocMemory(void *newVariable, int size, int length);
void resize(char** line, size_t *capacity, size_t counter);

/********** readaline ********
*
* Reads a single line in from a given file and returns the number of chars in
* the line and the line itself through the datapp variable. A line is defined
* by having at least one character and the newline character is included with
* the line it terminates
*
* Parameters:
*      FILE *inputfd - A pointer to a FILE stream that reads the line
*      char **datapp - double pointer to a char that holds the address to the 
*                      string that the line will be put into.
*
* Returns: the number of characters in the line, represented as a size_t
*
* Expects
*       the FILE stream to be open and *datapp to be declared. Neither is NULL
* Notes:
*      Will allocate memory for *datapp to store, which must be freed by the
*      caller of readaline
************************/
size_t readaline(FILE *inputfd, char **datapp)
{
        assert(inputfd != NULL && datapp != NULL); 
        char curr = '\0'; 
        size_t counter = 0;
        size_t capacity = 64;
        char *line = allocMemory(line, 1, capacity);
        while ((curr = fgetc(inputfd)) != EOF) {
                assert(!ferror(inputfd));
                if (counter == capacity) { resize(&line, &capacity, counter); }
                line[counter] = curr;
                counter++;
                if (curr == '\n') { break; } 
        }
        /* Handle empty file case: line is freed and we set datapp to null */
        if (counter == 0) { 
                free(line);
                *datapp = NULL;
                return 0;
        }
        /* Truncate extra memory in line array */
        resize(&line, &capacity, counter);

        *datapp = line;
        return counter;
}
/********** allocMemory ********
*
* Function that allocates memory for any variable type and returns a pointer to
* that variable
*
* Parameters:
*      void *newVariable - a pointer-to-void that represents the variable 
*                          to be initialized
*      int size - the size of the variable type
*      int length - length of the new variable (how many elements are needed)
*
* Return: a pointer-to-void of the address of the variable whose memory has just
*         been allocated
*
* Expects
*      size and length ≠ 0
* Notes:
*      throws Checked Runtime Error if memory cannot be allocated using malloc
************************/
void *allocMemory(void *newVariable, int size, int length)
{
        newVariable = malloc(size * length);
        assert(newVariable != NULL);
        return newVariable;
}

/********** resize ********
*
* Either gets more memory for the line pointer or truncates it, depending on the
* fullness of the line array.   
*
* Parameters:
*      char **line - a pointer a string that is the line we are reading
*      size_t *capacity - the current capacity (size of the char array)
*      size_t counter - the number of elements in the char array
*
* Return: returns nothing; expands or truncates the given array
*
* Expects
*      None of the arguments should be NULL
* Notes:
*      Throws Checked Runtime Error if memory cannot be allocated using realloc
************************/
void resize(char** line, size_t *capacity, size_t counter) 
{
        if (counter == *capacity) {
                *capacity *= 2;
                *line = realloc(*line, *capacity);
        } else { 
                *line = realloc(*line, counter);
        }
        assert(*line != NULL); 
}