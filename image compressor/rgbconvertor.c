/*
*     rgbconvertor.c
*     By: Brendan Roy (broy02) and Harrison Tun (htun01)
*     Date: March 6th, 2025
*     arith
*
*     This file implements the interface defined in rgbconvertor.h
*/
#include "rgbconvertor.h"
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"
#include "uarray2.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int VIDEO_COMPONENT_SIZE = sizeof(struct VideoComponent);
static const int RGB_SIZE = sizeof(struct Pnm_rgb);
static const int MAX_RGB = 1;
static const int MIN_RGB = 0;


static void toComponentVideo(int col, int row, A2Methods_UArray2 array, 
                                                 void *elem, void *cl);

static void toRGB(int col, int row, A2Methods_UArray2 array, void *elem, 
                                                               void *cl);
static void clampRGBVals(float *r, float *g, float *b);


/*******************************************************************************
 *                              Decompression Functions                        *
 ******************************************************************************/

/********** videoToRGB ********
*
* Converts a video component image to and RGB image. This function maps each
* video component to its corresponding RGB value and replaces the original pixel
* data with the new RGB pixel data
*
* Parameters:
*      Pnm_ppm image - a pointer to PPM image
*
* Return: void 
*
* Expects
*      The image is not NULL
*      image->pixel is not NULL
*      image->methods is not NULL
* Notes:
*      - a new RGB image array is allocated 
*      - The original pixel data is freed and replaced with the new RGB image
*      - Will CRE if image, image->pixels, or image->methods is NULL
************************/
void videoToRgb(Pnm_ppm image)
{
        assert(image != NULL);
        assert(image->pixels != NULL);
        assert(image->methods != NULL);

        A2Methods_UArray2 rgbImage = image->methods->new(image->width,
                                                         image->height, 
                                                         RGB_SIZE);

        image->methods->map_row_major(rgbImage, toRGB, image);

        image->methods->free(&(image->pixels));
        image->pixels = rgbImage;
}

/********** toRGB ********
*
* Converts a single pixel's video component values into a RGB pixel. 
*
* Parameters:
*      int col - col index
*      int row - row index
*      UArray2_T array - the array being traversed
*      void *elem - a pointer to the element in the RGB image array where
*                   converted RGB data will be stored
*      void *cl -   A pointer to the Pnm_ppm image structure that provides 
                    access to the original video component data and the 
*                   image's denominator.
*
* Return: void
*
* Expects
*      'cl' must not be NULL
*      the VideoComponent at the specified index is valid
* Notes:
*     - Retrieves the video component data for the current pixel.
*     - Clamps the RGB values to [0, 1] using 'clampRGBVals'.
*     - Scales the clamped values by the image's denominator and rounds them
*     - CRE thrown for assertion failures
************************/
void toRGB(int col, int row, A2Methods_UArray2 array, void *elem, void *cl)
{
        (void)array;
        (void)elem;

        Pnm_ppm image = (Pnm_ppm)cl;
        VideoComponent currVC = (VideoComponent)(image->methods->at(
                                                 image->pixels, col, row));
        float y = currVC->y;
        float pr = currVC->pr;
        float pb = currVC->pb;

        /* Use constants as given in the spec */
        float r = y + (1.402 * pr);
        float g = y - (0.344136 * pb) - (0.714136 * pr);
        float b = y + (1.772 * pb);

        clampRGBVals(&r, &g, &b);
       
        /* Round to the nearest integer; don't always round down like C does */
        unsigned roundR = round(r * (float)image->denominator);
        unsigned roundG = round(g * (float)image->denominator);
        unsigned roundB = round(b * (float)image->denominator);
        
        Pnm_rgb pix = (Pnm_rgb)elem;

        pix->red = roundR;
        pix->green = roundG;
        pix->blue = roundB;
}
/********** clampRGBVals ********
*
* Helper function to clampn the RGB values so that they fall within the range
* 0 - 1
*
* Parameters:
*      float *r - pointer to red component value
*      float *g - pointer to green component value
*      float *b - pointer to blue component value
*
* Return: void
*
* Expects
*      r, g, b must not be NULL
* Notes:
*      Values that are less than 0 are set to 0 and values that are greater 
*      than 1 are set to 1
*      Will CRE if r, g, or b, are NULL
************************/
void clampRGBVals(float *r, float *g, float *b)
{
        assert(r != NULL);
        assert(b != NULL);
        assert(b != NULL);

        if (*r < MIN_RGB) {
                *r = MIN_RGB;
        }
        if (*r > MAX_RGB) {
                *r = MAX_RGB;
        } 
        if (*g < MIN_RGB) {
                *g = MIN_RGB;
        } 
        if (*g > MAX_RGB) {
                *g = MAX_RGB;
        }        
        if (*b < MIN_RGB) {
                *b = MIN_RGB; 
        }
        if (*b > MAX_RGB) {
                *b = MAX_RGB;
        }   
}

/*******************************************************************************
 *                              Compression Functions                          *
 ******************************************************************************/

/********** rbgToVideo ********
*
*  Converts an RGB image into a video component image by mapping each RGB pixel
*  to its corresponding video component (Y, Pb, Pr) values.
*
* Parameters:
*      Pnm_ppm image - a pointer a PPM image
*
* Return: void 
*
* Expects
*      image must not be NULL
*      image->pixels must not be NULL
*      image->methods must not be NULL
* Notes:
*      CRE for any assertion failures
*      A videoImage is created to hold the converted VideoComponent values
*      The original RGB image is freed and replaced by the videoImage
************************/
void rgbToVideo(Pnm_ppm image)
{
        assert(image != NULL);
        assert(image->pixels != NULL);
        assert(image->methods != NULL);

        UArray2_T videoImage = UArray2_new(image->width, image->height, 
                                           VIDEO_COMPONENT_SIZE);
        
        image->methods->map_row_major(videoImage, toComponentVideo, image);

        image->methods->free(&(image->pixels));
        image->pixels = videoImage;
}
/********** toComponentVideo ********
*
* Converts a single RGB pixel to its video component representation
*
* Parameters:
*      int col - col index
*      int row - row index
*      UArray2_T array - the array being traversed
*      void *elem - a pointer to the element in the VideoComponent array
*      void *cl -   A pointer to the Pnm_ppm image structure that provides 
                    access to the source's RGB pixel data
*
* Return: void
*
* Expects
*      'cl' must be NULL
*      'elem' must be NULL
* Notes:
*      Will CRE if cl or elem are NULL
*      Conversion formulas:
*          y  = 0.299 * r + 0.587 * g + 0.114 * b
*          pb = -0.168736 * r - 0.331264 * g + 0.5 * b
*          pr = 0.5 * r - 0.418688 * g - 0.081312 * b
************************/
void toComponentVideo(int col, int row, A2Methods_UArray2 array, void *elem,
                      void *cl)
{
        assert(cl != NULL);
        assert(elem != NULL);
        (void)array;
        
        Pnm_ppm image = (Pnm_ppm)cl;
        Pnm_rgb currPixel = (Pnm_rgb)image->methods->at(image->pixels, 
                                                        col, row);
        unsigned denominator = image->denominator;

        /* Scale each value by the denominator of the image */
        float r = currPixel->red / (float)denominator;
        float g = currPixel->green / (float)denominator;
        float b = currPixel->blue / (float)denominator;


        VideoComponent vc = (VideoComponent)elem;

        /* Use constants as given in the spec */
        vc->y = 0.299 * r + 0.587 * g + 0.114 * b;
        vc->pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        vc->pr = 0.5 * r - 0.418688 * g - 0.081312 * b;
}