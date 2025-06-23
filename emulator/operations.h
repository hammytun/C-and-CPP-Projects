/**************************************************************
 *
 *                     operations.h
 *
 *     	Assignment: um
 *     	Authors:  Harrison Tun (htun01), Arman Kassam (akassa01)
 *     	Date:     4/9/25
 * 
 *	operations.c emulates an ALU, carrying out 13 main instructions. these
 *	range from simple adding to loading a new program.
 *
 **************************************************************/


#include "um.h"
#include "memory.h"
#include "bitpack.h"

void op_conditional_move(UM *um, uint32_t instruction);
void op_segmented_load(UM *um, uint32_t instruction);
void op_segmented_store(UM *um, uint32_t instruction);
void op_add(UM *um, uint32_t instruction);
void op_multiply(UM *um, uint32_t instruction);
void op_divide(UM *um, uint32_t instruction);
void op_bitwise_nand(UM *um, uint32_t instruction);
void op_map(UM *um, uint32_t instruction);
void op_unmap(UM *um, uint32_t instruction);
void op_output(UM *um, uint32_t instruction);
void op_input(UM *um, uint32_t instruction);
void op_load_program(UM *um, uint32_t instruction);
void op_load_value(UM *um, uint32_t instruction);