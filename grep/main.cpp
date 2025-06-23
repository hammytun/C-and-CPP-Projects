/*
 *  main.cpp
 *  Harrison Tun {htun01} and Jonah Pflaster {jplas01}
 *  11/30/24
 *
 *  COMP 15 Proj Gerp
 *
 *  Purpose: This is the driver file for Gerp. Gerp is a program that helps a
 *           user search for specific text within files in a directory. You 
 *           can use it to find words, phrases, or patterns in one or multiple 
 *           files, making it easier to locate and manage information.
 *
 */
#include "gerpProcessor.h"
using namespace std;

/*
* name: main
* purpose: Runs the gerp program. 
* arguments: a Command line argument and a integer of arguments
* returns: When the program is finished running, main returns 0
* effects: 
*/
int main(int argc, char *argv[]) 
{
    if (argc != 3) {
        cerr << "Usage: ./gerp inputDirectory outputFile" << endl;
        exit(EXIT_FAILURE);
    }
    string inputDirectory = argv[1];
    string outputFile = argv[2];
    
    IndexBuilder builder;
    builder.run(inputDirectory, outputFile);
    cout << "Goodbye! Thank you and have a nice day." << endl;
    return 0;
}