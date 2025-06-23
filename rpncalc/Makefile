###
### Makefile for CalcYouLater Project
###
### Author:  YOUR NAME HERE

CXX      = clang++
CXXFLAGS = -g3 -Wall -Wextra -Wpedantic -Wshadow
LDFLAGS  = -g3 

##
##  --- Delete these comments and put appropriate comments in ---
##
## Put your variables and rules here. You can use previous Makefiles
## as examples, and you can refer to the "make" documentation on the
## course Reference page. Note, you do NOT need a rule for Datum.o
## as that (pre-compiled) object file is provided to you.
##
##  --- Delete these comments and put appropriate comments in ---
CalcYouLater: main.o RPNCalc.o DatumStack.o parser.o Datum.o
	$(CXX) $(LDFLAGS) $^ -o $@

main.o: main.cpp RPNCalc.h DatumStack.h parser.h Datum.h
	$(CXX) $(LDFLAGS) -c main.cpp

RPNCalc.o: RPNCalc.cpp RPNCalc.h DatumStack.h parser.h Datum.h
	$(CXX) $(LDFLAGS) -c RPNCalc.cpp

DatumStack.o: DatumStack.h DatumStack.cpp Datum.h 
	$(CXX) $(CXXFLAGS) -c DatumStack.cpp

parser.o: parser.h parser.cpp
	$(CXX) $(LDFLAGS) -c parser.cpp

unit_test: unit_test_driver.o DatumStack.o Datum.o parser.o RPNCalc.o
	$(CXX) $(CXXFLAGS) $^

## Rules for building an executable from Datum+vector_example.cpp
## This is useful in seeing the behavior of Datum::toString()
Datum+vector_example.o: Datum+vector_example.cpp Datum.h

Datum+vector_example: Datum+vector_example.o Datum.o
	${CXX} ${LDFLAGS} -o $@ $^

##
## Here is a special rule that removes all .o files besides the provided one
## (Datum.o), all temporary files (ending with ~), and 
## a.out produced by running unit_test. First, we find all .o files 
## that are not provided files, then we run rm -f on each found file 
## using the -exec find flag. Second, we delete the temporary files
## and a.out. @ is used to suppress stdout.
## 
## You do not need to modify or further comment this rule!
##
clean:
	@find . -type f \( \
	-name '*.o' ! -name 'Datum.o' \
	\) -exec rm -f {} \;
	@rm -f *~ a.out

##
## Here is a rule for compiling .cyl files for use in your testing
##
%.cylc:  %.cyl
	./cylc $^
