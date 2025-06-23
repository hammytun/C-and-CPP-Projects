/*
 *  DatumStack.cpp
 *  Harrison Tun
 *  CalcYouLater
 *  10/11/24
 *
 *  Implementation of DatumStack. Handles the function that DatumStack will 
 *  use in RPN calc
 *
 */
#include "DatumStack.h"

/*
 * name:      DatumStack( )
 * purpose:   DatumStack constructor
 * arguments: NADA
 * returns:   DatumStack object 
 * effects:   none
 */
DatumStack::DatumStack(){}
/*
 * name:      DatumStack( )
 * purpose:   Initializes stack with Datum objects
 * arguments: Datum object array and a size of the array
 * returns:   DatumStack object 
 * effects:   none
 */
DatumStack::DatumStack(Datum data[], int size){
    for(int i = 0; i < size; i++){
        stack.push_back(data[i]);
    }
}
/*
 * name:      ~DatumStack( )
 * purpose:   DatumStack destructor
 * arguments: NADA
 * returns:   DatumStack object 
 * effects:   none
 */
DatumStack::~DatumStack(){}
/*
 * name:      size( )
 * purpose:   gets the size of the stack
 * arguments: NADA
 * returns:   size of stack 
 * effects:   none
 */
int DatumStack::size() const {
    return stack.size();
}
/*
 * name:      isEmpty( )
 * purpose:   Returns true if the stack is empty
 * arguments: NADA
 * returns:   bool
 * effects:   none
 */
bool DatumStack::isEmpty() const {
    return(stack.size() == 0);
}
/*
 * name:      top( )
 * purpose:   returns the top element of the stack
 * arguments: NADA
 * returns:   Datum object
 * effects:   none
 */
Datum DatumStack::top() const {
    if(isEmpty()){
        throw runtime_error("empty_stack");
    }
    return(stack.back());
}
/*
 * name:      pop( )
 * purpose:   pops the top element off the stack
 * arguments: NADA
 * returns:   nada
 * effects:   none
 */
void DatumStack::pop() {
    if(isEmpty()){
        throw runtime_error("empty_stack");
    }
    stack.pop_back();
}
/*
 * name:      push( )
 * purpose:   pushes element onto the stack
 * arguments: NADA
 * returns:   nada
 * effects:   none
 */
void DatumStack::push(Datum d){
    stack.push_back(d);
}
/*
 * name:      clear( )
 * purpose:   empties the stack of all elements
 * arguments: NADA
 * returns:   nada
 * effects:   none
 */
void DatumStack::clear(){
    stack.clear();
}