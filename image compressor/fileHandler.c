/*
*     filehandler.c
*     By: Brendan Roy (broy02) and Harrison Tun (htun01)
*     Date: March 6th, 2025
*     arith
*
*     This file implements the interface defined in filehandler.c
*/
#include "filehandler.h"
#include "assert.h"
#include "bitpack.h"
#include "uarray2.h"
#include "dct.h"
#include <stdio.h>
#include <stdlib.h>

static const int A_WIDTH = 9;
static const int B_WIDTH = 5;
static const int C_WIDTH = 5;
static const int D_WIDTH = 5;
static const int PB_WIDTH = 4;
static const int PR_WIDTH = 4;
static const int A_LSB = 23;
static const int B_LSB = 18;
static const int C_LSB = 13;
static const int D_LSB = 8;
static const int PB_LSB = 4;
static const int PR_LSB = 0;
static const int BYTE_SIZE = 8;
static const int BYTE1 = 0;
static const int BYTE2 = 8;
static const int BYTE3 = 16;
static const int BYTE4 = 24;
static const int DENOMINATOR = 255;
static const int QUANTIZED_SIZE = sizeof(struct Quantized);

static void toCodewordApply(int col, int row, UArray2_T array, void* elem, 
                            void *cl); 
static void printImageApply(int col, int row, UArray2_T array, void* elem, 
                            void *cl); 
static void codewordToQuantApply(int col, int row, UArray2_T array, void* elem, 
                            void *cl);
static void getCodewordsApply(int col, int row, UArray2_T array, void* elem, 
                              void *cl);
static UArray2_T unPackBits(UArray2_T codewords);
static UArray2_T packBits(UArray2_T QuantizedVals);

/*******************************************************************************
 *                              Decompression Functions                        *
 ******************************************************************************/

/********** readCompressedImage ********
*
* This function reads the compressed image and sorts each 4-byte codeword into
* their corresponding values in the 'Quantized' struct
*
* Parameters:
*      FILE *input - a pointer to a FILE struct that holds the compressed ppm
*                    image
*      A2Methods_T methods - the A2 methods that we are using for our Pnm_ppm
*
* Return: a pointer to a Pnm_ppm struct whose height and width are those
*         of the original image and the pixels contains the Quantized values
*
* Expects
*      input should not be NULL
*      methods should not be NULL
* Notes:
*      Will CRE if input is NULL
*      Will CRE if methods is NULL
*      Will CRE if the header of the compressed image is not formatted properly
*      Allocates memory (the Pnm_ppm) that must be freed by the caller, and will
*      CRE if there is no more free memory that can be allocated
************************/
Pnm_ppm readCompressedImage(FILE *input, A2Methods_T methods)
{        
        assert(input != NULL);
        assert(methods != NULL);

        /* Read in the header of the file */
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                                                          &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert((char)c == '\n');
        assert(width % 2 == 0);
        assert(height % 2 == 0);

        /*
        * codewords has half the height and width because it corresponds to 
        * the 2x2 sub-images
        */
        UArray2_T codewords = UArray2_new(width / 2, height / 2, 
                                          sizeof(uint32_t));
        UArray2_map_row_major(codewords, getCodewordsApply, input);

        /* Create the new Pnm_ppm struct to be returned */
        struct Pnm_ppm *pixmap = malloc(sizeof(*pixmap));
        assert(pixmap != NULL);
        pixmap->methods = methods;
        pixmap->denominator = DENOMINATOR;
        pixmap->width = width;
        pixmap->height = height;

        pixmap->pixels = unPackBits(codewords);
        return pixmap;
}
/********** getCodewordsApply ********
*
* An apply function for UArray2 map function. Takes in the file stream as the
* closure and has each element of the UArray2 hold a 4-byte codeword
*
* Parameters:
*      int col - col index
*      int row - row index
*      UArray2_T array - the array being traversed
*      void *elem - a pointer to the a uint32_t that holds each codeword of the
*                   compressed image
*      void *cl -  A pointer to the FILE struct that has the contents of the
*                  compressed image
*
* Return: void
*
* Expects
*      elem should not be NULL
*      cl should not be NULL
*      The FILE stream should have at least as many codewords as specified by
*      the height / width of the UArray2_T that we are mapping
* Notes:
*      Will CRE if elem is NULL
*      Will CRE if cl is NULL
*      Will CRE if the FILE stream hits EOF before all the array's indices have
*      been populated with codewords.
************************/
void getCodewordsApply(int col, int row, UArray2_T array, void* elem, void *cl)
{
        assert(elem != NULL);
        assert(cl != NULL);
        (void)array;
        (void)col;
        (void)row;

        FILE *input = (FILE *)cl;
        int currByte;

        currByte = fgetc(input);
        assert(currByte != EOF);
        uint32_t firstByte = Bitpack_newu(0, BYTE_SIZE, BYTE4, currByte);
        
        currByte = fgetc(input);
        assert(currByte != EOF);
        uint32_t secondByte = Bitpack_newu(firstByte, BYTE_SIZE, BYTE3, 
                                           currByte);
        
        currByte = fgetc(input);
        assert(currByte != EOF);
        uint32_t thirdByte = Bitpack_newu(secondByte, BYTE_SIZE, BYTE2, 
                                          currByte);
        
        currByte = fgetc(input);
        assert(currByte != EOF);
        uint32_t lastByte = Bitpack_newu(thirdByte, BYTE_SIZE, BYTE1, currByte);

        *(uint32_t *)elem = lastByte;
}
/********** unPackBits ********
*
* Take a UArray2_T filled with codewords and return a new one filled with 
* the Quantized values that were hidden in the codewords
*
* Parameters:
*      UArray2_T codewords - a reference to a UArray2_T that holds each codeword
*                            of the image
*
* Return: A reference to a UArray2_T of struct Quantized's that hold all the
*         Quantized values for their respective pixels in the image
*
* Expects
*      codewords should not be NULL
* Notes:
*      Will CRE if codewords is NULL
*      Frees memory associated with codewords
*      Allocates new memory for the returned UArray2_T that the caller must free
*      Will CRE if no more memory can be allocated
************************/
UArray2_T unPackBits(UArray2_T codewords) 
{
        assert(codewords != NULL);

        int height = UArray2_height(codewords);
        int width = UArray2_width(codewords);

        UArray2_T unpackedVals = UArray2_new(width, height, QUANTIZED_SIZE);
        UArray2_map_row_major(codewords, codewordToQuantApply, unpackedVals);
        
        UArray2_free(&codewords);
        return unpackedVals;
}

/********** codewordToQuantApply ********
*
* An apply function for UArray2 map function. Takes in the file stream as the
* closure and has each element of the UArray2 hold a 4-byte codeword
*
* Parameters:
*      int col - col index
*      int row - row index
*      UArray2_T array - the array being traversed
*      void *elem - a pointer to the a uint32_t that holds each codeword of the
*                   compressed image
*      void *cl -  A pointer to the FILE struct that has the contents of the
*                  compressed image
*
* Return: void
*
* Expects
*      elem should not be NULL
*      cl should not be NULL
*      The FILE stream should have at least as many codewords as specified by
*      the height / width of the UArray2_T that we are mapping
* Notes:
*      Will CRE if elem is NULL
*      Will CRE if cl is NULL
*      It is a URE for cl not to point to a UArray2_T of struct Quantized
*      It is a URE for elem not to point to a uint32_t
************************/
void codewordToQuantApply(int col, int row, UArray2_T array, void* elem, 
                          void *cl)
{
        assert(elem != NULL);
        assert(cl != NULL);
        (void)array;

        UArray2_T unpackedBits = (UArray2_T)cl;
        Quantized currQuant = (Quantized)UArray2_at(unpackedBits, col, row);
        uint32_t codeword = *(uint32_t *)elem;

        /* Unpack each codeword; assign contents to respective Quantized vals*/
        currQuant->a = Bitpack_getu(codeword, A_WIDTH, A_LSB);
        currQuant->b = Bitpack_gets(codeword, B_WIDTH, B_LSB);
        currQuant->c = Bitpack_gets(codeword, C_WIDTH, C_LSB);
        currQuant->d = Bitpack_gets(codeword, D_WIDTH, D_LSB);
        currQuant->pb = Bitpack_getu(codeword, PB_WIDTH, PB_LSB);
        currQuant->pr = Bitpack_getu(codeword, PR_WIDTH, PR_LSB);
}

/*******************************************************************************
 *                              Compression Functions                          *
 ******************************************************************************/

/********** writeCompressedImage ********
*
* This packs the Quantized values into codewords and outputs the final
* compressed image to stdout. 
*
* Parameters:
*      Pnm_ppm image - a reference to a Pnm_ppm struct that holds all the
*                      image's metadata and Quantized values as its 'pixels'.
*
* Return: void
*
* Expects
*      image should not be NULL
*      image->pixels is not NULL
*      image->methods is not NULL
* Notes:
*      Will CRE if image is NULL
*      Will CRE if image->pixels is NULL
*      Will CRE if image->methods is NULL
*      Allocates and frees memory - will CRE if memory cannot be allocated
*      Modifies the image->pixels to hold the codewords, not Quantized values
************************/
void writeCompressedImage(Pnm_ppm image) 
{
        assert(image != NULL);
        assert(image->pixels != NULL);
        assert(image->methods != NULL);

        UArray2_T codewords = packBits(image->pixels);

        /* Print the header to stdout */
        printf("COMP40 Compressed image format 2\n%u %u\n", image->width, 
                                                            image->height);

        UArray2_map_row_major(codewords, printImageApply, NULL);
        UArray2_free(&codewords);
}

/********** printImageApply ********
*
* Apply function for UArray2_T map function that outputs the codewords stored in
* the mapped UArray2_T to stdout in big-endian order
*
* Parameters:
*      int col - col index
*      int row - row index
*      UArray2_T array - the array being traversed
*      void *elem - a pointer to the a uint32_t that holds each codeword of the
*                   compressed image
*      void *cl -  A pointer to void: points to nothing
*
* Return: void
*
* Expects
*      elem should not be NULL
* Notes:
*      Will CRE if elem is NULL
*      It is a URE for elem not to point to a uint32_t
*      Prints the contents of each codeword in big-endian order to stdout
************************/
void printImageApply(int col, int row, UArray2_T array, void* elem, void *cl)
{
        (void)cl;
        (void)array;
        (void)col;
        (void)row;

        /* Prints the codewords to stdout to big endian order */
        uint32_t codeword = *(uint32_t *)elem;
        putchar((uint32_t)Bitpack_getu(codeword, BYTE_SIZE, BYTE4));
        putchar((uint32_t)Bitpack_getu(codeword, BYTE_SIZE, BYTE3));
        putchar((uint32_t)Bitpack_getu(codeword, BYTE_SIZE, BYTE2));
        putchar((uint32_t)Bitpack_getu(codeword, BYTE_SIZE, BYTE1));
}
/********** packBits ********
*
* Takes a UArray2_T of struct Quantized and returns a UArray2_T of uint32_ts
* holding the corresponding codewords
*
* Parameters:
*      UArray2_T QuantizedVals - a reference to a UArray2_T struct that holds
*                                all of the Quantized values (as Quantized 
                                 structs) of the image
*
* Return: a reference to a UArray2_T struct of uint32_ts holding the codewords
*
* Expects
*      QuantizedVals should not be NULL
* Notes:
*      Will CRE if QuantizedVals is NULL
*      Frees all memory associated with QuantizedVals
*      Allocates new memory for the UArray2_T that is returned that the caller
*       must free - holds the codewords
*      Will CRE if memory cannot be allocated
************************/
UArray2_T packBits(UArray2_T QuantizedVals)
{
        assert(QuantizedVals != NULL);

        int height = UArray2_height(QuantizedVals);
        int width = UArray2_width(QuantizedVals);

        UArray2_T packedBits = UArray2_new(width, height, sizeof(uint32_t));
        UArray2_map_row_major(QuantizedVals, toCodewordApply, packedBits);

        return packedBits;
}
/********** toCodewordApply ********
*
* Apply function for the UArray2_T map function. Takes each Quantized elemenet
* and generates the corresponding codeword for it and puts that into the other
* UArray2_T that is passed in through the closure
*
* Parameters:
*      int col - col index
*      int row - row index
*      UArray2_T array - the array being traversed
*      void *elem - a pointer to current Quantized element
*      void *cl -  a pointer to a UArray2_T of uint32_ts that holds all the 
*                  codewords
*
* Return: void
*
* Expects
*      elem should not be NULL
*      cl should not be NULL
* Notes:
*      Will CRE if elem is NULL
*      Will CRE if cl is NULL
*      It is a URE for cl not to point to a UArray2_T of uint32_ts
*      It is a URE for elem not to point to a Quantized struct
************************/
void toCodewordApply(int col, int row, UArray2_T array, void* elem, void *cl)
{
        assert(elem != NULL);
        assert(cl != NULL);
        (void)array;
        
        UArray2_T packedBits = (UArray2_T)cl;
        uint32_t codeword = 0;
        Quantized currQuant = (Quantized)elem; 
        
        /* Put each Quantized value into correct spot in the codeword */
        codeword = Bitpack_newu(codeword, A_WIDTH, A_LSB, currQuant->a);
        codeword = Bitpack_news(codeword, B_WIDTH, B_LSB, currQuant->b);
        codeword = Bitpack_news(codeword, C_WIDTH, C_LSB, currQuant->c);
        codeword = Bitpack_news(codeword, D_WIDTH, D_LSB, currQuant->d);
        codeword = Bitpack_newu(codeword, PB_WIDTH, PB_LSB, currQuant->pb);
        codeword = Bitpack_newu(codeword, PR_WIDTH, PR_LSB, currQuant->pr);

        *(uint32_t *)UArray2_at(packedBits, col, row) = codeword;
}

