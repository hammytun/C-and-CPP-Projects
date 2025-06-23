/*
*     filehandler.h
*     By: Brendan Roy (broy02) and Harrison Tun (htun01)
*     Date: March 6th, 2025
*     arith
*
*     This module handles file I/O operations related to compressed and 
*     decompressed PPM images. It writes the compressed version in a series of
*     custom 'codewords' and can then be used to read said compressed file and
*     interpret the codewords.
*     
*/
#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#include "pnm.h"
#include "a2methods.h"

void writeCompressedImage(Pnm_ppm image);
Pnm_ppm readCompressedImage(FILE *input, A2Methods_T methods);

#endif