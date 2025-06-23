/*
 *  hashTable.h
 *  Harrison Tun and Jonah Pflaster
 *  {htun01}         {jpflas01}
 *  12/3/24
 *
 *  COMP 15 Proj Gerp
 *
 *  Purpose: This header file defines the interface for the HashTable class, 
 *          which is responsible for building the file table and hash table
 *          of words. The HashTable includes methods for inserting into a
 *          file table which is a vector of fileVars, then being able to get 
 *          the data from the fileVars, inserting data into the hashtable of 
 *          words and being able to get those words with the corresponding
 *          file and line number.
 *
 */

#ifndef __HASHTABLE_H
#define __HASHTABLE_H
#include <set>
#include <vector>
#include <functional>
#include <list>
#include <stdexcept>
#include <utility>
using namespace std;

struct wordVars {
    string key;
    vector<pair<int, int>> values;
};

string toLowerCase(string key);

class HashTable 
{
    public:
        HashTable();
        ~HashTable();
        string getFile(int key);
        set<pair<int, int>> getWord(string key);
        wordVars* getCaseSensWord(string key);
        int insertFile(string &file, vector<string> &lines);
        void insertWord(string key, int &index, int &value);
        int getFileSize();
        int getWordSize();
        int getCapacity();
        vector<string>& getLineContent(int key);
        string getLine(int key, int lineNum);
        void insertSorted(vector<pair<int, int>>& values, 
                          const pair<int, int>& newPair);
    private:
        struct fileVars {
            string filePath;
            vector<string> lineContent;
        };
        
        int tableCap = 1001;
        vector<fileVars> fileTable;
        //first elem in pair is key
        vector<vector<wordVars>> wordTable; 
        size_t wordSize;
        void wordResize();
        size_t hashWord(string key);
};

#endif