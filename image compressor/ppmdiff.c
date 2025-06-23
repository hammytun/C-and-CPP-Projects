#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"
#include "uarray2.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double compareImages(Pnm_ppm fileOne, Pnm_ppm fileTwo, A2Methods_T one,
                     A2Methods_T two);

int main(int argc, char *argv[])
{
        A2Methods_T methodsOne = uarray2_methods_plain;
        A2Methods_T methodsTwo = uarray2_methods_plain;
        if (argc != 3) {
                fprintf(stderr, "OH NO! PLEASE USE IT CORRECTLY!\n");
        }
        FILE *inputSecondFile =
                (strcmp(argv[2], "-") == 0) ? stdin : fopen(argv[2], "r");
        FILE *inputFirstFile = fopen(argv[1], "r");

        assert(inputFirstFile != NULL && inputSecondFile != NULL);
        
        Pnm_ppm fileOne = Pnm_ppmread(inputFirstFile, methodsOne);
        Pnm_ppm fileTwo = Pnm_ppmread(inputSecondFile, methodsTwo);
        assert(fileOne != NULL && fileTwo != NULL);

        // printf("flowers W: %d, flowers H: %d\n new W: %d, new H: %d\n",
        //         fileOne->width, fileOne->height, fileTwo->width,
        //         fileTwo->height);
        double difference =
                compareImages(fileOne, fileTwo, methodsOne, methodsTwo);
        Pnm_ppmfree(&fileOne);
        Pnm_ppmfree(&fileTwo);
        fclose(inputFirstFile);
        fclose(inputSecondFile);
        printf("%0.4f\n", difference);
}

double compareImages(Pnm_ppm fileOne, Pnm_ppm fileTwo, A2Methods_T one,
                     A2Methods_T two)
{
        unsigned firstWidth = fileOne->width;
        unsigned secondWidth = fileTwo->width;
        unsigned width;
        unsigned firstHeight = fileOne->height;
        unsigned secondHeight2 = fileTwo->height;
        unsigned height;
        width = (firstWidth < secondWidth) ? firstWidth : secondWidth;
        height = (firstHeight < secondHeight2) ? firstHeight : secondHeight2;

        width -= width % 2;
        height -= height % 2;

        double red = 0, green = 0, blue = 0;
        float sum = 0;
        double scale = fileOne->denominator;
        double scale2 = fileTwo->denominator;
        for (unsigned i = 0; i < width; i++) {
                for (unsigned j = 0; j < height; j++) {
                        Pnm_rgb image1 =
                                (Pnm_rgb)(one->at(fileOne->pixels, i, j));
                        Pnm_rgb image2 =
                                (Pnm_rgb)(two->at(fileTwo->pixels, i, j));

                        red = (double)(image1->red) / scale -
                              (double)(image2->red) / scale2;
                        green = (double)(image1->green) / scale -
                                (double)(image2->green) / scale2;
                        blue = (double)(image1->blue) / scale -
                               (double)(image2->blue) / scale2;

                        sum += (red * red) + (green * green) + (blue * blue);
                }
        }
        sum /= (3.0 * width * height);
        return sqrt(sum);
}