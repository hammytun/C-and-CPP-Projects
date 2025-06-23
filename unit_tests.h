/*
 *  unit_tests.h
 *  Harrison Tun
 *  COMP 15 Proj Zap
 *  11/5/24
 *
 *  Purpose: Test functionality of Zap's functions
 *
 */
#include "phaseOne.h"
#include "HuffmanTreeNode.h"
#include "HuffmanCoder.h"
#include "ZapUtil.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
const int ASCII_SIZE = 256;
// void test_frequencies(){
//     int frequency[ASCII_SIZE] = {0};
//     istringstream bruh("Harrison's nice apple");
//     count_freqs(bruh, frequency);
// }
// void test_myserialize_tree() {
//     // Create a small Huffman tree manually
//     HuffmanTreeNode *root = new HuffmanTreeNode('\0', 0); 
//     HuffmanTreeNode *leftChild = new HuffmanTreeNode('a', 5); 
//     HuffmanTreeNode *rightChild = new HuffmanTreeNode('\0', 0); 
//     HuffmanTreeNode *rightLeft = new HuffmanTreeNode('b', 3); 
//     HuffmanTreeNode *rightRight = new HuffmanTreeNode('c', 2); 
//     // Set up the tree structure
//     root->set_left(leftChild);
//     root->set_right(rightChild);
//     rightChild->set_left(rightLeft);
//     rightChild->set_right(rightRight);
//     // Call serialize_tree
//     std::string serialized = serialize_tree(root);
//     std::string expected = "ILaILbLc"; 
//     if (serialized == expected) {
//         std::cout << "Test passed: " << serialized << std::endl;
//     } else {
//         std::cout << "Test failed: expected " << expected << ", got " 
//         << serialized << std::endl;
//     }
//     delete leftChild;
//     delete rightLeft;
//     delete rightRight;
//     delete rightChild;
//     delete root;
// }
// bool trees_equal(HuffmanTreeNode* a, HuffmanTreeNode* b) {
//     if (not a and not b) return true; // Both are null
//     if (not a or not b) return false; // One is null and the other is not
//     if (a->get_val() != b->get_val()) return false; // Compare values
//     return trees_equal(a->get_left(), b->get_left()) and 
//     trees_equal(a->get_right(), b->get_right());
// }
// void test_serialize_deserialize_tree() {
//     HuffmanTreeNode *root = new HuffmanTreeNode('\0', 0); 
//     HuffmanTreeNode *leftChild = new HuffmanTreeNode('a', 5); 
//     HuffmanTreeNode *rightChild = new HuffmanTreeNode('\0', 0);
//     HuffmanTreeNode *rightLeft = new HuffmanTreeNode('b', 3); 
//     HuffmanTreeNode *rightRight = new HuffmanTreeNode('c', 2); 
//     // Set up the tree structure
//     root->set_left(leftChild);
//     root->set_right(rightChild);
//     rightChild->set_left(rightLeft);
//     rightChild->set_right(rightRight);
//     // Serialize the tree
//     std::string serialized = serialize_tree(root);
//     // Deserialize the serialized string
//     HuffmanTreeNode *deserializedRoot = deserialize_tree(serialized);
//     // Check if the original tree and deserialized tree are equal
//     if (trees_equal(root, deserializedRoot)) {
//         std::cout << "Test passed successfully." << std::endl;
//     } else {
//         std::cout << "Test failed: Trees do not match." << std::endl;
//     }
//     // Clean up allocated memory
//     delete leftChild;
//     delete rightLeft;
//     delete rightRight;
//     delete rightChild;
//     delete root;
//     delete deserializedRoot; // Ensure to also delete the deserialized tree
// }
// void test_deserialize_tree() {
//     char internalNodeVal = '\0';
//     // Step 1: Create the Huffman tree using makeFigure1Tree
//     HuffmanTreeNode *original_tree = makeFigure1Tree(internalNodeVal);
//     // Step 2: Serialize the original tree
//     std::string serialized_tree = serialize_tree(original_tree);
//     // Step 3: Deserialize the serialized tree
//     size_t index = 0;
//     HuffmanTreeNode *deserialized_tree = deserialize_tree(serialized_tree);
//     // Step 4: Print both trees to verify visually (optional)
//     std::cout << "Original Tree Structure:\n";
//     printTree(original_tree, internalNodeVal);
//     std::cout << "\nDeserialized Tree Structure:\n";
//     printTree(deserialized_tree, internalNodeVal);
// }
// void test_serialize_tree() {
//     char internalNodeVal = '\0';
//     // Step 1: Create the Huffman tree using makeFigure1Tree
//     HuffmanTreeNode *original_tree = makeFigure1Tree(internalNodeVal);
//     // Step 2: Serialize the original tree
//     std::string serialized_tree = serialize_tree(original_tree);
//     std::string expected = "IIILaLbILeLfILcLd"; 
//     if (serialized_tree == expected) {
//         std::cout << "Test passed: " << serialized_tree << std::endl;
//     } else {
//         std::cout << "Test failed: expected " << expected << ", got " 
//         << serialized_tree << std::endl;
//     }
//     delete original_tree;
// }

// bool trees_equal(HuffmanTreeNode* a, HuffmanTreeNode* b) {
//     if (not a and notb) return true; // Both are null
//     if (not a or notb) return false; // One is null and the other is not
//     if (a->get_val() != b->get_val() or a->get_freq() != b->get_freq()) {
//         return false; // Compare values
//     } 
//     return trees_equal(a->get_left(), b->get_left()) and 
//     trees_equal(a->get_right(), b->get_right());
// }
// void verifyLeafFrequencies(HuffmanTreeNode* node, const std::vector<int>& 
//                            frequency) {
//     if (node == nullptr) {
//         return;
//     }
//     // If it's a leaf node, verify its frequency
//     if (node->get_left() == nullptr and node->get_right() == nullptr) {
//         // Check if the character exists in the frequency array
//         char c = node->get_val();
//         assert(frequency[c] == node->get_freq()); 
//         std::cout << "Leaf node: " << c << " with frequency " << 
//         node->get_freq() << std::endl;
//     }
//     // Recursively check left and right children
//     verifyLeafFrequencies(node->get_left(), frequency);
//     verifyLeafFrequencies(node->get_right(), frequency);
// }

// void testHuffmanTreeBuild() {
//     // Sample frequency array (based on some input text)
//     int frequency[ASCII_SIZE] = {0};
//     frequency['A'] = 5;
//     frequency['B'] = 9;
//     frequency['C'] = 12;
//     frequency['D'] = 13;
//     frequency['E'] = 16;
//     frequency['F'] = 45;
//     std::vector<int> freqVec(frequency, frequency + ASCII_SIZE);
//     // Build the Huffman tree using your build function
//     HuffmanCoder coder;  // Assuming you have a HuffmanCoder class
//     HuffmanTreeNode* root = coder.build(frequency);
//     int totalFrequency = 5 + 9 + 12 + 13 + 16 + 45;
//     assert(root->get_freq() == totalFrequency);
//     // Traverse the tree and verify the leaf frequencies
//     verifyLeafFrequencies(root, freqVec);
//     std::cout << "Test passed!" << std::endl;
// }
// void test_build(){
//     char internalNodeVal = '\0';
//     int frequency[ASCII_SIZE] = {0};
//     frequency['a'] = 5;
//     frequency['b'] = 3;
//     frequency['c'] = 2;
//     frequency['\0'] = 0;
//     frequency['\0'] = 0;
//     HuffmanCoder coder;
//     HuffmanTreeNode *root = new HuffmanTreeNode('\0', 0); 
//     HuffmanTreeNode *leftChild = new HuffmanTreeNode('a', 5); 
//     HuffmanTreeNode *rightChild = new HuffmanTreeNode('\0', 0);
//     HuffmanTreeNode *rightLeft = new HuffmanTreeNode('b', 3); 
//     HuffmanTreeNode *rightRight = new HuffmanTreeNode('c', 2); 
//     root->set_left(leftChild);
//     root->set_right(rightChild);
//     rightChild->set_left(rightLeft);
//     rightChild->set_right(rightRight);
//     printTree(root, internalNodeVal);
//     cout << "built tree" << endl;
//     HuffmanTreeNode *newRoot = coder.build(frequency);
//     printTree(newRoot, internalNodeVal);

// }
// void test_huffman_coding() {
//     // Step 1: Define frequencies for test case
//     int frequency[ASCII_SIZE] = {0};  // Initialize frequency array to 0
//     // Example frequencies (ASCII characters 'a', 'b', 'c', 'd')
//     frequency[(unsigned char)'a'] = 5;
//     frequency[(unsigned char)'b'] = 9;
//     frequency[(unsigned char)'c'] = 12;
//     frequency[(unsigned char)'d'] = 13;
//     // Step 2: Build the Huffman tree
//     HuffmanCoder coder;
//     HuffmanTreeNode* root = coder.build(frequency);
//     // Step 3: Generate the Huffman codes from the tree
//     std::unordered_map<char, std::string> codes;
//     coder.generateCodes(root, "", codes);
//     // Step 4: Print the generated codes for verification
//     std::cout << "Generated Huffman Codes:" << std::endl;
//     for (const auto& pair : codes) {
//         std::cout << pair.first << ": " << pair.second << std::endl;
//     }
//     assert(codes['a'] == "00");
//     assert(codes['b'] == "01");
//     assert(codes['c'] == "10");
//     assert(codes['d'] == "11");
//     std::cout << "Test passed: Huffman codes are correct!" << std::endl;
//     // Clean up dynamically allocated memory
//     delete root;
// }

void test_decoder(){
    HuffmanCoder coder;
    try {
        coder.decoder("penis.txt", "vagina.txt");
    }catch(runtime_error e) {
        cout << e.what() << endl;
    }
    
}
void test_encoder(){
    HuffmanCoder coder;
    try {
        coder.encoder("penis.txt", "vagina.txt");
    }catch(runtime_error e) {
        cout << e.what() << endl;
    }
    
}
void empty_encoder(){
    HuffmanCoder coder;
    try {
        coder.encoder("empty.txt", "MyEmpty.out");
    }catch(runtime_error e) {
        cout << e.what() << endl;
    }
    
}
// void shakespeare_encoder(){
//     HuffmanCoder coder;
//     try {
//         coder.encoder("works_of_shakespeare.txt", "MyShake.out");
//         assert(true);
//     }catch(runtime_error e) {
//         cout << e.what() << endl;
//     }
    
// }