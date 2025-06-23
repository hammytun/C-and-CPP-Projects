#include "restoration.h"

/**************************************************************
 *
 *                     restoration.c
 *
 *     Assignment: filesofpix
 *     Authors:  Harrison Tun (htun01) and Brendan Roy (broy02)
 *     Date:     1/25/25
 *
 *     Summary:
 *     This is the implementation for restoration.c. This program restores a 
 *     corrupted P2 PGM file by identifying and removing sequences of non-digit 
 *     characters that have been introduced into the lines. The cleaned data is
 *     then converted and outputed in the P5 binary format. The program reads
 *     from a file or standard input and utilizes readaline.c to do it.
 *
 **************************************************************/

/********** main ********
*
* Runs the entire restoration program to convert a corrupted P2 pgm into a
* P5 pgm that stores the original data.
*
* Parameters:
*      int argc - number of arguments supplied to the command line
*      char *argv[] - a list of each argument supplied to the command line
*
* Returns:
*      returns an int, 0, to signal the program is over 
*
* Expects
*      argc should be <= 2, with the second argument being the corrupted file.
* Notes:
*      Throws Checked Runtime Error if file cannot be read or too many arguments
************************/
int main(int argc, char *argv[]) 
{
        assert(argc <= 2);
        FILE *inputfd;
        if (argc == 2) { inputfd = fopen(argv[1], "r"); }
        else           { inputfd = stdin; }
        assert(inputfd != NULL);
        
        Seq_T lines = Seq_new(100);
        Seq_T sizes = Seq_new(100);
        readFile(&lines, &sizes, inputfd);
        assert(Seq_length(lines) > 1 && Seq_length(sizes) > 1);
        char *infused = findSequence(&lines, &sizes);
        assert(infused != NULL);
        processLines(&lines, &sizes, infused);
        writeImage(&lines, &sizes);

        Seq_free(&lines);
        Seq_free(&sizes);
        free(infused);
        fclose(inputfd);
}

/********** writeImage ********
 *
 * Writes the final processed image data in P5 format to standard output
 *
 * Parameters:
 *      Seq_T *lines: sequence of the lines containing pixel data
 *      Seq_T *sizes: sequence of the sizes of each line
 *
 * Return: void, but prints to stdout
 *
 * Expects
 *      lines and sizes must not be null and indices should be synchronized
 * Notes:
 *      Frees memory associated with the pixel data in the sequences
 ************************/
void writeImage(Seq_T *lines, Seq_T *sizes)
{ 
        int width;
        int height = 0;
        for (int i = 0; i < Seq_length(*sizes); i++) {
                if (Seq_get(*sizes, i) != NULL) {
                        width = *((size_t*)Seq_get(*sizes, i));
                        height++;
                }
        }
        assert(height > 1 && width > 1);
        /* write P5 image header */
        printf("P5\n%d %d\n255\n", width, height);

        for (int i = 0; i < Seq_length(*lines); i++) {  
                if (Seq_get(*lines, i) != NULL) {
                        char *row = Seq_get(*lines, i);
                        int numPixels = *((size_t*)Seq_get(*sizes, i));
                        printRow(row, numPixels);
                        free(Seq_get(*lines, i));
                        free(Seq_get(*sizes,i));
                }   
        }
}
/********** printRow ********
 *
 * Converts a single row of space-separated numbers into a binary row of 
 * corresponding ASCII characters.
 *
 * Parameters:
 *      char *row:       string containing space-separated pixel values
 *      int numPixels:   number of pixels in the row
 *
 * Return: void, prints to stdout
 *
 * Expects:
 *      row must not be NULL, numPixels >= 0
 * Notes:
 *      N/A
 ************************/
void printRow(char *row, int numPixels)
{
        for (int j = 0; j <  numPixels && row != NULL; j++) {
                /* determine size of current number */
                int currSize = distToSpace(row);
                char currNum[currSize + 1];
                currNum[currSize] = '\0';
                for (int k = 0; k < currSize; k++) {
                        currNum[k] = row[k];
                }
                /* 
                 * moves row pointer forward to skip past current number and
                 * and the following space 
                 */
                row += (currSize + 1);

                char *endptr;
                int temp = strtol(currNum, &endptr, 10);
                printf("%c", (char)temp);   
        }
}
/********** distToSpace ********
 *
 * Finds the distance to the first space character in a string.
 *
 * Parameters:
 *      char *row: string to search
 *
 * Return:
 *      int: index of the first space character or the string's length if no 
 *           space is found
 *
 * Expects:
 *      row must not be NULL
 * Notes:
 *      N/A
 ************************/
int distToSpace(char *row)
{
        for (size_t i = 0; i < strlen(row); i++) {
                if (row[i] == ' ') { return i; }
        }
        return strlen(row);
}
/********** readFile ********
 *
 * Reads all lines from an input file, storing them and their sizes in 
 * sequences.
 *
 * Parameters:
 *      Seq_T *lines:    sequence to store the lines themselves
 *      Seq_T *sizes:    sequence to store the number of chars in each line
 *      FILE *inputfd:   file pointer to read from
 *
 * Return: void
 *
 * Expects:
 *      lines, sizes, and inputfd must not be NULL.
 * Notes:
 *      Allocates memory for each line and its size, which its caller must free.
 ************************/
void readFile(Seq_T *lines, Seq_T *sizes, FILE *inputfd) 
{
        bool atEnd = false;
        while (!atEnd) {
                char *currLine;
                size_t *currSize = allocMemory(currSize, sizeof(*currSize), 1);
                *currSize = readaline(inputfd, &currLine);
                if (currLine != NULL) {
                        Seq_addhi(*sizes, currSize);
                        Seq_addhi(*lines, currLine); 
                } else {
                        /* Free extra memory if EOF is reached */
                        free(currSize);
                        atEnd = true;
                }
        }
}
/********** findSequence ********
 *
 * Finds a sequence of non-digit characters that appears in multiple lines.
 *
 * Parameters:
 *      Seq_T *lines:    sequence of lines to analyze
 *      Seq_T *sizes:    sequence of sizes for each line
 *
 * Return:
 *      char*: sequence of non-digit characters or NULL if no repeated sequence 
 *             is found
 *
 * Expects:
 *      lines and sizes must not be NULL and must be synchronized.
 * Notes:
 *      Returns a dynamically allocated string that the caller of findSequence
 *      must free.
 ************************/
char *findSequence(Seq_T *lines, Seq_T *sizes) 
{
        Table_T table = Table_new(100, NULL, NULL);
        int one = 1;
        for (int i = 0; i < Seq_length(*lines); i++) {
                if (((char*)Seq_get(*lines, i))[0] == '\n') { continue; }
                int index = 0;
                char *line = Seq_get(*lines, i);
                size_t *length = ((size_t*)Seq_get(*sizes, i));
                char currInfusion[*length];                 
                for (int j = 0; j < (int) *length - 1; j++) {
                        if (!isdigit(line[j])) {
                                currInfusion[index++] = line[j];
                        }
                }
                currInfusion[index] = '\0';
                const char *atomInfusion = Atom_new(currInfusion, index + 1);
                /* Once we find the repeated infusion, stop iterating lines */
                if (Table_put(table, atomInfusion, &one) != NULL) { 
                        Table_free(&table);
                        char *infused = allocMemory(&infused, 1, index + 1);
                        for (int j = 0; j < index + 1; j++) {
                                infused[j] = currInfusion[j];
                        }
                        return infused; 
                }
        }
        Table_free(&table);
        return NULL;
}

/********** processLines ********
 *
 * Processes each line to remove corrupted data and retain valid pixels.
 *
 * Parameters:
 *      Seq_T *lines:      sequence of lines to process
 *      Seq_T *sizes:      sequence of sizes for each line
 *      const char *infused: string of non-digit characters to remove
 *
 * Return: void
 *
 * Expects:
 *      lines, sizes, and infused must not be NULL and must be synchronized.
 * Notes:
 *      Modifies the contents of lines and sizes in place.
 ************************/
void processLines(Seq_T *lines, Seq_T *sizes, const char *infused)
{        
        for (int i = 0; i < Seq_length(*lines); i++) {
                size_t *newLineSize = allocMemory(newLineSize, 
                                                  sizeof(*newLineSize), 1);
                *newLineSize = 0;
                Seq_T newLine = checkLine(lines, sizes, infused, i, 
                                          newLineSize);
                if (newLine == NULL) { 
                        free(newLineSize);
                        continue; 
                }  
                 
                char *origLine = seqToString(&newLine);
                char *oldLine = Seq_get(*lines, i);
                size_t *oldSizes = Seq_get(*sizes, i);

                Seq_put(*lines, i, origLine);
                Seq_put(*sizes, i, newLineSize);

                Seq_free(&newLine);
                free(oldLine);
                free(oldSizes);
        }
}
/********** checkLine ********
 *
 * Checks and processes a single line to remove corrupted data and retain valid 
 * pixels.
 *
 * Parameters:
 *      Seq_T *lines:        sequence of lines
 *      Seq_T *sizes:        sequence of sizes
 *      const char *infused: string of corrupted characters
 *      int currIdx:         index of the current line to check
 *      size_t *newLineSize: address to store the size of the processed line
 *
 * Return:
 *      Seq_T: sequence representing the new processed line, or NULL if 
 *      corrupted
 *
 * Expects:
 *      lines, sizes, infused, and newLineSize must not be NULL.
 * Notes:
 *      N/A
 ************************/
Seq_T checkLine(Seq_T *lines, Seq_T *sizes, const char *infused, int currIdx, 
                size_t *newLineSize) 
{
        int infusedIterator = 0;
        int *lineLength = Seq_get(*sizes, currIdx);
        Seq_T newLine = Seq_new(20);
        /* The spec says newline chars are not part of the infused sequence */
        if (((char *)Seq_get(*lines, currIdx))[0] == '\n') { 
                deleteInfo(*lines, *sizes, &newLine, currIdx);   
                return NULL; 
        }
        for (int i = 0; i < *lineLength - 1; i++) {
                char *currChar = &((char*)Seq_get(*lines, currIdx))[i];
                if (*currChar == infused[infusedIterator]) {
                        infusedIterator++;
                        continue;
                /* if a non digit is found, mark line as corrupted */
                }  else if (!isdigit(*currChar)) {
                        deleteInfo(*lines, *sizes, &newLine, currIdx);
                        return NULL;
                }
                /* process valid characters and add to new sequence */
                processPixel(&currChar, &i, *lineLength, &newLine, newLineSize);
        } 
        if (infusedIterator < (int)strlen(infused)) { 
                deleteInfo(*lines, *sizes, &newLine, currIdx);
        }
        return newLine;
}
/********** processPixel ********
 *
 * Processes a single character from the line, adds it to the new sequence, 
 * and checks for validity.
 *
 * Parameters:
 *      char **currChar:      pointer to the line being processed
 *      int *i:               pointer to the current index in the line
 *      int lineLength:       total length of the line
 *      Seq_T newLine:        sequence to store the processed characters
 *      size_t *newLineSize:  pointer to store the size of the processed line
 *
 * Return:
 *      int: 1 if the character is valid and processed, 0 if corrupted
 *
 * Expects:
 *      currChar, i, newLine, and newLineSize must not be NULL.
 * Notes:
 *      Frees memory for invalid sequences.
 ************************/
void processPixel(char **currChar, int *i, int lineLength, Seq_T *newLine, 
                size_t *newLineSize)
{
        while (*i < lineLength && isdigit(**currChar)) {
                Seq_addhi(*newLine, *currChar); 
                (*currChar)++;
                (*i)++;
        }
        /* each pixel, represented as a 1, 2, or 3 digit number, counts as 1 */
        (*newLineSize)++;
        /* adjust i, prevents skipping */
        (*i)--; 
       
        char *space = allocMemory(&space, sizeof(*space), 1);
        *space = ' ';
        Seq_addhi(*newLine, space);
}

/********** seqToString ********
 *
 * Converts a sequence of characters into a null-terminated string.
 *
 * Parameters:
 *      Seq_T *line: sequence of character arrays (lines of file)
 *
 * Return:
 *      char*: dynamically allocated string
 *
 * Expects:
 *      line must not be NULL.
 * Notes:
 *      Frees spaces within the sequence.
 ************************/
char *seqToString(Seq_T *line)
{
        char *string = allocMemory(&string, sizeof(*string), 
                                   Seq_length(*line) + 1);
        for (int a = 0; a < Seq_length(*line); a++) {
                string[a] = *((char *)Seq_get(*line, a));
                /* free the extra space characters */
                if (string[a] == ' ') { free(Seq_get(*line, a)); }
        } 
        string[Seq_length(*line)] = '\0';
        return string;
}

/********** deleteInfo ********
 *
 * Deletes corrupted line and size data at a specific index in their sequences.
 * Also deletes/frees the new line.
 *
 * Parameters:
 *      Seq_T lines:     sequence of lines from file
 *      Seq_T sizes:     sequence of sizes of each line
 *      Seq_T *newLine:  sequence of characters to free
 *      int index:       index of the line/size to delete
 *
 * Return: void
 *
 * Expects:
 *      lines, sizes, and newLine must not be NULL.
 * Notes:
 *      N/A
 ************************/
void deleteInfo(Seq_T lines, Seq_T sizes, Seq_T *newLine, int index) 
{
        for (int i = 0; i < Seq_length(*newLine); i++) {
                if (*((char *)Seq_get(*newLine, i)) == ' ') { 
                        free(Seq_get(*newLine, i)); 
                }
        }
        Seq_free(newLine);
        free(Seq_get(lines, index));
        free(Seq_get(sizes, index));

        Seq_put(lines, index, NULL);
        Seq_put(sizes, index, NULL);
}