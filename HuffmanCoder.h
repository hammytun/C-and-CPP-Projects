/*
 *  HuffmanCoder.h
 *  Harrison Tun
 *  11/7/24
 *
 *  COMP 15 Proj Zap
 *
 *  Purpose: This is the header file for the implementation of the 
 *   HuffmanCoder class. 
 *
 */
#ifndef _HUFFMAN_CODER
#define _HUFFMAN_CODER

#include <string>
#include <unordered_map>
#include "HuffmanTreeNode.h"
#include "ZapUtil.h"
using namespace std;

class HuffmanCoder {
    // Feel free to add additional private helper functions as well as a
    // constructor and destructor if necessary
    public:
    void encoder(const std::string &inputFile, const std::string &outputFile);
    void decoder(const std::string &inputFile, const std::string &outputFile);
    private:
       void generateCodes(HuffmanTreeNode *root, const string &code, 
                       unordered_map<char, std::string> &codes);
        HuffmanTreeNode *build(int frequency[]);
        void count_frequency(string inputFile, int frequency[]);
        std::string serialize_tree(HuffmanTreeNode *root);
        HuffmanTreeNode *deserialize_tree(const std::string &s);
        HuffmanTreeNode *deserialize_helper(const string &s, int &index);
        void deleteNodes(HuffmanTreeNode *root);
    
};

#endif