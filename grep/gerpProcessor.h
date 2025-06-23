/*
 *  gerpProcessor.h
 *  Harrison Tun and Jonah Pflaster
 *  {htun01}         {jpflas01}
 *  11/30/24
 *
 *  COMP 15 Proj Gerp
 *
 *  Purpose: This header file defines the interface for the IndexBuilder class, 
 *          which is responsible for building the file tree and hash table. 
 *          The IndexBuilder includes methods for building a 
 *          FSTree tree, stripping words accordingly, and inserting data into 
 *          the hashtable.
 *
 */

#ifndef GERP_PROCESSOR_H
#define GERP_PROCESSOR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cctype>
#include "FSTree.h"
#include "DirNode.h"
#include "hashTable.h"
using namespace std;



class IndexBuilder 
{
    public:
        void run(string inputDirectory, string &outputFile);
    private:
        HashTable table;
        int fileIndexCounter = 0;
        void buildIndex(string directoryPath);
        void processFile(string &filePath);
        void traverseHelper(DirNode *root, string currPath);
        string stripNonAlphaNum(string &input);
        void queryLoop(string& output);
        void findWord(string &input, ofstream& output, bool caseSens);
        void readWords(pair<int, int> node, ofstream &output);
};

#endif