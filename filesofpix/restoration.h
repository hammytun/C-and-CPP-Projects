/**************************************************************
 *
 *                     restoration.h
 *
 *     Assignment: filesofpix
 *     Authors:  Harrison Tun (htun01) and Brendan Roy (broy02)
 *     Date:     1/25/25
 *
 *     Summary:
 *     This header file defines the interface of restoration.c, which restores
 *     corrupted P2 PGM files. It provides the interface for identifying and 
 *     removing non-digit characters from the corrupted file, converting the 
 *     cleaned data into the binary P5 format, and outputting the final 
 *     restored image as a raw pgm.
 *
 **************************************************************/
#include "readaline.h"
#include <stdio.h>
#include <stdlib.h>
#include "seq.h"
#include <stdbool.h>
#include <ctype.h>
#include "atom.h"
#include "table.h"
#include "string.h"
#include "assert.h" 
void processPixel(char **currChar, int *i, int lineLength, Seq_T *newLine, 
                size_t *newLineSize);
char *seqToString(Seq_T *line);
char *findSequence(Seq_T *lines, Seq_T *sizes);
void readFile(Seq_T *lines, Seq_T *sizes, FILE *inputfd);
void processLines(Seq_T *lines, Seq_T *sizes, const char *infused);
Seq_T checkLine(Seq_T *lines, Seq_T *sizes, const char *infused, int i, 
                size_t *newLineSize);
void deleteInfo(Seq_T lines, Seq_T sizes, Seq_T *newLine, int currIdx);
void writeImage(Seq_T *lines, Seq_T *sizes);
void printRow(char *row, int numPixels);
int distToSpace(char *row);
void *allocMemory(void *newVariable, int size, int length);