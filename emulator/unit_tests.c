#include "um.h"
#include "memory.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

/* Tests that the program correctly opens, reads, and closes the UM file 
   Will ensure that there is proper memory management and pointer cleanup,
   so there is no memory leaks.
*/
void test_FILEIO()
{
   char* filename = "umbin/codex.umz";
	FILE *fp = fopen(filename, "r");
	UM *um = UM_initialize(fp);
   printf("made it through\n");

	UM_free(um);
	
	fclose(fp);
}

void map_and_free() {
	Memory mem = Memory_new();
	int seg_id = Memory_map(mem, 5);
	assert(seg_id == 1);
	Memory_free(mem);
}

void map_and_unmap() {
	Memory mem = Memory_new();
	int seg_id = Memory_map(mem, 5);
	Memory_unmap(mem, seg_id);
	Memory_free(mem);
}

void lots_of_maps() {
	Memory mem = Memory_new();
	for (int i = 1; i < 10000; i++) {
		Memory_map(mem, 5);
	}
	uint32_t value = 772;
	Memory_set(mem, 5700, 2, value);
	Memory_set(mem, 5500, 4, value);

	Memory_unmap(mem, 5600);

	assert(Memory_get(mem, 5700, 2) == value);
	assert(Memory_get(mem, 5700, 2) == value);
	assert(Memory_get(mem, 5500, 4) == value);
	assert(Memory_map(mem, 5) == 5600);

	Memory_free(mem);
}

void map_set_free() {
	Memory mem = Memory_new();
	int seg_id = Memory_map(mem, 5);
	uint32_t value = 772;
	Memory_set(mem, seg_id, 2, value);
	Memory_free(mem);
}

void map_set_get() {
	Memory mem = Memory_new();
	int seg_id = Memory_map(mem, 5);
	uint32_t value = 772;
	Memory_set(mem, seg_id, 2, value);
	assert(Memory_get(mem, seg_id, 2) == value);
	assert(Memory_get(mem, seg_id, 1) == 0);
	Memory_free(mem);
}

void check_unmapped_ids() {
	Memory mem = Memory_new();

	for (int i = 1; i < 1000; i++) {
		Memory_map(mem, 5);
	}

	Memory_unmap(mem, 22);
	Memory_unmap(mem, 90);
	Memory_unmap(mem, 76);
	Memory_unmap(mem, 12);
	Memory_unmap(mem, 2);

	assert(Memory_map(mem, 5) == 2);
	assert(Memory_map(mem, 5) == 12);
	assert(Memory_map(mem, 5) == 76);
	assert(Memory_map(mem, 5) == 90);
	assert(Memory_map(mem, 5) == 22);
	assert(Memory_map(mem, 5) == 1000);

	Memory_free(mem);
}

void free_unmapped_ids() {
	Memory mem = Memory_new();

	for (int i = 1; i < 1000; i++) {
		Memory_map(mem, 5);
	}

	Memory_unmap(mem, 22);
	Memory_unmap(mem, 90);
	Memory_unmap(mem, 76);
	Memory_unmap(mem, 12);
	Memory_unmap(mem, 2);

	Memory_free(mem);
}

void duplicate_segment() {
	Memory mem = Memory_new();
	for (int i = 1; i < 100; i++) {
		Memory_map(mem, 5);
	}

	int seg_id = 37;
	uint32_t value = 772;
	Memory_set(mem, seg_id, 0, value);
	Memory_set(mem, seg_id, 1, value << 1);
	Memory_set(mem, seg_id, 2, value << 2);
	Memory_set(mem, seg_id, 3, value << 3);
	Memory_set(mem, seg_id, 4, value << 4);

	struct segment *duplicated = Memory_get_segment(mem, seg_id);

	for (int i = 0; i < 5; i++) {
		uint32_t n_value = (uint32_t)(uintptr_t)Seq_get(
							duplicated->array, i);
		uint32_t o_value = Memory_get(mem, seg_id, i);
		assert(n_value == o_value);
	}

	Memory_unmap(mem, seg_id);
	for (int i = 0; i < 5; i++) {
		uint32_t n_value = (uint32_t)(uintptr_t)Seq_get(
							duplicated->array, i);
		assert(n_value == value << i);
	}
	
	// Seq_T seq = duplicated->array;
	Seq_free(&duplicated->array);
	free(duplicated);

	// for (int i = 0; i < 5; i++) {
	// 	uint32_t n_value = (uint32_t)(uintptr_t)Seq_get(seq, i);
	// 	printf("%u\n", n_value);
	// }

	Memory_free(mem);
}

void memory_tests() {
	Memory mem = Memory_new();
	assert (mem != NULL);
	Memory_free(mem);

	map_and_free();
	map_and_unmap();
	lots_of_maps();
	map_set_free();
	map_set_get();
	check_unmapped_ids();
	free_unmapped_ids();
	duplicate_segment();
	
}


int main(int argc, char *argv) {
	// test_FILEIO();
	memory_tests();

	return 0;
}