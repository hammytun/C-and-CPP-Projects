/*
*     dct.c
*     By: Brendan Roy (broy02) and Harrison Tun (htun01)
*     Date: March 6th, 2025
*     arith
*
*     This file implements the interface defined in dct.h
*/
#include "dct.h"
#include "a2methods.h"
#include <math.h>
#include "a2plain.h"
#include "assert.h"
#include "rgbconvertor.h"
#include "uarray2.h"
#include <stdio.h>
static const int QUANTIZED_SIZE = sizeof(struct Quantized);
static const float MAX_A_SIZE = 511.0;
static const float SCALE_B_C_D = 50.0;
static const float BOX_SIZE = 4.0;
static const float MIN_VAL = -0.3;
static const float MAX_VAL = 0.3;
static const unsigned Y1 = 1;
static const unsigned Y2 = 2;
static const unsigned Y3 = 3;
static const unsigned Y4 = 4;


/********** struct Coefficients ********
*
* This struct has the unrounded coefficients of luminance and chroma values of 
* the 2x2 sub-image found as a result of the discrete cosine transform.  
*
* Members:
*       float a: the average brightness of the subimage
*       float b: the degree to which the subimage gets brighter as we move from
*                top to bottom
*       float c: the degree to which the subimage gets brighter as we move from 
*                left to right
*       float d: the degree to which the pixels on one diagonal are brighter 
*                than the pixels on the other diagonal.
*       float pb: average pb value of each pixel in the subimage
*       float pr: average pr value of each pixel in the subimage
* Notes:
*      It is typedef-ed as a pointer
************************/
typedef struct Coefficients {
        float a;
        float b;
        float c;
        float d;
        float pb;
        float pr;
} *Coefficients;
const int COEFFICIENTS_SIZE = (sizeof(struct Coefficients));

static void checkCoefficients(Coefficients currElem);
static void toQuant(int col, int row, UArray2_T array, void *elem, void *cl);
static void toTwobyTwo(int col, int row, UArray2_T array, void *elem, 
                          void *cl);
static void quantToVideoApply(int col, int row, UArray2_T array, void* elem, 
                              void *cl);
extern unsigned Arith40_index_of_chroma(float x);
extern float Arith40_chroma_of_index(unsigned n);


/*******************************************************************************
 *                              Decompression Functions                        *
 ******************************************************************************/
 
/********** inverseDCT ********
*
* Reverses the Quantized transformation applied to a PPM image. Performs an 
* inverse Discrete Cosine Transform to restore the image's video component data
*
* Parameters:
*      Pnm_ppm image - A PPM image pointer that contains the image pixel data
* 
*
* Return: void
*
* Expects:
*      the image pointer must not be NULL
*      image->pixels must not be NULL
*      image->methods must not be NULL
*      The image structure must have valid width and height
* Notes:
*      Will CRE if image, image->pixels, or image->methods is NULL
*      The image->pixels will be updated to hold the video component values
************************/
void inverseDCT(Pnm_ppm image) 
{
        assert(image != NULL);
        assert(image->pixels != NULL);
        assert(image->methods != NULL);

        UArray2_T luminImage = UArray2_new(image->width, image->height, 
                                           sizeof(struct VideoComponent));
        UArray2_map_row_major(luminImage, quantToVideoApply, image);
        
        image->methods->free(&(image->pixels));
        image->pixels = luminImage;
}
/********** quantToVideoApply ********
*
*    Converts Quantized image Coefficients into video component values for a 
*    specific pixel. This function is used during the inverse transformation, 
*    where Quantized Coefficients are scaled back to their original 
*    (or near-original) video component values.
*
* Parameters:
*      int col - col index
*      int row - row index
*      UArray2_T array - the array being traversed
*      void *elem - a pointer to the video component that is updated with the
*                   inverse-transformed values
*      void *cl -  A pointer to the Pnm_ppm image structure containing the 
*                  original Quantized pixel data.
*
* Return: void
*
* Expects
*       'elem' must be a valid pointer to a VideoComponent struct
*       'cl' must be a valid pointer to a Pnm_Ppm image
* Notes:
*      - Chrominance components (pr and pb) are converted using 
*        Arith40_chroma_of_index.
*      - Will CRE if elem or cl is NULL
************************/
void quantToVideoApply(int col, int row, UArray2_T array, void* elem, void *cl)
{
        assert(elem != NULL);
        assert(cl != NULL);
        (void)array;

        VideoComponent currCoef = (VideoComponent)elem;
        Pnm_ppm image = (Pnm_ppm)cl;
        
        int vidCol = col / 2;
        int vidRow = row / 2;
        Quantized currQuants = (Quantized)(image->methods->at(image->pixels, 
                                                              vidCol, vidRow));
        
        /* Scale back each Quantized so that it is in the correct range */
        float a = (float)currQuants->a / MAX_A_SIZE;
        float b = (float)currQuants->b / SCALE_B_C_D;
        float c = (float)currQuants->c / SCALE_B_C_D;
        float d = (float)currQuants->d / SCALE_B_C_D;

        /* 
        * Compute which y number we are at; each 2x2 has of course 4 y values,
        * with y1 being top left, y2 top right, y3 bottom left, and y4 bottom 
        * right. Odd column indices thus add 1 (even add 0) while odd row
        * indices add 3 (even add 1).
        */
        unsigned yNum = col % 2 + ((row % 2 == 0) ? 1 : 3);

        /* Add or subtract the quantized values depending on y number */
        currCoef->y += a;
        currCoef->y += (yNum == Y3 || yNum == Y4) ? b : -b;
        currCoef->y += (yNum == Y2 || yNum == Y4) ? c : -c;
        currCoef->y += (yNum == Y1 || yNum == Y4) ? d : -d;

        currCoef->pr += Arith40_chroma_of_index(currQuants->pr);
        currCoef->pb += Arith40_chroma_of_index(currQuants->pb);
}


/*******************************************************************************
 *                              Compression Functions                          *
 ******************************************************************************/

/********** quantizeVideo ********
*
*   Converts a video image's pixel data into a Quantized representation by 
*   processing 2x2 blocks of pixels. It computes average coefficient values 
*   for each block and then converts these Coefficients to Quantized form.
*
* Parameters:
*      Pnm_ppm videoImage - A PPM image pointer that contains the image pixel 
*                           data
*
* Return: void
*
* Expects
*      the videoImage pointer must not be NULL
*      videoImage->pixels must not be NULL
*      videoImage->methods must not be NULL
*      The image structure must have valid width and height
* Notes:
*      CRE for and failed exceptions
*      boxes is made to store the images Quantized coefficent 2x2 block data
*      After processing the image, the original videoImage->pixels data is freed
*      and replaced with the Quantized image
************************/
void quantizeVideo(Pnm_ppm videoImage)
{
        assert(videoImage != NULL);
        assert(videoImage->pixels != NULL);
        assert(videoImage->methods != NULL); 

        int origWidth = videoImage->width;
        int origHeight = videoImage->height;
        UArray2_T boxes = UArray2_new(origWidth / 2, origHeight / 2, 
                                      (COEFFICIENTS_SIZE));


        UArray2_map_row_major(videoImage->pixels, toTwobyTwo, boxes);

        UArray2_T toQuantImage = UArray2_new(origWidth / 2, origHeight / 2,
                                             QUANTIZED_SIZE);
        UArray2_map_row_major(boxes, toQuant, toQuantImage);
        UArray2_free(&boxes);
        videoImage->methods->free(&(videoImage->pixels));
        videoImage->pixels = toQuantImage;
}
/********** toTwobyTwo ********
*
* Puts Quantized coefficient values from individual pixels into the 
* corresponding 2x2 block
*
* Parameters:
*      int col - col index
*      int row - row index
*      UArray2_T array - the array being traversed
*      void *elem - a pointer to the video component struct containing the 
*                   brightness and chroma values
*      void *cl -  A pointer to a UArray2 where the coefficent blocks will be 
*                   stored
*
* Return: void
*
* Expects
*      elem' must be a valid pointer to a VideoComponent struct
*      'cl' must be a valid pointer to a UArray of coeffcient blocks
* Notes:
*     - The function computes the corresponding 2x2 block indices for the 
*        pixel. 
*     - It calculates an adjusted luminance value and updates the 
*        coefficient block's a, b, c, and d fields based on the pixel's position 
*        within the block.
*     - Chrominance components (pr and pb) are averaged into the coefficient 
*        block.
*     - CRE for any assertion failure
************************/
void toTwobyTwo(int col, int row, UArray2_T array, void *elem, void *cl)
{ 
        assert(elem != NULL);
        assert(cl != NULL);
        (void)array;

        UArray2_T boxes = (UArray2_T)cl;
        VideoComponent curr = ((VideoComponent)elem);

        int boxCol = col / 2;
        int boxRow = row / 2;
        Coefficients boxCurr = (Coefficients)UArray2_at(boxes, boxCol, boxRow);
        /* 
        * Compute which y number we are at; each 2x2 has of course 4 y values,
        * with y1 being top left, y2 top right, y3 bottom left, and y4 bottom 
        * right. Odd column indices thus add 1 (even add 0) while odd row
        * indices add 3 (even add 1).
        */
        unsigned yNum = col % 2 + ((row % 2 == 0) ? 1 : 3);

        /* 
        *  To compute the average of each value, we divide by the size of the
        *  2x2 box, then add or subtract depending on the y number
        */
        float addYVal = curr->y / BOX_SIZE;

        boxCurr->a += addYVal;
        boxCurr->b += (yNum == Y3 || yNum == Y4) ? addYVal : -addYVal;
        boxCurr->c += (yNum == Y2 || yNum == Y4) ? addYVal : -addYVal;
        boxCurr->d += (yNum == Y1 || yNum == Y4) ? addYVal : -addYVal;
        boxCurr->pr += curr->pr / BOX_SIZE;
        boxCurr->pb += curr->pb / BOX_SIZE;
}
/********** toQuant ********
*
* Converts 2x2 block into Quantized form by scaling and rounding its values 
* and mapping the values to their corresponding indices
*
* Parameters:
*      int col - col index
*      int row - row index
*      UArray2_T array - the array being traversed
*      void *elem - a pointer to the a Coeffcients struct
*      void *cl -  A pointer to a UArray2 where all the Quantized blocks will 
*                  be stored
*
* Return: void
*
* Expects
*      - 'elem' must be a valid pointer to a coefficient block.
*      - 'cl' must be a valid pointer to a UArray2 array designated for 
          Quantized data.
* Notes:
*      - Chroma values are converted to indices using 
*        Arith40_index_of_chroma.
*      - Will CRE if elem or cl are NULL
************************/
void toQuant(int col, int row, UArray2_T array, void *elem, void *cl)
{
        assert(elem != NULL);
        assert(cl != NULL);
        (void)array;

        UArray2_T toQuantImage = (UArray2_T)cl;
        Quantized currQuant = (Quantized)((UArray2_at(toQuantImage, col, row)));
        Coefficients currElem = (Coefficients)elem;

        /* Clamp values to account for possible quantization range errors */
        checkCoefficients(currElem);

        /* Scale each value for maximum precision within the given byte size */
        currQuant->a = round(MAX_A_SIZE * currElem->a);
        currQuant->b = round(SCALE_B_C_D * currElem->b);
        currQuant->c = round(SCALE_B_C_D * currElem->c);
        currQuant->d = round(SCALE_B_C_D * currElem->d);

        currQuant->pb = Arith40_index_of_chroma(currElem->pb);
        currQuant->pr = Arith40_index_of_chroma(currElem->pr);
}

/********** checkCoefficients ********
*
* Checks that the coeffcient values b, c, and d are within the ranges -.3 and
* .3 in a  coefficient block. This is to maintain consistency and stability in
* the quantization process
*
* Parameters:
*      Coefficients currElem - a pointer to a Foeffecients struct
*
* Return: void
*
* Expects
*      'currElem' must not be NULL
* Notes:
*     - CRE if currElem is NULL
************************/
void checkCoefficients(Coefficients currElem)
{
        assert(currElem != NULL);
        
        if (currElem->b < MIN_VAL) {
                currElem->b = MIN_VAL;
        }
        if (currElem->c < MIN_VAL) {
                currElem->c = MIN_VAL;
        }
        if (currElem->d < MIN_VAL) {
                currElem->d = MIN_VAL;
        }
        if (currElem->b > MAX_VAL) {
                currElem->b = MAX_VAL;
        }
        if (currElem->c > MAX_VAL) {
                currElem->c = MAX_VAL;
        }
        if (currElem->d > MAX_VAL) {
                currElem->d = MAX_VAL;
        }
}
