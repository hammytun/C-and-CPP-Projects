/*
*     rgbconvertor.h
*     By: Brendan Roy (broy02) and Harrison Tun (htun01)
*     Date: March 6th, 2025
*     arith
*
*     The rgbconvertor is used for converting an image's pixels from RGB to
*     video component representation and back again. This allows us to extract
*     valuable information from the RGB values into a more usable form.
*/

#ifndef RGBCONVERTOR_H
#define RGBCONVERTOR_H

#include "pnm.h"


/********** struct VideoComponent ********
*
* This struct holds the video component values that correspond to the red, green
* and blue values of a normal r, g, b pixel. 
*
* Members:
*      float y - the luminance of the pixel - always between 0 and 1
*      float pr - this is proportional to the color difference between r - y
                   always between -0.5 and 0.5
*      float pb - this is proportional to the color difference between r - b
                   always between -0.5 and 0.5
* Notes:
*       It is typedef-ed as a pointer
************************/
typedef struct VideoComponent {
        float y;
        float pr;
        float pb;
} *VideoComponent;

void rgbToVideo(Pnm_ppm image);
void videoToRgb(Pnm_ppm videoImage);

#endif
