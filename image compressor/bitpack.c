/*
*     bitpack.c
*     By: Brendan Roy (broy02) and Harrison Tun (htun01)
*     Date: March 6th, 2025
*     arith
*
*     bitpack provides several functionalities regarding how bits are stored in
*     a 64-bit word. It contains functions for testing if values fit within a 
*     given number of bits, as well as for extracting and inserting both signed
*     and unsigned values into a 64-bit uint64_t word. 
*/
#include "bitpack.h"
#include "assert.h"


Except_T Bitpack_Overflow = { "Overflow packing bits" };


static uint64_t left_shift(uint64_t value, unsigned shift);
static uint64_t right_shiftu(uint64_t value, unsigned shift);


/********** left_shift ********
*
* Shifts an unsigned 64 bit value left by a given number of bits
*
* Parameters:
*      uint64_t value - value to be shifted
*      unsigned shift - shift amount
*
* Return: a uint64_t representing the new value shifted left by a given number 
*         of bits
*
* Expects
*      'shift' must be <= 64
* Notes:
*      CRE will be thrown is the shift is not <= 64
*      If the shift amount is 64, the function will return 0
************************/
uint64_t left_shift(uint64_t value, unsigned shift) 
{
        assert(shift <= 64);
        if (shift == 64) {
                return (uint64_t)0;
        }
        return value << shift;
}

/********** right_shiftu ********
*
* Shifts an unsigned 64 bit value right by a given number of bits
*
* Parameters:
*      uint64_t value - value to be shifted
*      unsigned shift - shift amount
*
* Return: a uint64_t representing the new value shifted right by a given number 
*         of bits
*
* Expects
*      'shift' must be <= 64
* Notes:
*      CRE will be thrown is the shift is not <= 64
*      If the shift amount is 64, the function will return 0
************************/
uint64_t right_shiftu(uint64_t value, unsigned shift)
{
        assert(shift <= 64);
        if (shift == 64) {
                return (uint64_t)0;
        }        
        return value >> shift;
}
/********** Bitpack_fitsu ********
*
* Checks if an unsigned integer can be represented within a given bit width
*
* Parameters:
*      uint64_t n - the unsigned integer to check
*      unsigned width - the number of bits available. Must be <= 64
*
* Return: True if n fits and false otherwise
*
* Expects
*      'width' must be <= 64 and if the width is 0 it returns false
* Notes:
*      The check compares 'n' to 2^width - 1
*      Will throw a CRE if the width is greater than or equal to 64 
************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);
        if (width == 0) {
                return false;
        }
        return (n <= left_shift(1, width) - 1);
}
/********** Bitpack_fitss ********
*
* Checks if a signed integer can be represented within a given bit width
*
* Parameters:
*      int64_t n - the signed integer to check
*      unsigned width - the number of bits available. Must be <= 64
*
* Return: True if n fits and false otehrwise
*
* Expects
*      'width' must be <= 64 and if the width is 0 it returns false
* Notes:
*      a valid range is from -2^width - 1 and 2^(width-1) -1 
************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= 64);
        if (width == 0) {
                return false;
        }

        /* Check the number n fits in both the positive and negative bounds */
        bool negativeBound = (n >= -(int64_t)(left_shift(1, width - 1)));
        bool positiveBound = (n <= (int64_t)left_shift(1, width - 1) - 1);

        return negativeBound && positiveBound;
}
/********** Bitpack_getu ********
*
* Extracts an unsigned value from a word using the specified bit field
*
* Parameters:
*      uint64_t word - word from which to extract bits
*      unsigned width - the number of bits to extract
*      unsigned lsb - the least significant bit in the bit field
*
* Return: uint64_t representing the extracted unsigned value
*
* Expects
*      'width' <= 64
*      'width + lsb' <= 64 
* Notes:
*      Will throw CRE if width is <= 64 or if (width + lsb <= 64)
*      a mask is computed and applied after shifting word
************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{       
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (width == 0) {
                return 0;
        }
        /* Shift the word by the lsb and use a mask to return the value there */
        word = right_shiftu(word, lsb);

        uint64_t mask = ~(uint64_t)0;
        mask = right_shiftu(mask, 64 - width);

        return word & mask;
}
/********** Bitpack_gets ********
*
* Extracts an signed value from a word using the specified bit field
*
* Parameters:
*      uint64_t word - word from which to extract bits
*      unsigned width - the number of bits to extract
*      unsigned lsb - the least significant bit in the bit field
*
* Return: int64_t representing the extracted signed value
*
* Expects
*      'width' <= 64
*      'width + lsb' <= 64 
* Notes:
*      Will throw CRE if width is <= 64 or if (width + lsb <= 64)
*      The unsigned value is extracted first then adjusted for sign
************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (width == 0) {
                return 0;
        }
        
        uint64_t unsignedVal = Bitpack_getu(word, width, lsb);
        uint64_t mask = right_shiftu(~(uint64_t)0, 64 - width);

        /*
         * If the value can be represented as an unsigned int (ie is positive),
         * then return it as is. Otherwise, return it as a negative number 
         */
        return (Bitpack_fitss(unsignedVal, width)) ? unsignedVal 
                                                   : ~(~unsignedVal & mask);


}
/********** Bitpack_newu ********
*
* Inserts an unsigned value into a word at a specified field
*
* Parameters:
*      uint64_t word - word from which to extract bits
*      unsigned width - the number of bits to extract
*      unsigned lsb - the least significant bit in the bit field
*      uint64_t value - unsigned value to be inserted into word
*
* Return: uint64_t representing the new word with the value inserted
*
* Expects
*      'width' <= 64 
*      (width + lsb) <= 64
*      'value' must fit in width bits  
* Notes:
*      Will throw a CRE is width <= 64 and (width + lsb) <= 64
*      Raises a 'Bitpack_Overflow' exception if the value does not fit
*      Clears the target field and inserts the new value
*      
************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t mask = ~(uint64_t)0;

        mask = right_shiftu(mask, (64 - width));
        mask = left_shift(mask, lsb);

        /*
        *  Clear the value field of the word, and everything but the value field
        *  in the value
        */
        uint64_t clearedField = word & ~mask;
        uint64_t clearedValue = left_shift(value, lsb) & mask;

        return clearedField | clearedValue;
}
/********** Bitpack_news ********
*
* Inserts an signed value into a word at a specified field
*
* Parameters:
*      uint64_t word - word from which to extract bits
*      unsigned width - the number of bits to extract
*      unsigned lsb - the least significant bit in the bit field
*      int64_t value - signed value to be inserted into word
*
* Return: int64_t representing the new word with the value inserted
*
* Expects
*      'width' <= 64 
*      (width + lsb) <= 64
*      'value' must fit in width bits  
* Notes:
*      Will throw a CRE is width <= 64 and (width + lsb) <= 64
*      Raises a 'Bitpack_Overflow' exception if the value does not fit
*      For nonnegative values, Bitpack_newu is used
*      For negative values, the value is adjusted before insertion
*      
************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                                                    int64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        /* If nonnegative, pass value in as unsigned */
        if (value >= 0) {
                return Bitpack_newu(word, width, lsb, value);
        }

        /* Otherwise, pass it in as its absolute value */
        uint64_t mask = ~(uint64_t)0;
        mask = left_shift(mask, width);
        value &= ~mask;

        return Bitpack_newu(word, width, lsb, value);
}