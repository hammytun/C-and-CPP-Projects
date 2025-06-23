/**************************************************************
 *
 *                     memory.c
 *
 *     	Assignment: um
 *     	Authors:  Harrison Tun (htun01), Arman Kassam (akassa01)
 *     	Date:     4/9/25
 *
 *     	Implementation for an emulated memory that is able to map and
 *	unmap segments as well as store uint32_ts in each segment. The 
 *	user will only be able to access mapped memory and segment size
 *	must be declared at time of mapping and abided by for any getting
 *	and setting instructions. 
 *
 *	Importantly: segment 0, or the active program segment, is not 
 *	handled by this implementation of memory. Rather, it expects that 
 *	the program is handled and kept track of by the client. A placeholder
 *	is put in Seq_T segments to ensure indices line up. If the client
 *	would like to load a different program from another segment, they can
 *	do so using the Memory_get_segment function. 
 *
 **************************************************************/
#include "memory.h"
#include <stdio.h>

/********** Memory_T ********
 *
 * A struct that emulates CPU memory
 *
 * Properties:
 *      uint32_t next_empty_segment     equivalent to the highest 
 *					mapped index + 1
 *      Seq_T unmapped_ids     		contains all unmapped indices
 *      Seq_T segments      		a sequence of sequences in which all
 *					program memory (minus program itself)
 *					is stored
 *	Bit_T mapped_bitmap		a bit array representing which segments
 *					are mapped and which are unmapped
 *
 ************************/
struct Memory_T { 
	uint32_t next_empty_segment;
	Seq_T unmapped_ids;
	Seq_T segments;
	Bit_T mapped_bitmap;
}; 

/********** resize_bitmap ********
*
* copies Bit_T old into a new bitmap with length new_length
*
* Parameters:
*      	Bit_T old		the old bitmap to be copied over
*      	uint32_t new_length		the length of the new bitmap
*
* Return: Bit_T
*
* Expects
*	old is not null
*      	new length is greater than old length
* Notes:
*      	frees the old bitmap, but expects the newly malloc'd bitmap to be 
*	freed elsewhere
*	relies on bit_new and bit_free to create and clean up memory properly
************************/
static Bit_T resize_bitmap(Bit_T old, uint32_t new_length)
{
	assert(old != NULL);
	uint32_t old_length = Bit_length(old);
	assert(new_length > old_length);

	/* create a new bitmap and copy over the elements */
	Bit_T new = Bit_new(new_length);
	for (uint32_t i = 0; i < old_length; i++) {
		if (Bit_get(old, i)) {
			Bit_put(new, i, 1);
		}
	}
	/* free the old one and return the new one*/
	Bit_free(&old);
	return new;
}

/********** is_mapped ********
*
* checks if the given segment id is mapped in memory, ie. if the id is lower
* than the lowest unmapped id and if the index is mapped
*
* Parameters:
*      	Memory_T mem		the memory structure
*      	uint32_t segment_id	the segment id to be checked
*
* Return: bool indicating if the id is mapped
*
* Expects
*	mem is not NULL
* Notes:
*      	none
************************/
static bool is_mapped(Memory_T mem, uint32_t segment_id)
{
	assert(mem != NULL);
	return segment_id < mem->next_empty_segment &&
	       Bit_get(mem->mapped_bitmap, segment_id);
}

/********** initialize_sequence ********
*
* creates a new sequence and fills it with zeroes up to index length
*
* Parameters:
*      	uint32_t length		the length of the new sequence
*
* Return: the new Seq_T
*
* Expects
*	length is an initialized value
* Notes:
*      	allocates memory with Seq_new and expects that the memory is freed by
*	the caller
*	relies on Seq_new to properly allocate memory
************************/
static Seq_T initialize_sequence(uint32_t length) 
{
	Seq_T array = Seq_new(length);
	for (uint32_t i = 0; i < length; i++) {
		Seq_addhi(array, (void*)(uintptr_t)(uint32_t)0);
	}
	return array;
}

/********** Memory_new ********
*
* creates and returns a new memory structure
*
* Parameters:
*      	none
*
* Return: Memory_T
*
* Expects
*	the program has enough resources to create a new mem object
* Notes:
*      	lots of memory is allocated here. the caller must call mem_free to 
*	ensure it is all cleaned up
*	a placeholder is put into index zero of mem->segments since it is 
*	expected that the client house the program on their side
*	relies on bit_new and seq_new to properly allocate memory
************************/
Memory_T Memory_new()
{
	Memory_T mem = malloc(sizeof(struct Memory_T));
	assert(mem != NULL);
	mem->next_empty_segment = 1;
	mem->unmapped_ids = Seq_new(100);
	mem->mapped_bitmap = Bit_new(100);
	mem->segments = Seq_new(100);

	/* throw in a placeholder for program at index 0 */
	Seq_addhi(mem->segments, (void*)(uintptr_t)(uint32_t)0);

	return mem;
}

/********** Memory_free ********
*
* frees a memory structure 
*
* Parameters:
*      	Memory_T mem		a memory structure
*
* Return: void
*
* Expects
*	mem is not NULL
* Notes:
*      	frees all memory previously allocated by memory_new
*	relies on seq_free and bit_free to properly clean up allocated memory
************************/
void Memory_free(Memory_T mem) 
{
	assert(mem != NULL);
	/* loop through segments and if mapped, free */
	for (int i = 1; i < Seq_length(mem->segments); i++) {
		if (is_mapped(mem, i)) {
			Seq_T to_free = Seq_get(mem->segments, i);
			Seq_free(&to_free);
		}
	}
	Seq_free(&mem->segments);
	Seq_free(&mem->unmapped_ids);
	Bit_free(&mem->mapped_bitmap);
	
	free(mem);
}

/********** Memory_map ********
*
* maps a new segment in memory of size size
*
* Parameters:
*      	Memory_T mem		the memory structure
*	uint32_t size		the size of the segment to be initialized
*
* Return: uint32_t representing the segment index
*
* Expects
*	mem is not NULL
* Notes:
*      	allocates memory by creating a new segment. it also may allocate memory
*	through resize_bitmap. it is expected that the caller will free this 
*	memory by later calling memory_free
************************/
uint32_t Memory_map(Memory_T mem, uint32_t size)
{
	assert(mem != NULL);
	Seq_T new_segment = initialize_sequence(size);

	/* if there are unmapped ids available, use that, otherwise use 
	next_empty segment and increment */
	uint32_t segment_id;
	if (Seq_length(mem->unmapped_ids) > 0) {
		segment_id = (uint32_t)(uintptr_t)Seq_remhi(mem->unmapped_ids);
		Seq_put(mem->segments, segment_id, new_segment);
	} else {
		Seq_addhi(mem->segments, new_segment);
		segment_id = mem->next_empty_segment;
		mem->next_empty_segment++;
	}

	/* expand the bitmap if necessary and update it with the newly mapped 
	index */
	if (segment_id >= (uint32_t)Bit_length(mem->mapped_bitmap)) {
		mem->mapped_bitmap = resize_bitmap(mem->mapped_bitmap, 
								segment_id * 2);
	}
	Bit_put(mem->mapped_bitmap, segment_id, 1);

	return segment_id;
}

/********** Memory_get ********
*
* gets a value in the given segment at the given offset
*
* Parameters:
*      	Memory_T mem		the memory structure
*	uint32_t segment_id	the segment id to get from
*	uint32_t offset		the offset to get at
*
* Return: the uint32_t at segments[segment_id][offset]
*
* Expects
*	mem is not NULL
*	the offset is less than the sequence length
*	the segment id is not zero and is mapped
* Notes:
*      	none
************************/
uint32_t Memory_get(Memory_T mem, uint32_t segment_id, uint32_t offset)
{
	assert(mem != NULL);
	assert(is_mapped(mem, segment_id));
	assert(segment_id != 0);
	Seq_T seg = Seq_get(mem->segments, segment_id);
	assert(offset < (uint32_t)Seq_length(seg));
	return (uint32_t)(uintptr_t)Seq_get(seg, offset);
}

/********** Memory_get ********
*
* sets segments[segment_id][offset] to the given value
*
* Parameters:
*      	Memory_T mem		the memory structure
*	uint32_t segment_id	the segment to place the value into
*	uint32_t offset		the offset to set at
*	uint32_t value		
*
* Return: void
*
* Expects
*	mem is not NULL
*	the offset is less than the sequence length
*	the segment id is not zero and is mapped
* Notes:
*      	none
************************/
void Memory_set(Memory_T mem, uint32_t segment_id, uint32_t offset, 
								uint32_t value)
{
	assert(mem != NULL);
	assert(is_mapped(mem, segment_id));
	assert(segment_id != 0);

	Seq_T seg = Seq_get(mem->segments, segment_id);
	assert(offset < (uint32_t)Seq_length(seg));
	Seq_put(seg, offset, (void*)(uintptr_t)value);
}

/********** Memory_unmap ********
*
* unmaps (frees) the segment at index segment id
*
* Parameters:
*      	Memory_T mem		the memory structure
*	uint32_t segment_id	the segment to place the value into
*	uint32_t offset		the offset to set at
*	uint32_t value		the value to insert 
*
* Return: void
*
* Expects
*	mem is not NULL
*	segment_id is not 0 and is mapped
* Notes:
*      	will abort if unmap tries to access unmapped memory
*	frees the segment at index segment id
*	accesses unmapped ids and mapped bitmap so that the id can be reused
*	and so that the rest of mem is aware this index is unmapped
************************/
void Memory_unmap(Memory_T mem, uint32_t segment_id)
{
	assert(mem != NULL);
	assert(segment_id != 0);
	assert(is_mapped(mem, segment_id));

	Seq_T to_free = Seq_get(mem->segments, segment_id);
	Seq_free(&to_free);
	Seq_addhi(mem->unmapped_ids, (void*)(uintptr_t)segment_id);
	Bit_put(mem->mapped_bitmap, segment_id, 0);
}

/********** Memory_get_segment ********
*
* duplicates and returns the sequence at segments[segment_id]
*
* Parameters:
*      	Memory_T mem		the memory structure
*	uint32_t segment_id	the segment id to get
*
* Return: a duplicate of the Seq_T at segments[segment_id]
*
* Expects
*	mem is not NULL
*	the segment id is not zero and is mapped
* Notes:
*      	allocates memory for a new segment and makes it identical to the one
*	at segments[segment_id]. assumes the memory is freed by the caller
************************/
Seq_T Memory_get_segment(Memory_T mem, uint32_t segment_id) 
{
	assert(mem != NULL);
	assert(is_mapped(mem, segment_id));
	assert(segment_id != 0);

	/* get the correct segment and create a new one */
	Seq_T seg = Seq_get(mem->segments, segment_id);
	Seq_T new_seg = initialize_sequence(Seq_length(seg));

	/* fill the new segment with the old one's elements */
	for (int i = 0; i < Seq_length(seg); i++) {
		uint32_t value = (uint32_t)(uintptr_t)Seq_get(seg, i);
		Seq_put(new_seg, i, (void*)(uintptr_t)value);
	}

	return new_seg;
}