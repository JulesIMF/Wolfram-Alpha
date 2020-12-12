/*
Copyright (c) 2020  MIPT

Module Name:
    Vertex.h

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#ifndef WOLFRAM_VERTEX
#define WOLFRAM_VERTEX
#include "StringsHolder.h"


enum class VertexType
{
    CONSTANT,
    VARIABLE,
    OPERATION,
};


enum Operation
{
    ADD     = '+',
    SUB     = '_',
    MUL     = '*',
    DIV     = '/',
    LOG     = 'l',
    POW     = '^',
    SIN     = 's',
    COS     = 'c',
    EXP     = 'e',
    ASN     = 'S',
    ACS     = 'C',
    SQR     = 'r',
};


class Vertex
{
public:
    Vertex*         left        (void);
    Vertex*         right       (void);
    Vertex*         parent      (void);
    VertexType      type        (void) const;
    int             number      (void) const;
    bool            isLeaf      (void) const;
    void            setLeft     (Vertex* newLeft);
    void            setType     (VertexType type);
    void            setNumber   (int number);
    void            setRight    (Vertex* newRight);
    void            swapSons    (void);
    void            cutLeft     (void);
    void            cutRight    (void);
    
    Vertex(void) = delete;
    static Vertex* newVertex(VertexType type,
                             int number  = 0,
                             Vertex* parent = nullptr, 
                             Vertex* left   = nullptr,
                             Vertex* right  = nullptr);

    static void    deleteVertex(Vertex* vertex);

    static Vertex* makeCopy(Vertex const* vertex, Vertex* parent = nullptr);

protected:
    Vertex
        * left_,
        * right_,
        * parent_;
    
    VertexType  type_;
    int         number_;
};
#endif //!WOLFRAM_VERTEX