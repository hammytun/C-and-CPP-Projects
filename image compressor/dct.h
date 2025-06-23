/*
*     dct.h
*     By: Brendan Roy (broy02) and Harriston Tun (htun01)
*     Date: March 6, 2025
*     arith
*
*     This module provides uses the discrete cosine transform to quantize the 
*     video component values for an accurate lossy compression. It also 
*     performs the inverse of those operations in order to convert back into 
*     video component.
*/

#ifndef DCT_H
#define DCT_H


#include <stdlib.h>
#include <stdint.h>
#include "pnm.h"


/********** struct Quantized ********
*
* Holds the quantized versions of the luminance and chroma values of the
* 2x2 sub-image as integers
*
* Members:
*       uint32_t a: the average brightness of the subimage
*       int32_t b: the degree to which the subimage gets brighter as we move 
*                  from top to bottom
*       int32_t c: the degree to which the subimage gets brighter as we move 
*                  from left to right
*       int32_t d: the degree to which the pixels on one diagonal are brighter 
*                  than the pixels on the other diagonal.
*       uint32_t pr: average pb value of each pixel in the subimage
*       uint32_t pb: average pr value of each pixel in the subimage
* Notes:
*      It is typedef-ed as a pointer
************************/
typedef struct Quantized {
        uint32_t a;
        int32_t b;
        int32_t c;
        int32_t d;
        uint32_t pr;
        uint32_t pb;
} *Quantized;




void inverseDCT(Pnm_ppm quantImage);
void quantizeVideo(Pnm_ppm image);

#endif