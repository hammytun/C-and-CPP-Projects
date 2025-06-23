/*
 *  RPNcalc.h
 *  Harrison Tun
 *  CalcYouLater
 *  10/11/24
 *
 *  Interface for RPN calculator
 *
 */
#include "DatumStack.h"
#include "parser.h"
#include <iostream>
#include <string>
using namespace std;

#ifndef RPNCALC_H
#define RPNCALC_H

class RPNCalc{

    public:
        RPNCalc();
        ~RPNCalc();
        void run();

    private:
        //stack for RPNcalc
        DatumStack stack;
        //Command Handlers
        void handleCommand(string &command, istream &input);
        void performArithmetic(string &op);
        void performComparison(string &op);
        void handleExec();
        void handleIf();
        void handleFile();

        //helper functions
        void pushDatum(string &token);
        void popTwoForOp(Datum &a, Datum &b);
        void simpleCommands(string &command);
        void handleStackOps(string &command);
        void handleNot();
        void handleSwap();
        //function for debugging/testing stack
        void printStack() const;
};

#endif