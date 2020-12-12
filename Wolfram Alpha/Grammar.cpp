/*
Copyright (c) 2020  MIPT

Module Name:
    Grammar.cpp

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    30.11.2020 18:45

Edit Notes:
    
*/
#include <cstring>
#include "Vertex.h"
#include "Insert.h"
#include "Grammar.h"
#include "ErrorMessage.h"


static const int MAX_LENGTH = 20;


/**
 * Убирает пробелы.
 * 
 * \param source преобразуемая строка
 */
static void pseudoLexer(char* source)
{
    bool isEndOfLine = false;
    char* nextSymbol = source;
    while (!isEndOfLine)
    {
        while (!isEndOfLine)
        {
            while (*source == ' ' || *source == '\t')
                source++;
            isEndOfLine = !(*source);
            *(nextSymbol++) = *(source++);
        }
    }
}


static int countUntilSlashN(char const* place)
{
    int ans = 0;
    while (*place != '\0' &&
        *place != '\n' &&
        ans != MAX_LENGTH) ans++, place++;

    return ans;
}


static void throwParsingException(char const* message, char const* place)
{
    errorMessage("%s\n"
        "\n\t%.*s\n\t^~~\n",
        message,
        countUntilSlashN(place),
        place);

    system("pause");

    exit(-1);
}

void Parser::expected(char const* tok, int len, int advance)
{
    if (strncmp(tok, expr, len))
        throwParsingException("expected different symbol", expr);

    expr += advance;
}


bool Parser::possible(char const* tok, int len, int advance)
{
    if (strncmp(tok, expr, len))
        return false;

    expr += advance;
    return true;
}


Vertex* Parser::getG(char* source, bool table_[])
{
    pseudoLexer(source);
    printf("expr = %s\n", source);
    expr = source;
    this->table = table_;
    if (expr == nullptr)
        return nullptr;

    auto tree = getE();
    expected("\0");
    expr++;

    return tree;
}

Vertex* Parser::getE(void)
{
    auto tree = getT();

    while(*expr == '+'
          ||
          *expr == '-')
    {
        char oper = *expr;
        expr++;

        auto newRight = getT();
        tree = Vertex::newVertex
        (
            VertexType::OPERATION,
            (oper == '+') ? ADD : SUB,
            nullptr,
            tree,
            newRight
        );
    }

    return tree;
}

Vertex* Parser::getT(void)
{
    auto tree = getM();

    while(*expr == '*'
          ||
          *expr == '/')
    {
        char oper = *expr;
        expr++;

        auto newRight = getM();
        tree = Vertex::newVertex
        (
            VertexType::OPERATION,
            (oper == '*') ? MUL : DIV,
            nullptr,
            tree,
            newRight
        );
    }

    return tree;
}


Vertex* Parser::getM(void)
{
    auto tree = getP();

    while (*expr == '^')
    {
        expr++;
        auto newRight = getM();
        tree = Vertex::newVertex
        (
            VertexType::OPERATION,
            POW,
            nullptr,
            tree,
            newRight
        );
    }

    return tree;
}


Vertex* Parser::getP(void)
{
    static Operation   O[] = {  SIN ,  COS ,  SQR  ,  ASN  ,  ACS  ,  EXP ,  LOG  };
    static char const* F[] = { "sin", "cos", "sqrt", "asin", "acos", "exp", "log" };
    static int  const  L[] = {  3   ,  3   ,  4    ,  4    ,  4    ,  3   ,  3    };

    constexpr int nFunctions = sizeof(O) / sizeof(Operation);
    char currentFnc = '\0';

    for (int i = 0; i != nFunctions; i++)
    {
        if (possible(F[i], L[i], L[i]))
        {
            currentFnc = O[i];
            expected("(");
            break;
        }
    }
    if (possible("("))
    {
        auto tree = getE();
        if (currentFnc != '\0')
            tree = Vertex::newVertex(
                VertexType::OPERATION,
                currentFnc,
                nullptr,
                tree,
                NEW_VAL(0)
            );

        expected(")", 1, 1);

        return tree;
    }

    if (isalpha(*expr))
    {
        char var = *expr;
        expr++;
        
        table[var] = true;

        return NEW_VAR(var);
    }

    return getN();
}

Vertex* Parser::getN(void)
{
    bool isNegative = possible("-");
    int val = 0;
    if (!isdigit(*expr))
        throwParsingException("expected digit", expr);

    while (isdigit(*expr))
    {
        val = val * 10 + *expr - '0';
        expr++;
    }

    if (isNegative)
        val *= -1;

    return NEW_VAL(val);
}
