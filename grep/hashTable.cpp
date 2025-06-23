/*
 *  hashTable.cpp
 *  Harrison Tun and Jonah Pflaster
 *  12/3/24
 *
 *  COMP 15 Proj Gerp
 *
 *  Purpose: This implementation file provides the function definitions for 
 *          the HashTable class. It includes logic for getting the file path,
 *          line numbers, words, etc. It also allows for inserting all of the 
 *          aforementioned pieces of data. And allows for getting words in
 *          both case sensitive and case insensitive manners. This from hashing
 *          the lowercase version as the key but then storing the actual word
 *          which allows access to a vector of pairs of ints which the ints
 *          correspond to the file table index of the file table and the line
 *          number in that file the word appears.
 *
 */

#include "hashTable.h"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>

/*
 * Default constructor for HashTable class
 */
HashTable::HashTable () 
{
    wordTable.resize(tableCap);
    wordSize = 0; 
}

/*
 * Destructor for HashTable class
 */
HashTable:: ~HashTable() {}

/*
 * name:        getFile (member function)
 * purpose:     Retrieve the file path associated with a given key
 * arguments:
 *     key: integer key to identify the file
 * returns:     string file path corresponding to the key
 * effects:     Throws a runtime_error if the key is not found
 */
string HashTable:: getFile(int key) 
{
    if (key < 0 or key >= int(fileTable.size())) {
        throw runtime_error("key not found");
    }
    return fileTable[key].filePath;
}

/*
 * name:        getLine (member function)
 * purpose:     Retrieve the line content associated with a given key and line 
 *              number
 * arguments:
 *     key: integer key to identify the file
 *     lineNum: line number to retrieve content from
 * returns:     string line content at the specified key and line number
 * effects:     Throws a runtime_error if the key is not found
 */
string HashTable:: getLine(int key, int lineNum) 
{
    if (key < 0 or key >= int(fileTable.size())) {
        throw runtime_error("key not found");
    }
    return fileTable[key].lineContent[lineNum];
}

/*
 * name:        getLineContent (member function)
 * purpose:     Retrieve the line content associated with a given key
 * arguments:
 *     key: integer key to identify the file
 * returns:     vector<string>& reference to the line content vector
 * effects:     Throws a runtime_error if the key is not found
 */
vector<string>& HashTable:: getLineContent(int key) 
{
    if (key < 0 or key >= int(fileTable.size())) {
        throw runtime_error("key not found");
    }
    return fileTable[key].lineContent;
}

/*
 * name:        getWord (member function)
 * purpose:     Get all word occurrences matching a given key 
 *              (case-insensitive)
 * arguments:
 *     key: string key to search for
 * returns:     set<pair<int, int>> vector of wordVars objects containing 
 *              matches
 * effects:     None
 */
set<pair<int, int>> HashTable:: getWord(string key) 
{
    size_t bucketIndex = hashWord(toLowerCase(key)) % wordTable.size();
    set<pair<int, int>> seenLines;
    for (auto &node : wordTable[bucketIndex]) {
        if (toLowerCase(node.key) == toLowerCase(key)) {
            //if key matches, add node to set
            for(auto &lines : node.values) { //making sure no duplicates
                seenLines.insert(lines);
            }
        }
    }
    return seenLines; //return set containing all matching wordVars
}

/*
 * name:        getCaseSensWord (member function)
 * purpose:     Get the word occurrence matching a given key (case-sensitive)
 * arguments:
 *     key: string key to search for
 * returns:     wordVars* pointer to the wordVars object if found, nullptr 
 *              otherwise
 * effects:     None
 */
wordVars* HashTable:: getCaseSensWord(string key) 
{
    size_t bucketIndex = hashWord(toLowerCase(key)) % wordTable.size();
    for (auto &node : wordTable[bucketIndex]) {
        if (node.key == key) { //check for caseSens key
            return &node;
        }
    }
    return nullptr; //return nullptr, if no match is found
}

/*
 * name:        insertFile (member function)
 * purpose:     Insert a new file entry into the hash table
 * arguments:
 *     file: string representing the file path
 *     lines: vector of strings containing the lines of the file
 * returns:     int index of the inserted file
 * effects:     None
 */
int HashTable:: insertFile(string &file, vector<string> &lines) 
{
    fileTable.push_back({file, lines});
    return (fileTable.size() - 1);
}

/*
 * name:        insertWord (member function)
 * purpose:     Insert a new word occurrence into the hash table
 * arguments:
 *     key: string key to insert
 *     index: reference to the index of the word
 *     value: reference to the value of the word
 * returns:     Nothing
 * effects:     Resizes the hash table if the load factor exceeds 0.75
 */
void HashTable::insertWord(string key, int &index, int &value) 
{
    // Compute the bucket for the word
    size_t bucket = hashWord(toLowerCase(key)) % wordTable.size();
    // Iterate through the bucket to find a matching wordVars
    for (auto& node : wordTable[bucket]) {
        if (node.key == key) {
            auto newPair = make_pair(index, value);
            //pair is inserted correctly, don't continue
            insertSorted(node.values, newPair);
            return; //duplicate is found, do nothing
        }
    }
    // If no match is found, create a new wordVars entry
    wordVars newNode = {key, {}};
    insertSorted(newNode.values, {index, value});
    wordTable[bucket].push_back(newNode);
    // Increment the size of words tracked
    wordSize++;
    // Resize if the load factor exceeds 0.75
    if (wordSize > 0.75 * wordTable.size()) {
        wordResize();
    }   
}

/*
 * name:        insertSorted (member function)
 * purpose:     check for duplicates
 * arguments:
 *     values: reference to vector of pair of ints
 *     newPair: reference to pair of ints
 * returns:     nothing
 * effects:     Keeps duplicates out of vector
 */
void HashTable::insertSorted(vector<pair<int, int>>& values, const pair<int, 
                             int>& newPair) 
{
    if (values.empty() or values.back() < newPair) {
        values.push_back(newPair); 
        return;
    }
    if (values.back() == newPair) { //check last element for duplicates
        return;
    }
    values.push_back(newPair); 
    return;
}

/*
 * name:        getFileSize (member function)
 * purpose:     Get the number of files stored in the hash table
 * arguments:   None
 * returns:     int number of files in the table
 * effects:     None
*/
int HashTable:: getFileSize() 
{
    return fileTable.size();
}

/*
 * name:        getWordSize (member function)
 * purpose:     Get the number of unique words stored in the hash table
 * arguments:   None
 * returns:     int number of unique words in the table
 * effects:     None
 */
int HashTable:: getWordSize() 
{
    return wordSize;
}

/*
 * name:        wordResize (member function)
 * purpose:     Resize the hash table when the load factor exceeds 0.75
 * arguments:   None
 * returns:     Nothing
 * effects:     Rehashes the word table to a larger size
 */
void HashTable:: wordResize() 
{
    vector<vector<wordVars>> temp(wordTable.size() * 2 + 1);
    for (auto &bucket : wordTable) {
        for (auto& node : bucket) {
            size_t bucketIndex = hashWord(toLowerCase(node.key)) % temp.size();
            temp[bucketIndex].push_back(node);
        }
    }
    wordTable = std::move(temp);
}

/*
 * name:        hashWord (member function)
 * purpose:     Hash a string key to a size_t value
 * arguments:
 *     key: string key to hash
 * returns:     size_t hash value of the key
 * effects:     None
 */
size_t HashTable::hashWord(string key)
{
    return std::hash<string>{}(key);
}

/*
 * name:        getCapacity (member function)
 * purpose:     Get the current capacity of the word table
 * arguments:   None
 * returns:     int current capacity of the word table
 * effects:     None
 */
int HashTable:: getCapacity() 
{
    return wordTable.capacity();
}

/*
* name: toLowerCase
* purpose: Converts a word to lowercase
* arguments: word - the word to convert
* returns: the word converted to lowercase
* effects: Changes the word to lowercase
*/
string toLowerCase(string word) 
{
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    return word;
}