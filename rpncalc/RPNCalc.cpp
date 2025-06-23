/*
 *  RPNcalc.cpp
 *  Harrison Tun
 *  CalcYouLater
 *  10/19/24
 *
 *  Implementation for RPN calculator. Handles the execution of the RPN 
 *  calculator.
 *
 */

#include "RPNCalc.h"
#include <stdexcept>

// These includes are necessary for got_int( )
#include <string>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
using namespace std;

/*
 * Name: got_int 
 * Purpose: determines if a string contains an integer and loads said
 *           integer into an integer variable whose address has been
 *           passed to this function
 * Parameters: a string to check if it contains an integer as well as 
 *              a pointer to an integer to load in the event an 
 *              integer can be parsed from said string
 * Returns: a boolean where true indicates an integer was successfully
 *           parsed from the provided string (and therefore the
 *           addressed integer has been loaded) and false indicates 
 *           an integer could not be read 
 * Effects: the integer pointed to by the passed pointer will be updated
 *           with the integer parsed from the string in the event true
 *           is returned (otherwise nothing happens)
 * Author: Mark A. Sheldon, Tufts University, Fall 2016
 * Note to students: you do not have to modify this comment!
 */
bool got_int(string s, int *resultp) {
    /* Holds the first non-whitespace character after the integer */
    char extra;
    return sscanf(s.c_str(), " %d %c", resultp, &extra) == 1;
}
/*
 * name:      RPNCalc( )
 * purpose:   Default constructor
 * arguments: NADA
 * returns:   NADA
 * effects:   none
 */
RPNCalc::RPNCalc(){}
/*
 * name:      ~RPNCalc( )
 * purpose:   deallocate memory
 * arguments: NADA
 * returns:   NADA
 * effects:   none
 */
RPNCalc::~RPNCalc(){}
/*
 * name:      run( )
 * purpose:   Gets the user inputted command and runs the program. An error
 *            is thrown if the user's input is invalid
 * arguments: NADA
 * returns:   NADA
 * effects:   none
 */
void RPNCalc::run(){
    string command;
    while(cin >> command){ 
        if(command == "quit"){
            cerr << "Thank you for using CalcYouLater.\n";
            return; 
        }
        try{
            handleCommand(command, cin);
        }catch(runtime_error e){ //catch all runtime errors
            //Do not stop program
            cerr << "Error: " << e.what() << "\n";
        }
    }
    cerr << "Thank you for using CalcYouLater.\n";
}
/*
 * name:      handleCommand( )
 * purpose:   handles the user's input and send the input to its respective 
 *            block
 * arguments: Reference to a string and reference to the input stream
 * returns:   NADA
 * effects:   none
 */
void RPNCalc::handleCommand(string &command, istream &input){ 
    if(command == "+" or command == "-" or command == "*" or command == "/" or 
       command == "mod"){
        performArithmetic(command);
    } else if (command == "<" or command == ">" or command == "==" or 
             command == "<=" or command == ">=") {
                performComparison(command);
    } else if (command == "exec") {
        handleExec();
    } else if (command == "if") {
        handleIf();
    } else if(command == "file") {
        handleFile();
    } else if(command == "{") {
        //Parses Rstring of white space
        string parsedRString = parseRString(input);
        stack.push(Datum(parsedRString));
    } else {
        simpleCommands(command);
    }
}
/*
 * name:      performArithmetic( )
 * purpose:   if the user inputs a mathematical operation, this function will
 *            run and handle the operation.
 * arguments: reference to the operation
 * returns:   NADA
 * effects:   pushes an int onto the stack
 */
void RPNCalc::performArithmetic(string &op){
    //top 2 operators on stack get popped off
    Datum b = stack.top(); stack.pop();
    Datum a = stack.top(); stack.pop();
    if(not a.isInt() or not b.isInt()){
        throw runtime_error("datum_not_int");
    } 
    int result;
    //handles arithmetic
    if(op == "+"){result = a.getInt() + b.getInt();}
    else if(op == "-"){result = a.getInt() - b.getInt();}
    else if(op == "*"){result = a.getInt() * b.getInt();}
    else if(op == "/"){
        if(b.getInt() == 0){
            throw runtime_error("division by 0."); //error for div. by 0
        }
        result = a.getInt() / b.getInt();
    }else if(op == "mod"){
        if(b.getInt() == 0){
            throw runtime_error("division by 0."); //error for mod 0
        }
        result = a.getInt() % b.getInt();
    }
    stack.push(Datum(result)); //pushes result onto stack
}
/*
 * name:      performComparison( )
 * purpose:   if the user inputs a comparative operation, this function will
 *            run and handle the operation.
 * arguments: reference to the operation
 * returns:   NADA
 * effects:   pushes a bool onto the stack
 */
void RPNCalc::performComparison(string &op){
    //top 2 elements on stack popped off for operation
    Datum b = stack.top(); stack.pop();
    Datum a = stack.top(); stack.pop();
    bool result;
    if (op == "==") {
        if (a.isInt() and b.isInt()){result = a.getInt() == b.getInt();}
        else if (a.isBool() and b.isBool()) {
            result = a.getBool() == b.getBool(); //compares bool to bool
        } else if (a.isRString() and b.isRString()) {
            result = a.getRString() == b.getRString();
        } else {
            result = false;
        }
    }else if(op == "<" or op == ">" or op == "<=" or op == ">="){
        if(not a.isInt() or not b.isInt()){ //error check
            throw runtime_error("datum_not_int");
        }
        //handle comparison
        if(op == "<"){result = a.getInt() < b.getInt();}
        else if(op == ">"){result = a.getInt() > b.getInt();}
        else if(op == "<="){result = a.getInt() <= b.getInt();}
        else if(op == ">="){result = a.getInt() >= b.getInt();}
    }
    stack.push(Datum(result)); //push result onto stack
}
/*
 * name:      handleExec( )
 * purpose:   if the input is an rstring, exec can be called to handle the 
 *            operation inside the rstring
 * arguments: NADA
 * returns:   NADA
 * effects:   calls handle command with the content inside the rstring
 */
void RPNCalc::handleExec(){
    //pops top element off the stack
    Datum top = stack.top();
    stack.pop();
    if(not top.isRString()){
        throw runtime_error("cannot execute non rstring");
    }
    string rstring = top.getRString(); //converts rstring into string
    //truncades opening and closing brackets of rstring
    istringstream rstringStream(rstring.substr(1, rstring.size() - 2));
    string command;
    while(rstringStream >> command){ //reads in input string stream
        handleCommand(command, rstringStream);
    }
}
/*
 * name:      handleIf( )
 * purpose:   if the input is an rstring, if can be called to handle an 
 *            operation between two cases and pushes a boolean onto the stack
 * arguments: NADA
 * returns:   NADA
 * effects:   calls handle command with the content inside the rstring
 */
void RPNCalc::handleIf(){
    //pops 3 off the stack
    Datum falseCase = stack.top();
    stack.pop();
    Datum trueCase = stack.top();
    stack.pop();
    Datum testCondition = stack.top();
    stack.pop();
    if(not testCondition.isBool()){
        throw runtime_error("expected boolean in if test");
    }
    if(not falseCase.isRString() or not trueCase.isRString()){
        throw runtime_error("expected rstring in if branch");
    }
    string blockToExecute;
    //Determines which case to exec
    if(testCondition.getBool()){
        blockToExecute = trueCase.getRString();
    }else{
        blockToExecute = falseCase.getRString();
    }
    istringstream blockStream(blockToExecute.substr(1, 
                              blockToExecute.size() - 2));
    string command;
    while(blockStream >> command){
        handleCommand(command, blockStream); //reads in case to handle 
    }
}
/*
 * name:      handleFile( )
 * purpose:   if the input is an rstring, file can be called to handle the 
 *            operation inside the rstring which is a file and the file 
 *            content will be performed
 * arguments: NADA
 * returns:   NADA
 * effects:   calls handle command with the content inside the rstring
 */
void RPNCalc::handleFile(){
    Datum fileDatum = stack.top(); //pops top of the stack
    stack.pop();
    if(not fileDatum.isRString()){
        throw runtime_error("file operand not rstring");
    }
    string filename = fileDatum.getRString();
    if(filename.front() == '{'){
        filename = filename.substr(2, filename.size() - 4);
    }
    ifstream file(filename);
    if(not file.is_open()){
        throw runtime_error("Unable to read" + filename);
    }
    string command;
    while(file >> command){
        try{
            handleCommand(command, file);
        } catch(runtime_error e){
            cerr << "Error: " << e.what() << "\n"; //throw error message 
        }
    }
    file.close(); //close file when done
}
/*
 * name:      simpleCommands( )
 * purpose:   prints and calls stack operation functions for the one word 
 *            commands that do not require and rstring 
 * arguments: reference to string
 * returns:   NADA
 * effects:   NADA
 */
void RPNCalc::simpleCommands(string &command){
    if(command == "print"){
        cout << stack.top().toString() << "\n";
    }else if(command == "clear" or command == "dup" or command == "swap" 
             or command == "drop" or command == "not"){
                //handles these commands
                handleStackOps(command);
    }else{
        //pushes values onto stack
        pushDatum(command);
    }
}
/*
 * name:      pushDatum( )
 * purpose:   pushes values that the user inputs onto the stack, outputs an
 *            error if the token is unrecognized
 * arguments: reference to a string
 * returns:   NADA
 * effects:   adds objects to the stack
 */
void RPNCalc::pushDatum(string &token){
    int value;
    if(got_int(token, &value)){
        stack.push(Datum(value)); //push int
    }
    else if(token == "#t"){
        stack.push(Datum(true)); //push #t
    }else if(token == "#f"){
        stack.push(Datum(false)); //push #f
    }
    else{
        cerr << token << ": unimplemented\n"; //prints if not recognized
    }
}
/*
 * name:      handleStackOps( )
 * purpose:   handles all operations that affect the flow of the stack
 * arguments: reference to a string
 * returns:   NADA
 * effects:   manipulates the stack depending on the command
 */
void RPNCalc::handleStackOps(string &command){
    if(command == "clear"){
        stack.clear();
    }else if(command == "not"){
        handleNot();
    }else if(command == "dup"){
        if(stack.size() > 0){
            stack.push(stack.top());
        }else{
            throw runtime_error("empty_stack"); //can't dup with an no elems
        }
    }else if(command == "swap"){
        handleSwap();
    }else if(command == "drop"){
        stack.pop(); //pops top element on the stack
    }
}
/*
 * name:      handleNot( )
 * purpose:   Handles the "not" command which replaces the top element if it
 *            is a boolean with its opposite value
 * arguments: NADA
 * returns:   NADA
 * effects:   changes a value on the stack
 */
void RPNCalc::handleNot(){
    Datum top = stack.top();
    stack.pop();
    if(not top.isBool()){
        throw runtime_error("datum_not_bool");
    }
    //sets bool to opposite value
    bool oppValue = not top.getBool();
    stack.push(Datum(oppValue));
}
/*
 * name:      handleSwap( )
 * purpose:   Handles the "swap" command which switches two elements on the 
 *            stack
 * arguments: NADA
 * returns:   NADA
 * effects:   changes a values on the stack
 */
void RPNCalc::handleSwap(){
    //swaps 
    Datum top1 = stack.top();
    stack.pop();
    Datum top2 = stack.top();
    stack.pop();
    stack.push(top1);
    stack.push(top2);
}
/*
 * name:      printStack( )
 * purpose:   debugging helper function that prints the current elements on 
 *            the stack
 * arguments: NADA
 * returns:   NADA
 * effects:   NADA
 */
void RPNCalc::printStack() const {
    // Copy the stack for safe printing
    DatumStack tempStack = stack;  
    cout << "Current Stack: [";
    while (not tempStack.isEmpty()) {
        cout << tempStack.top().toString();
        tempStack.pop();
        if (not tempStack.isEmpty()) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}