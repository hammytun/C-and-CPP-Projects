/*
 *Harrison Tun
  10/7/24
  parser.cpp
  CalcYouLater
  Implementation of parser.h. Handles the parsing of an rstring and elimating
  whitespace within the string.
 */
#include "parser.h"
#include <sstream>
#include <stdexcept>
#include <string>
#include <iostream>
using namespace std;


/*
 * name:      CollapseWhiteSpace( )
 * purpose:   collapses consecutive spaces, newlines, and tabs into a 1 space
 * arguments: a reference to a string
 * returns:   A new string that has all of its spaces condensed
 * effects:   none
 */
string collapseWhiteSpace(string &str){
    ostringstream oss;
    bool inWhiteSpace = false;
    for(size_t i = 0; i < str.size(); i++){
        char ch = str[i];
        //check for whitespace
        if(ch == ' ' or ch == '\n' or ch == '\t'){
            if(not inWhiteSpace){
                oss << ' ';
                //flag update
                inWhiteSpace = true;
            }
        }else{
            //print character
            oss << ch;
            inWhiteSpace = false;
        }
    }
    return oss.str();
}
/*
 * name:      ParseRstring( )
 * purpose:   Parses an input stream to an rstring that starts and ends with
 *            curly braces. The content inside within the braces are parsed 
 *            and returned as a string with its whitespace collapsed
 * arguments: a reference to an input stream
 * returns:   Returns a parsed string with no whitespace
 * effects:   none
 */
string parseRString(istream &input) {
    ostringstream oss;
    int braceCount = 1;
    bool parsingComplete = false;
    //assume first bracket has been read
    oss << "{ ";
    char ch;
    while(input.get(ch) and not parsingComplete){
        oss << ch;
        if(ch == '{'){
            braceCount++;
        } else if(ch == '}'){
            braceCount--;
        }if(braceCount == 0){
            parsingComplete = true;
        }
    }
    if(braceCount != 0){
        throw runtime_error("Unmatched braces in rstring");
    }
    string rstring = oss.str();
    return collapseWhiteSpace(rstring);
}
