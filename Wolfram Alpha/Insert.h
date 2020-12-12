/*
Copyright (c) 2020  MIPT

Module Name:
    Insert.h

Abstract:    
    Макросы для вставки новых элементов

Author:
    JulesIMF

Last Edit:
    29.11.2020 21:09

Edit Notes:
    
*/

#ifndef WOLFRAM_INSERT
#define WOLFRAM_INSERT

/*
enum Operation
{
    ADD     = '+',
    SUB     = '-',
    MUL     = '*',
    DIV     = '/',
    LOG     = 'l',
    SIN     = 's',
    COS     = 'c',
    EXP     = 'e',
    ASN     = 'S',
    ACS     = 'C',
    POW     = '^',
    SQR     = 'r',
};
*/

#include "Vertex.h"

#define NEW_VAL(value)          Vertex::newVertex(VertexType::CONSTANT, value, nullptr, nullptr)
#define NEW_VAR(value)          Vertex::newVertex(VertexType::VARIABLE, value, nullptr, nullptr)

#define NEW_ADD(left, right)    Vertex::newVertex(VertexType::OPERATION, ADD, nullptr, left, right)
#define NEW_SUB(left, right)    Vertex::newVertex(VertexType::OPERATION, SUB, nullptr, left, right)
#define NEW_MUL(left, right)    Vertex::newVertex(VertexType::OPERATION, MUL, nullptr, left, right)
#define NEW_DIV(left, right)    Vertex::newVertex(VertexType::OPERATION, DIV, nullptr, left, right)
#define NEW_LOG(left)           Vertex::newVertex(VertexType::OPERATION, LOG, nullptr, left, NEW_VAL(0))
#define NEW_SIN(left)           Vertex::newVertex(VertexType::OPERATION, SIN, nullptr, left, NEW_VAL(0))
#define NEW_COS(left)           Vertex::newVertex(VertexType::OPERATION, COS, nullptr, left, NEW_VAL(0))
#define NEW_EXP(left)           Vertex::newVertex(VertexType::OPERATION, EXP, nullptr, left, NEW_VAL(0))
#define NEW_ASN(left)           Vertex::newVertex(VertexType::OPERATION, ASN, nullptr, left, NEW_VAL(0))
#define NEW_ACS(left)           Vertex::newVertex(VertexType::OPERATION, ACS, nullptr, left, NEW_VAL(0))
#define NEW_POW(left, right)    Vertex::newVertex(VertexType::OPERATION, POW, nullptr, left, right)
#define NEW_SQR(left)           Vertex::newVertex(VertexType::OPERATION, SQR, nullptr, left, NEW_VAL(0))


#endif // !WOLFRAM_INSERT

