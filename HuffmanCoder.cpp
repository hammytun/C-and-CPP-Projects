/*
 *  HuffmanCoder.cpp
 *  Harrison Tun
 *  11/7/24
 *
 *  COMP 15 Proj Zap
 *
 *  Purpose: This file is the implementation of the HuffmanCoder class. It 
 *           includes the encoder and decoder implementation which are used in 
 *           the driver file. It also builds the Huffman Tree, generates 
 *           the binary code, and handles any valgrind cleanup.
 *
 */

#include "HuffmanCoder.h"
#include <fstream>
#include <iostream>
#include <queue>
#include <utility>
const int ASCII_SIZE = 256;

/*
 * name:      encoder( )
 * purpose:   Encodes a text file into binary, where a file is written to hold
 *            the binary string.
 * arguments: reference to a input file and output file string 
 * returns:   NADA
 * effects:   NADA
 */
void HuffmanCoder::encoder(const string &inputFile, const string &outputFile) {
    int frequency[ASCII_SIZE] = {0};
    ifstream input(inputFile);
    if (not input) {
        throw std::runtime_error("Unable to open file " + inputFile);
    }
    //string that holds input file's content
    string input_string { istreambuf_iterator<char>(input), 
                          istreambuf_iterator<char>() };
    if (input_string.empty()) {
        cout << inputFile << " is empty and cannot be compressed." << endl;
        return;
    }
    count_frequency(input_string, frequency); //counts freq of chars
    HuffmanTreeNode *root = build(frequency); //builds tree
    unordered_map<char, string> codes;
    generateCodes(root, "", codes); //makes binary code
    string encoded_data;
    for (char&c : input_string) {
        encoded_data += codes[c];
    }
    string serialized_tree = serialize_tree(root);
    writeZapFile(outputFile, serialized_tree, encoded_data);
    input.close();
    cout << "Success! Encoded given text using " 
    << encoded_data.length() << " bits." << endl;
    deleteNodes(root); //inorder traversal function to delete tree
}
/*
 * name:      Decoder( )
 * purpose:   Decodes a binary text file. Deserializes string to create 
              original Huffman tree. Outputs result to output file.
 * arguments: reference to a input file and output file string 
 * returns:   NADA
 * effects:   NADA
 */
void HuffmanCoder::decoder(const string &inputFile, const string &outputFile) {
    pair<string, string> data = readZapFile(inputFile);
    string serialized_tree = data.first; //serialized tree string
    string encoded_data = data.second; //binary string
    HuffmanTreeNode *root = deserialize_tree(serialized_tree);
    string decoded_data;
    HuffmanTreeNode* curr_root = root;
    for (size_t i = 0; i < encoded_data.size(); i++) {
        if (encoded_data[i] == '0') {
            curr_root = curr_root->get_left(); //traverse left
        } else {
            curr_root = curr_root->get_right(); //traverse right
        } if (curr_root->is_leaf()) {
            decoded_data += curr_root->get_val(); //append root val if leaf
            curr_root = root; //curr root becomes root
        } 
    }
    if (curr_root != root and not curr_root->is_leaf()) {
        //last node has to be a leaf
        throw runtime_error("Encoding did not match Huffman tree.");
    }
    ofstream output(outputFile);
    output << decoded_data;
    deleteNodes(root); //inorder traversal function to delete tree
}
/*
 * name:      count_frequency( )
 * purpose:   counts the instance of each character in an inputted string.
 *            Adds character to int array which holds the ASCII value of the 
 *            character.
 * arguments: input string and frequency array
 * returns:   NADA
 * effects:   NADA
 */
void HuffmanCoder::count_frequency(string inputFile, int frequency[]) {
    frequency[ASCII_SIZE] = {0};
    for (char& c : inputFile) {
        frequency[(int)c]++;
    }
}
/*
 * name:      build( )
 * purpose:   builds the Huffman tree using a priority queue
 * arguments: integer array holding each character's frequency
 * returns:   pointer to the root of the Huffman Tree
 * effects:   NADA
 */
HuffmanTreeNode* HuffmanCoder::build(int frequency[]) {
    priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, 
    NodeComparator> my_pq;
    for (int i = 0; i < ASCII_SIZE; i++) {
        if(frequency[i] > 0) {
            //populating priority queue
            my_pq.push(new HuffmanTreeNode((char)i, frequency[i]));
        }
    }
    if (my_pq.size() == 1) {
        HuffmanTreeNode *singlet = my_pq.top(); my_pq.pop();
        HuffmanTreeNode *root = new HuffmanTreeNode('\0', singlet->get_freq());
        root->set_left(singlet);
        my_pq.push(root);
    }
    while (my_pq.size() > 1) {
        //pick the two min frequency nodes
        HuffmanTreeNode *left = my_pq.top(); my_pq.pop();
        HuffmanTreeNode *right = my_pq.top(); my_pq.pop();
        //join together with parent node that stores children's freq 
        int freq = left->get_freq() + right->get_freq();
        HuffmanTreeNode *parent = new HuffmanTreeNode(freq, left, right);
        parent->set_left(left);
        parent->set_right(right);
        my_pq.push(parent);
    }
    HuffmanTreeNode* temp = my_pq.top();
    my_pq.pop();
    return temp;
}
/*
 * name:      generateCodes( )
 * purpose:   generates binary code for given huffman tree. Traverses the tree
 *            adds a 0 to the hashmap for each left turn and a 1 for each 
 *            right turn. When there is a leave, stores the code in the map
 *            with the value as its key.
 * arguments: A pointer to the tree's root, a reference to a string, 
 *            a reference to a hashmap that associates each character with its
 *            generated binary code.
 * returns:   NADA
 * effects:   NADA
 */
void HuffmanCoder::generateCodes(HuffmanTreeNode *root, const string &code, 
                                 unordered_map<char, std::string> &codes) {
    if (root == nullptr) {
        return; 
    } else if (root->is_leaf()) {
        //set key as value
        codes[root->get_val()] = code;
    }
    //traverse left and add 0
    generateCodes(root->get_left(), code + "0", codes);
    //traverse right and add 1
    generateCodes(root->get_right(), code + "1", codes);
}
/*
 * name:      serialize_tree( )
 * purpose:   Stores Huffman tree as a string. Traverses the tree using the 
              preorder traversal type distinguishing internal nodes as 'I' and
              Leaf nodes as 'L' followed by their character.
 * arguments: a pointer to a HuffmanTreeNode
 * returns:   String of the Huffman Tree
 * effects:   NADA
 */
string HuffmanCoder::serialize_tree(HuffmanTreeNode *root){
    if (not root) {
        return "";
    }if (root->is_leaf()) {
        //Adds L for leaf
        return "L" + string(1, root->get_val());
    }
    //Adds I for internal node
    return "I" + serialize_tree(root->get_left()) + 
    serialize_tree(root->get_right());
}
/*
 * name:      deserialize_tree( )
 * purpose:   calls the deserialize helper function and returns the tree 
              converted back to its tree form
 * arguments: an address to a string
 * returns:   NADA
 * effects:   A pointer to a HuffmanTreeNode
 */
HuffmanTreeNode* HuffmanCoder::deserialize_tree(const std::string &s){
    int index = 0;
    //calls helper function
    return deserialize_helper(s, index);
}
/*
 * name:      deserialize_helper( )
 * purpose:   Creates a HuffmanTreeNode for the curr node from the serialized
              string. If the node is a non-leaf, it recursively deserializes
              the node's left subtree, then its right.
 * arguments: an address to a string and an address to an int
 * returns:   NADA
 * effects:   calls handle command with the content inside the rstring
 */
HuffmanTreeNode* HuffmanCoder::deserialize_helper(const std::string &s, 
                                                  int &index) {
    int stringLen = s.length();
    if (index >= stringLen) {
        return nullptr;
    }
    char type = s[index++];
    if (type == 'L') {
        //If Leaf, create node with value
        char value = s[index++];
        return new HuffmanTreeNode(value, 1);
    }
    else if (type == 'I') {
        //internal node creation
        HuffmanTreeNode *left = deserialize_helper(s, index);
        HuffmanTreeNode *right = deserialize_helper(s, index); 
        HuffmanTreeNode *node = new HuffmanTreeNode('\0' , 0, left, right);
        return node;
    }
    return nullptr;
}
/*
 * name:      deleteNodes( )
 * purpose:   valgrind cleanup that uses in order traversal method to delete
 *            each node in the tree
 * arguments: A pointer to the tree's root.
 * returns:   NADA
 * effects:   NADA
 */
void HuffmanCoder::deleteNodes(HuffmanTreeNode *root){
    if (root == nullptr) {
        return;
    } if(root->is_leaf()) {
        delete root;
        return;
    }
    //left, right, curr
    deleteNodes(root->get_left());
    deleteNodes(root->get_right());
    delete root;
}