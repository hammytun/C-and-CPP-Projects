/**************************************************************
 *
 *                     operations.c
 *
 *     	Assignment: um
 *     	Authors:  Harrison Tun (htun01), Arman Kassam (akassa01)
 *     	Date:     4/9/25
 * 
 *    	operations.c emulates an ALU, carrying out 13 main instructions. these
 *	range from simple adding to loading a new program. Operations also
 *	has separate functions to get and set the program segment itself, as
 *	the emulated version of memory it relies on expects that its clients
 *	do so. 
 *
 **************************************************************/
#include "operations.h"
#define REG_A(instruction) ((instruction >> 6) & 0x7)
#define REG_B(instruction) ((instruction >> 3) & 0x7)
#define REG_C(instruction) (instruction & 0x7)
const uint64_t MAX_VAL = ((uint64_t)1 << 32);
const uint32_t INPUT_VAL = 0xFFFFFFFF;
const uint32_t MAX_ASCII_VAL = 255;
const uint32_t LOAD_VAL_SIZE = 25;

/********** program_get ********
*
* Gets value at segment 0[offset]
*
* Parameters:
*       UM *um - a pointer to a UM struct
*       uint32_t instruction -  a 32 bit instruction
*
* Return: a uint32 that represents segment 0
*
* Expects
*      'um' must not be NULL
*       The user must not be trying to access a segment outside the bounds
* Notes:
*      CRE for any failed assertions
************************/
static uint32_t program_get(UM *um, uint32_t offset) 
{
	assert(um != NULL);
	assert(offset < um->program_length);
	return (uint32_t)(uintptr_t)Seq_get(um->program, offset);
}

/********** program_set ********
*
* sets value at segment 0 [offset]
*
* Parameters:
*      UM *um - a pointer to a UM struct
*      uint32_t instruction -  a 32 bit instruction
*      uint32_t value - value to be set in segment 0 [offset]
*
* Return: a uint32 that represents segment 0
*
* Expects
*      'um' must not be NULL
*       The user must not be trying to access a segment outside the bounds
* Notes:
*      CRE for any failed assertions
************************/
static void program_set(UM *um, uint32_t offset, uint32_t value)
{
        assert(um != NULL);
	assert(offset < um->program_length);
	Seq_put(um->program, offset, (void*)(uintptr_t)value);
}

/********** segmented_load ********
*
* pulls a value from memory from segment[segment_index][offset] and returns it
*
* Parameters:
*      	int segment_index		the segment index
*      	int offset			the offset in the segment
*	Memory *memory			the pointer to the memory structure
*
* Return: uint32_t
*
* Expects
*	Memory pointer is not null
*      	the segment_index refers to a mapped segment
*      	the offset is within the bounds of the segment
*      	the value is a valid 4 byte value
* Notes:
*      	calls memory.c to retrieve the value from memory using *memory
*       Branches depending on if the instruction wants to access segment 0
************************/
void op_segmented_load(UM *um, uint32_t instruction)
{
        assert(um != NULL);
	uint32_t a = REG_A(instruction);
   	uint32_t b = REG_B(instruction);
    	uint32_t c = REG_C(instruction);

	if (um->registers[b] == 0) {
		um->registers[a] = program_get(um, um->registers[c]);
	} else {
		um->registers[a] = Memory_get(um->memory, um->registers[b], 
							 um->registers[c]);
	}
}

/********** segmented_store ********
*
* Stores a 32 bit word in a specific memory segment based off a given offset
*
* Parameters:
*      int segment_index - the segment index
*      int offset -		the offset in the segment
*      uint32_t value - the value to store in the segment
*      Memory * memory - the pointer to the memory structure
*
* Return: void
*
* Expects
*      Memory pointer is not NULL
*      A valid segment_index that refers to a mapped segment
*      A valid value that fits within 32 bit word
*      
* Notes:
*      CRE if any of the expectations are not met
*      Branches depending on if the instruction wants to access segment 0
************************/
void op_segmented_store(UM *um, uint32_t instruction)
{
        assert(um != NULL);
	uint32_t a = REG_A(instruction);
   	uint32_t b = REG_B(instruction);
    	uint32_t c = REG_C(instruction);
	if (um->registers[a] == 0) {
		program_set(um, um->registers[b], um->registers[c]);
	} else {
		Memory_set(um->memory, um->registers[a], um->registers[b], 
							um->registers[c]);
	}
}

/********** map_segment ********
*
* Creates a new memory segment that has a specified number of 32-bit words,
* initializes all the words to 0 and maps it into the UM's memory system
*
* Parameters:
*      uint32_t size - the number of the 32 bit words in a new segment
*      Memory *memory - a pointer to the memory struct
*
* Return: a uint32_t representing the index of the newly mapped segment
*
* Expects
*      size is a positive nonzero integer
*      there is available memory to map the segment
* Notes:
*      uses malloc via memory.c to allocate the memory needed for the segment
*      the segment is initialized to all zeroes
*      memory cleanup is done by the caller using unmap_segment
************************/
void op_map(UM *um, uint32_t instruction)
{
        assert(um != NULL);
   	uint32_t b = REG_B(instruction);
    	uint32_t c = REG_C(instruction);
	um->registers[b] = Memory_map(um->memory, um->registers[c]);
}

/********** unmap_segment ********
*
* Deallocates a memory segment from the UM's memory system
*
* Parameters:
*      int segment_id - id for the segment to be unmapped
*      Memory *memory - a pointer to memory
*
* Return: void
*
* Expects
*      Memory pointer is not NULL
*      segment_id is a valid index of a mapped segment
* Notes:
*      CRE is thrown for any failed expectations
************************/
void op_unmap(UM *um, uint32_t instruction)
{
        assert(um != NULL);
	uint32_t c = REG_C(instruction);
	Memory_unmap(um->memory, um->registers[c]);
}

/********** load_program ********
*
* Duplicates the memory in segment[segment_index] and returns it
*
* Parameters:
*      UM *um - a pointer to a UM struct
*      uint32_t instruction -  a 32 bit instruction
*
* Return: void
*
* Expects
*      segment_index is a valid index of a mapped segment 
* Notes:
*      if segment_index = 0, returns immediately as no work needs to be done
*      memory is allocated for the duplication as the segment is copied
*      calls memory.c to handle the memory allocation
************************/
void op_load_program(UM *um, uint32_t instruction)
{
        assert(um != NULL);
	uint32_t b = REG_B(instruction);
    	uint32_t c = REG_C(instruction);
	if (um->registers[b] != 0) {
		Seq_T new_program = Memory_get_segment(um->memory, 
                                                       um->registers[b]);
		Seq_free(&um->program);
		um->program = new_program;
		um->program_length = Seq_length(new_program);
	}
	um->program_counter = um->registers[c];
}

/********** op_conditional_move ********
*
* Moves value from register B into register A if conditional statement is true
*
* Parameters:
*      UM *um - a pointer to a UM struct
*      uint32_t instruction -  a 32 bit instruction
*
* Return: void
*
* Expects
*      'um' must not be NULL
* Notes:
*      CRE if 'um' is NULL
************************/
void op_conditional_move(UM *um, uint32_t instruction)
{
	assert(um != NULL);
	uint32_t a = REG_A(instruction);
	uint32_t b = REG_B(instruction);
	uint32_t c = REG_C(instruction);
	if (um->registers[c] != 0) {
		um->registers[a] = um->registers[b];
	}
}

/********** op_add ********
*
* Adds the values of registers B and C. Puts the sum in register A.
*
* Parameters:
*      UM *um - a pointer to a UM struct
*      uint32_t instruction -  a 32 bit instruction
*
* Return: void
*
* Expects
*      'um' must not be NULL
* Notes:
*      CRE if 'um' is NULL
************************/
void op_add(UM *um, uint32_t instruction)
{
	assert(um != NULL);
	uint32_t a = REG_A(instruction);
	uint32_t b = REG_B(instruction);
	uint32_t c = REG_C(instruction);
	um->registers[a] = (um->registers[b] + um->registers[c]) % MAX_VAL;
}

/********** op_multiply ********
*
* Multiplies the value of Register B and C and puts the product into a 
* register C.
*
* Parameters:
*       UM *um - a pointer to a UM struct
*       uint32_t instruction -  a 32 bit instruction
*
* Return: void
*
* Expects
*      'um' must not be NULL
* Notes:
*      CRE if 'um' is NULL
************************/
void op_multiply(UM *um, uint32_t instruction)
{
	assert(um != NULL);
	uint32_t a = REG_A(instruction);
	uint32_t b = REG_B(instruction);
	uint32_t c = REG_C(instruction);
	um->registers[a] = (um->registers[b] * um->registers[c]) % MAX_VAL;
}

/********** op_divide ********
*
* Divides the value of register B by regster C and puts the quotient into  
* third register C.
*
* Parameters:
*       UM *um - a pointer to a UM struct
*       uint32_t instruction -  a 32 bit instruction
*
* Return: void
*
* Expects
*      'um' must not be NULL
*       The value in um->registers[c] must not equal 0
* Notes:
*      CRE if 'um' is NULL
*      CRE if um->registers[c] is 0 
************************/
void op_divide(UM *um, uint32_t instruction)
{
        uint32_t a = REG_A(instruction);
	uint32_t b = REG_B(instruction);
	uint32_t c = REG_C(instruction);
	assert(um != NULL);

        /* Divisor cannot be zero */
	assert(um->registers[c] != 0);
	um->registers[a] = um->registers[b] / um->registers[c];
}

/********** op_bitwise_nand ********
*
* Performs a bitwise and on the values of registers B and C. Then, negates the 
* resulting value which is then stored into register A.
*
* Parameters:
*       UM *um - a pointer to a UM struct
*       uint32_t instruction -  a 32 bit instruction
*
* Return: void
*
* Expects
*      'um' must not be NULL
* Notes:
*      CRE if 'um' is NULL
************************/
void op_bitwise_nand(UM *um, uint32_t instruction)
{
	assert(um != NULL);
	uint32_t a = REG_A(instruction);
	uint32_t b = REG_B(instruction);
	uint32_t c = REG_C(instruction);
	um->registers[a] = ~(um->registers[b] & um->registers[c]);
}
/********** op_output ********
*
* Prints a character value to stdout
*
* Parameters:
*       UM *um - a pointer to a UM struct
*       uint32_t instruction -  a 32 bit instruction
*
* Return: void
*
* Expects
*      'um' must not be NULL
*       The printed value must be <= 255
* Notes:
*      CRE if 'um' is not NULL
*      CRE if the value to be printed is greater than 255
************************/
void op_output(UM *um, uint32_t instruction)
{
        uint32_t c = REG_C(instruction);
	uint32_t value = um->registers[c];
	assert(um != NULL);
	assert (value <= MAX_ASCII_VAL);
	putchar((char)value);
}

/********** op_input ********
*
* Loads user input into a register
*
* Parameters:
*       UM *um - a pointer to a UM struct
*       uint32_t instruction -  a 32 bit instruction
*
* Return: void
*
* Expects
*      'um' must not be NULL
* Notes:
*      CRE if 'um' is NULL
*      If the end of the input has been signaled, a full 32 bit word with all
*      ones is loaded into the register.
************************/
void op_input(UM *um, uint32_t instruction)
{
	assert(um != NULL);
        uint32_t c = REG_C(instruction);
        int ch = getc(stdin);
        if (ch == EOF) {
                um->registers[c] = INPUT_VAL;
        } else {
                um->registers[c] = (uint32_t)ch;
        }
}

/********** op_load_value ********
*
* The first 3 immediately less significant bits after the opcode describe 
* register A. Then, the subsequent 25 bits are loaded into the same register
* 
*
* Parameters:
*       UM *um - a pointer to a UM struct
*       uint32_t instruction -  a 32 bit instruction
*
*
* Return: void
*
* Expects
*      'um' must not be NULL
* Notes:
*      CRE is 'um' is NULL
*      To get the 3 bits that describe register A, the instruction is shifted
*      right by 25 and a bitwise and operation is performed to single out the
*      3 bits.
*	   This function uses the Bitpack interface to grab a 25 bit word from
*	   the given 32 bit instruction.
************************/
void op_load_value(UM *um, uint32_t instruction)
{
        assert(um != NULL);
        uint32_t a = ((instruction >> LOAD_VAL_SIZE) & 0x7);
        uint32_t value = Bitpack_getu(instruction, LOAD_VAL_SIZE, 0);
        um->registers[a] = value; 
}