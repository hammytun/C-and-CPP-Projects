/**************************************************************
 *
 *                     memory.h
 *
 *     	Assignment: um
 *     	Authors:  Harrison Tun (htun01), Arman Kassam(akassa01)
 *     	Date:     4/9/25
 *
 *     	Interface for an emulated memory that is able to map and
 *	unmap segments and store data within them. 
 *
 *	Importantly: segment 0, or the active program segment, is not 
 *	handled by this implementation of memory. Rather, it expects that 
 *	the program is handled and kept track of by the client.
 *
 **************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include "seq.h"
#include <stdbool.h>
#include <assert.h>
#include "bit.h"

#ifndef MEMORY_H
#define MEMORY_H

#define Memory Memory_T
typedef struct Memory *Memory;

Memory Memory_new();
void Memory_free(Memory_T mem);
uint32_t Memory_map(Memory_T mem, uint32_t size);
uint32_t Memory_get(Memory_T mem, uint32_t segment_id, uint32_t offset);
void Memory_set(Memory_T mem, uint32_t segment_id, uint32_t offset, 
								uint32_t value);
void Memory_unmap(Memory_T mem, uint32_t segment_id);
Seq_T Memory_get_segment(Memory_T mem, uint32_t segment_id);

#endif