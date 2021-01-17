!/bin/bash

lex lexer.l; g++ lex.yy.c ll1.cpp -std=c++11 -o ll1_parse;

./ll1_parse < test_case1.txt
./ll1_parse < test_case2.txt
./ll1_parse < test_case3.txt
