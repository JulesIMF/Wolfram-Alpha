/*
Copyright (c) 2020  MIPT

Module Name:
    Grammar.h

Abstract:    
    Реализует парсер грамматики алгебраических выражений.

    Описание грамматки:

    G ::= E '\0'
    E ::= T {['+' '-'] T}*
    T ::= M {['*' '\'] M}*
    M ::= P {['^']     P} 
    P ::= ['sin' 'cos' 'sqrt' 'asin' 'acos', 'exp', 'log']? '('E')' | ['a' - 'z', 'A' - 'Z'] | N
    N ::= ['-']? ['0' - '9']+

Author:
    JulesIMF

Last Edit:
    30.11.2020 18:36

Edit Notes:
    
*/

#include "Vertex.h"

class Parser
{
private:
    char const* expr = nullptr;
    bool* table;
    void expected(char const* tok, int len = 1, int advance = 0);
    bool possible(char const* tok, int len = 1, int advance = 1);
    
    Vertex* getE(void);
    Vertex* getT(void);
    Vertex* getM(void);
    Vertex* getP(void);
    Vertex* getN(void);

public:
    Vertex* getG(char* source, bool table[]);
};
