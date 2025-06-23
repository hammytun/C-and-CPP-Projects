#ifndef __PARSER_H 
#define __PARSER_H

/*
 *Harrison Tun
  10/7/24
  parser.h
  CalcYouLater
  Function declarations for functions in parser.cpp
 */

#include <string>
using namespace std;
//rstring interface
string parseRString(istream &input);
string collapseWhiteSpace(string &str);

#endif 
