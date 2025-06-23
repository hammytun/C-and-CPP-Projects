/**************************************************************
 *
 *                     um.h
 *
 *     	Assignment: um
 *     	Authors:  Harrison Tun (htun01), Arman Kassam (akassa01)
 *     	Date:     4/9/25
 * 
 *   um.h is the mothership of an emulated computer, relying on operations.c 
 *	to act as an ALU and memory.c to house all necessary storage. 
 *
 **************************************************************/
#ifndef UM_H
#define UM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "memory.h"
#include "seq.h"

typedef struct UM {
	uint32_t registers[8];
	Memory memory;
	Seq_T program;
	uint32_t program_length;
	uint32_t program_counter;
	int halted;
} UM;

UM *UM_initialize(FILE *input);
void UM_execute(UM *um);
void UM_free(UM *um);

#endif