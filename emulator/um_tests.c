/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc) {
	Um_instruction inst = 0; 
    
	inst = Bitpack_newu(inst, 4, 28, op);    
	inst = Bitpack_newu(inst, 3, 6, ra);
	inst = Bitpack_newu(inst, 3, 3, rb);
	inst = Bitpack_newu(inst, 3, 0, rc);
    
	return inst;
}

Um_instruction loadval(unsigned ra, unsigned val) {
	Um_instruction inst = 0;  

	inst = Bitpack_newu(inst, 4, 28, LV); 
	inst = Bitpack_newu(inst, 3, 25, ra);
	inst = Bitpack_newu(inst, 25, 0, val);
	return inst;
}


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

Um_instruction output(Um_register c) {
	return three_register(OUT, 0, 0, c);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_add_test(Seq_T stream) 
{
        append(stream, add(r1, r2, r3));
        append(stream, halt());
}

void build_digit_test(Seq_T stream)
{
	append(stream, loadval(r1, 48));
	append(stream, loadval(r2, 6));
	append(stream, add(r3, r1, r2));
	append(stream, output(r3));
	append(stream, halt());
}

void build_output_halt_test(Seq_T stream)
{
	append(stream, loadval(r1, 72)); // ASCII 'H'
	append(stream, output(r1));
	append(stream, halt());
}

void build_loadval_output_halt_test(Seq_T stream)
{
	append(stream, loadval(r3, 88)); // ASCII 'X'
	append(stream, output(r3));
	append(stream, halt());
}

void build_add_test_full(Seq_T stream)
{
	append(stream, loadval(r1, 5));
	append(stream, loadval(r2, 3));
	append(stream, add(r0, r1, r2)); // r0 = 8
	append(stream, add(r0, r0, r1)); // r0 = 13
	append(stream, loadval(r3, 48));
	append(stream, add(r0, r3, r0)); // ASCII offset
	append(stream, output(r0)); // should print '='
	append(stream, halt());
}

void build_mul_test(Seq_T stream)
{
	append(stream, loadval(r1, 6));
	append(stream, loadval(r2, 7));
	append(stream, three_register(MUL, r0, r1, r2)); // r0 = 42
	append(stream, output(r0)); // '*' (90)
	append(stream, halt());
}

void build_div_test(Seq_T stream)
{
	append(stream, loadval(r1, 56));
	append(stream, loadval(r2, 8));
	append(stream, three_register(DIV, r0, r1, r2)); // r0 = 7
	append(stream, loadval(r3, 48));
	append(stream, add(r0, r3, r0));
	append(stream, output(r0)); // '7'
	append(stream, halt());
}

void build_nand_test(Seq_T stream)
{
	append(stream, loadval(r1, 0xF0F0));
	append(stream, loadval(r2, 0x0F0F));
	append(stream, three_register(NAND, r0, r1, r2));
	append(stream, halt());
}

void build_input_output_test(Seq_T stream)
{
	append(stream, three_register(IN, 0, 0, r1)); // input into r1
	append(stream, output(r1)); // echo it
	append(stream, halt());
}

void build_cmov_zero_test(Seq_T stream)
{
	append(stream, loadval(r0, 65)); // A
	append(stream, loadval(r1, 66)); // B
	append(stream, loadval(r2, 0));  // condition = 0
	append(stream, three_register(CMOV, r0, r1, r2)); // should NOT move
	append(stream, output(r0)); // should print 'A'
	append(stream, halt());
}

void build_cmov_nonzero_test(Seq_T stream)
{
	append(stream, loadval(r0, 65)); // A
	append(stream, loadval(r1, 66)); // B
	append(stream, loadval(r2, 1));  // condition = 1
	append(stream, three_register(CMOV, r0, r1, r2)); // should move
	append(stream, output(r0)); // should print 'B'
	append(stream, halt());
}

void build_map_unmap_test(Seq_T stream)
{
	append(stream, loadval(r1, 10));              // r1 = segment size
	append(stream, three_register(ACTIVATE, r2, r3, r1)); 
	append(stream, three_register(INACTIVATE, r2, r1, r3)); 
	append(stream, halt());
}

void build_store_load_test(Seq_T stream)
{
	append(stream, loadval(r1, 1));              // segment size = 1
	append(stream, three_register(ACTIVATE, 0, r2, r1)); 

	append(stream, loadval(r3, 88));             // ASCII 'X'
	append(stream, loadval(r4, 0));              // offset 0
	append(stream, three_register(SSTORE, r2, r4, r3));  

	append(stream, three_register(SLOAD, r5, r2, r4));   
	append(stream, output(r5));                   // should output 'X'
	append(stream, halt());
}

void build_loadprog_test(Seq_T stream)
{
        append(stream, loadval(r1, 6));
    	append(stream, loadval(r2, 7));
    	append(stream, three_register(MUL, r0, r1, r2)); // r0 = 42
    	append(stream, output(r0)); // '*' (90)

	append(stream, three_register(ACTIVATE, 0, r3, r2)); // r3 now holds seg
	append(stream, three_register(SLOAD, r5, r6, r7));
	append(stream, three_register(SSTORE, r3, r7, r5));

	append(stream, loadval(r7, 1));
	append(stream, three_register(SLOAD, r5, r6, r7));
	append(stream, three_register(SSTORE, r3, r7, r5));

	append(stream, loadval(r7, 2));
	append(stream, three_register(SLOAD, r5, r6, r7));
	append(stream, three_register(SSTORE, r3, r7, r5));

	append(stream, loadval(r7, 3));
	append(stream, three_register(SLOAD, r5, r6, r7));
	append(stream, three_register(SSTORE, r3, r7, r5));

	append(stream, loadval(r7, 21));
	append(stream, loadval(r4, 4));
	append(stream, three_register(SLOAD, r5, r6, r7));
	append(stream, three_register(SSTORE, r3, r4, r5));

	append(stream, three_register(LOADP, r1, r3, r6));

	append(stream, halt());
}