/**************************************************************
 *
 *                     um.c
 *
 *     	Assignment: um
 *     	Authors:  Harrison Tun (htun01), Arman Kassam (akassa01)
 *     	Date:     4/9/25
 * 
 *    	um.c is the mothership of an emulated computer, relying on operations.c 
 *	to act as an ALU and memory.c to house all necessary storage. The UM
 *	simply calls said modules when necessary, and most of the details of
 *	the workings of the computer are abstracted from it.
 *
 **************************************************************/
#include "um.h"
#include "operations.h"
const int NUM_REGISTERS = 8;
const int OP_CODE_SHIFT = 28;

/********** read_prog ********
*
* Reads in an input file of human readable instructions. Converts the human
* readable instructions into readable binary for the universal machine
*
* Parameters:
*      FILE *input - a pointer to an input file
*      uint32 *program_length - a pointer to a uint32 that represents the length
*                               of the program
*	   
*
* Return: void
*
* Expects
*      The pointer to the input file must not be NULL
*      The pointer to the program length must not be NULL
* Notes:
*      CRE for any assertion failure
************************/
static Seq_T read_prog(FILE *input, uint32_t *program_length)
{
        assert(input != NULL);
        assert(program_length != NULL);

        /* Moves the file pointer to the end of the file to see the size of the 
           input file. Rewind is used for the second read that converts the 
           input file to um readable text. 
        */
        
	fseek(input, 0, SEEK_END);
	long file_size = ftell(input);
	rewind(input);
 	*program_length = file_size / 4;
	Seq_T program = Seq_new(*program_length);

	for (uint32_t i = 0; i < *program_length; i++) {
		uint32_t word = ((uint64_t)((uint8_t)fgetc(input)) << 24) | 
		((uint64_t)((uint8_t)fgetc(input)) << 16) | 
		((uint64_t)((uint8_t)fgetc(input)) << 8) | 
		(uint64_t)((uint8_t)fgetc(input));
		
		Seq_addhi(program, (void *)(uintptr_t)word);
   	}

    	return program;
}
/********** UM_execute ********
*
* Executes a single instruction from the UM's program
*
* Parameters:
*      UM *um - a pointer to the UM struct
*
* Return: void
*
* Expects
*      'um' pointer must not be NULL
*       segment 0 is currently mapped
*       program counter is within the bounds of program
* Notes:
*      CRE for any failed assertions
************************/
void UM_execute(UM *um) 
{
	assert(um != NULL);
	assert(um->program_counter < um->program_length);

	uint32_t instruction = (uint32_t)(uintptr_t)Seq_get(
					um->program, um->program_counter);
	um->program_counter++;
	uint32_t opcode = instruction >> OP_CODE_SHIFT;
	assert(opcode < 14);

	switch(opcode) {
		case 0:
		    	op_conditional_move(um, instruction);
		    	break;
		case 1:
			op_segmented_load(um, instruction);
			break;
		case 2:
			op_segmented_store(um, instruction);
			break;
		case 3:
			op_add(um, instruction);
			break;
		case 4:
			op_multiply(um, instruction);
			break;
		case 5:
			op_divide(um, instruction);
			break;
		case 6:
			op_bitwise_nand(um, instruction);
			break;
		case 7:
			um->halted = 1;
			break;
		case 8:
			op_map(um, instruction);
			break;
		case 9:
			op_unmap(um, instruction);
			break;
		case 10:
		   	op_output(um, instruction);
		    	break;
		case 11:
		    	op_input(um, instruction);
		    	break;
		case 12:
		    	op_load_program(um, instruction);
		    	break;
		case 13:
		    	op_load_value(um, instruction);
		    	break;
	}
}
/********** UM_initialize ********
*
* Initializes the Universal Machine. Unpacks the read in file and converts the 
* contents to readable information for the UM (32-bit codewords)
*
* Parameters:
*      FILE *input - a pointer to an input file
*
* Return: A pointer to a UM
*
* Expects
*      FILE *input to not be NULL
* Notes:
*      CRE for any failed assertions
*      This function calls the Memory module to initialize the um's memory
************************/
UM *UM_initialize(FILE *input)
{
	UM *um = malloc(sizeof(UM));
	assert(um != NULL);

	for (int i = 0; i < NUM_REGISTERS; i++) {
		um->registers[i] = 0;
	}
	um->memory = Memory_new();
	um->program = read_prog(input, &um->program_length);

	um->program_counter = 0;
	um->halted = 0;
	return um;
}

/********** um_free ********
*
* Frees all memory that corresponds to the UM struct.
*
* Parameters:
*      UM *um - a pointer to a um struct
*
* Return: void
*
* Expects
*      'um' must not be NULL
* Notes:
*      CRE if 'um' is NULL
*      This function relies on the Memory module to free the um's allocated 
*      memory.
************************/
void UM_free(UM *um)
{
    assert(um != NULL);
	Seq_free(&um->program);
	Memory_free(um->memory);
    free(um);
}
