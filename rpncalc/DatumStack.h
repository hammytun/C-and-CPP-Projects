/*
 *  DatumStack.h
 *  Harrison Tun
 *  CalcYouLater
 *  10/11/24
 *
 *  Interface for DatumStack
 *
 */

#ifndef DATUMSTACK_H
#define DATUMSTACK_H

#include "Datum.h"
#include <vector>
#include <stdexcept>
using namespace std;
//DatumStack Interface
class DatumStack{
    public:
        DatumStack();
        DatumStack(Datum data[], int size);
        ~DatumStack(); //if needed

        bool isEmpty() const;
        void clear();
        int size() const;
        Datum top() const;
        void pop();
        void push(Datum d);
    private:
        vector<Datum> stack;
};

#endif
