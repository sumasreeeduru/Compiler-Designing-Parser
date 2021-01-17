%{
#define VARIABLE 1
#define ARROW 2
#define PRODUCTIONS 3
#define PIPE 4
#define NEW_LINE 5
#include <iostream>

using namespace std;
%}

%%
[A-Z]           return VARIABLE;
"->"            return ARROW;
[a-zA-Z+x]+       return PRODUCTIONS;
"|"             return PIPE;
"\n"            return NEW_LINE;
[ \t]         ;
.               cout << "unexpected character\n";
%%

int yywrap()
{
    return 1;
}