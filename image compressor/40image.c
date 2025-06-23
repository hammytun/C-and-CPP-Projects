/*
*     40image.c
*     By: Brendan Roy (broy02) and Harrison Tun (htun01)
*     Date: March 6th, 2025
*     arith
*
*     This file implements a lossy ppm image compressor / decompressor. The user
*     can compress any ppm image (Magic Number P3), raw or plain, and decompress
*     it into a raw ppm. The compressed version of the image is about 1/3 the
*     size of the original. 
*/
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"
#include "bitpack.h"
#include "compress40.h"
#include "filehandler.h"
#include "dct.h"
#include "pnm.h"
#include "rgbconvertor.h"
#include "uarray2.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void (*compress_or_decompress)(FILE *input) = compress40;

/********** main ********
*
*  This is the main function for 40image / arith. It handles the command
*  line arguments and calls the appropriate compress / decompress function.
*
* Parameters:
*      int argc     - the number of arguments on the command line
*      char *argv[] - a pointer to an array of strings that holds each of the
*                     command line arguments
*
* Return: an int: 0 for EXIT_SUCCESS and 1 for EXIT_FAILURE
*
* Expects
*      Usage is ./40image -[d | c] [inputFilename]
*      User can write -d to denote decompression and -c to denote compression
*      Provided filename should be able to be opened
* Notes:
*       Will return EXIT_FAILURE and terminate program if command line arguments
*       do not align with the defined Usage. i.e. needs to have -d or -c then
*       an openable file. Calls functions that may CRE if input is somehow 
*       invalid
************************/
int main(int argc, char *argv[])
{
        int i;
        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr,
                                "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1); /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS;
}

/********** decompress40 ********
*
* Reads the compressed ppm file from FILE *input and outputs the decompressed
* version to stdout. Note that the compression was lossy, so the original file
* and the output file will not be identical.
*
* Parameters:
*      FILE *input - A pointer to a FILE struct that has the input stream
*                    containing the compressed image
*
* Return: void
*
* Expects: input should not be NULL
*      
* Notes:
*      Will CRE if input is NULL
*      Does not close input. That must be done by the caller
************************/
void decompress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_plain;

        /* Create a Pnm_ppm from the compressed format */
        Pnm_ppm pixmap = readCompressedImage(input, methods);
        assert(pixmap->width % 2 == 0);
        assert(pixmap->height % 2 == 0);


        inverseDCT(pixmap); /* Undo the DCT and quantization */
        videoToRgb(pixmap); /* Convert from video component to RGB values */

        Pnm_ppmwrite(stdout, pixmap);
        
        pixmap->methods->free(&(pixmap->pixels));
        free(pixmap);
}

/********** compress40 ********
*
* Reads in the ppm file from the given input stream and outputs a compressed
* format of it to stdout. The compression is lossy, and the outputted text is
* about 1/3 of the size of the original image.
*
* Parameters:
*       FILE *input - A pointer to a FILE struct that has the input stream
*                     containing the ppm image
*
* Return: void
*
* Expects
*      input should not be NULL
* Notes:
*      Will CRE if input is NULL
*      Does not close input. That must be done by the caller.
************************/
void compress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_plain;

        /* Read the Pnm_ppm image and trim width/height to even numbers */
        Pnm_ppm image = Pnm_ppmread(input, methods);
        image->width -= image->width % 2;
        image->height -= image->height % 2;

        rgbToVideo(image); /* RGB --> VIDEO */
        quantizeVideo(image); /* DCT + QUANT */
        writeCompressedImage(image); /* Write the image to the stdout */

        Pnm_ppmfree(&image);
}