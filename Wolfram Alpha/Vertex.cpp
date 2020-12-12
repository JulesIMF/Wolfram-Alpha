/*
Copyright (c) 2020  MIPT

Module Name:
    Vertex.cpp

Abstract:    
    Реализует класс "Vertex" - вершины дерева

Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include "Vertex.h"



VertexType Vertex::type(void) const
{
    return type_;
}


int Vertex::number(void) const
{
    return number_;
}


Vertex* Vertex::left(void)
{
    return left_;
}


Vertex* Vertex::right(void)
{
    return right_;
}


Vertex* Vertex::parent(void)
{
    return parent_;
}


bool Vertex::isLeaf(void) const
{
    assert( !( (left_ == 0) ^ (right_ == 0) ) );
    return left_ == nullptr;
}



void Vertex::setLeft(Vertex* newLeft)
{
    left_ = newLeft;
}


void Vertex::setRight(Vertex* newRight)
{
    right_ = newRight;
}


void Vertex::swapSons(void)
{
    auto tmp = right_;
    right_   = left_;
    left_    = tmp;
}


Vertex* Vertex::newVertex(VertexType type,
                          int number,
                          Vertex* parent,
                          Vertex* left,
                          Vertex* right)
{
    Vertex* vertex = (Vertex*)calloc(1, sizeof(Vertex));
    assert(vertex);
    vertex->parent_ = parent;
    vertex->left_   = left;
    vertex->right_  = right;
    vertex->type_   = type;
    vertex->number_ = number;
    return vertex;
}


void Vertex::setType(VertexType type)
{
    type_ = type;
}


void Vertex::setNumber(int number) 
{
    number_ = number;
}


void Vertex::cutLeft(void)
{
    deleteVertex(left_);
    left_ = nullptr;
    auto oldRight = right_;

    if (oldRight == nullptr)
        return;

    *this = *oldRight;

    free(oldRight);
}


void Vertex::cutRight(void)
{
    deleteVertex(right_);
    right_ = nullptr;
    auto oldLeft = left_;

    if (oldLeft == nullptr)
        return;

    *this = *oldLeft;

    free(oldLeft);
}


void Vertex::deleteVertex(Vertex* vertex)
{
    if (vertex == nullptr)
        return;


    //printf("Trying to delete vertex %p : TYPE = %d, NUM = %d (%c)\n", vertex, vertex->type_, vertex->number_, vertex->number_);

    if (vertex->type_ == VertexType::OPERATION)
    {
        deleteVertex(vertex->left_);
        deleteVertex(vertex->right_);
    }
    free(vertex);
}


Vertex* Vertex::makeCopy(Vertex const* vertex, Vertex* parent)
{
    if (vertex == nullptr)
        return nullptr;

    auto copy = newVertex(
        vertex->type_,
        vertex->number_,
        parent
    );

    copy->left_  = makeCopy(vertex->left_,  copy);
    copy->right_ = makeCopy(vertex->right_, copy);
    return copy;
}
