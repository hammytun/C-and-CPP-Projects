/*
 *  main.cpp
 *  Harrison Tun
 *  11/7/24
 *
 *  COMP 15 Proj Zap
 *
 *  Purpose: This is the driver file for Zap. Zap compress a file using the 
 *           keyword zap. When running zap, a given ASCII text file is encoded
 *           into compressed binary code. The other componenet of zap is the
 *           unzap keyword. The unzap keyword takes a file that contains 
 *           compressed binary code and turns it back into text.
 *
 */

#include "HuffmanCoder.h"
#include <map>
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        //Incorrect argument count
        cerr << "Usage: ./zap [zap | unzap] inputFile outputFile" << endl;
        return EXIT_FAILURE;
    }
    string command = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];
    HuffmanCoder coder;
    //runs program
    if(command == "zap") {
        coder.encoder(inputFile, outputFile); //encodes text to binary
    } else if(command == "unzap") {
        coder.decoder(inputFile, outputFile); //decodes text from binary
    } else {
        cerr << "Usage: ./zap [zap | unzap] inputFile outputFile" << endl;
        return EXIT_FAILURE;
    }
    return 0;
}
