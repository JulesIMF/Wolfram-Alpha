/*
Copyright (c) 2020  MIPT

Module Name:
    Simplify.cpp

Abstract:    
    Реализует модуль упрощения выражения
Author:
    JulesIMF

Last Edit:
    30.11.2020 1:28

Edit Notes:
    
*/


#include <cmath>
#include "Simplify.h"
#include "Insert.h"
#include "Parsing.h"
#include "Latex.h"

#define ISNUL(vertex) (vertex && vertex->type() == VertexType::CONSTANT  && vertex->number() == 0)
#define ISONE(vertex) (vertex && vertex->type() == VertexType::CONSTANT  && vertex->number() == 1)
#define ISVAL(val, vertex) (vertex && vertex->type() == VertexType::CONSTANT  && vertex->number() == val)

#define ISDIV(vertex) (vertex && vertex->type() == VertexType::OPERATION && vertex->number() == DIV)

#define IS(oper, vertex) (vertex && vertex->type() == VertexType::OPERATION && vertex->number() == oper)

#define VLEFT   (vertex->left())
#define VRIGHT  (vertex->right())


static bool deepCompare(Vertex* first, Vertex* second)
{
    if (first == nullptr && second == nullptr)
        return true;

    if ((first == nullptr) ^ (second == nullptr))
        return false;

    if(first->type()   != second->type()
       ||
       first->number() != second->number())
        return false;

    return deepCompare(first->left(), second->left()) && deepCompare(first->right(), second->right());
}


static bool convolution(Vertex* vertex)
{
    if (vertex->type() != VertexType::OPERATION)
        return false;

    if (vertex->left() == nullptr
        ||
        vertex->left()->type() != VertexType::CONSTANT
        ||
        vertex->right() == nullptr
        ||
        vertex->right()->type() != VertexType::CONSTANT)
        return false;

    auto left = vertex->left()->number();
    auto right = vertex->right()->number();

    double result = 0.0;

    switch (vertex->number())
    {
    case ADD:
        vertex->setNumber(left + right);
        break;

    case SUB:
        vertex->setNumber(left - right);
        break;

    case MUL:
        vertex->setNumber(left * right);
        break;

    case POW:
        vertex->setNumber(pow(left, right));
        break;

    case DIV:
        if (right && left % right == 0)
        {
            vertex->setNumber(left / right);
            break;
        }

        else
            return false;

    case SQR:
        result = sqrt(left);
        if (left != (int)result * (int)result)
            return false;

        vertex->setNumber(result);
        break;

    case LOG:
        if (left != 1)
            return 0;

        vertex->setNumber(0);
        break;

    default:

        return false;
    }

    //generatePicture(vertex);

    Vertex::deleteVertex(vertex->left());
    Vertex::deleteVertex(vertex->right());
    vertex->setType(VertexType::CONSTANT);
    return true;
}


static bool fourFlooredFrac(Vertex* vertex)
{
    if (vertex->type()            != VertexType::OPERATION 
        || 
        vertex->number()          != DIV
        ||
        vertex->left()->type()    != VertexType::OPERATION
        ||
        vertex->left()->number()  != DIV
        ||
        vertex->right()->type()   != VertexType::OPERATION
        ||
        vertex->right()->number() != DIV
        )
        return false;

    auto newLeft  = NEW_MUL(vertex->left()->left(), vertex->right()->right());
    auto newRight = NEW_MUL(vertex->left()->right(), vertex->right()->left());

    vertex->left()->setLeft(nullptr);
    vertex->left()->setRight(nullptr);
    Vertex::deleteVertex(vertex->left());
    vertex->right()->setLeft(nullptr);
    vertex->right()->setRight(nullptr);
    Vertex::deleteVertex(vertex->right());
    vertex->setLeft(newLeft);
    vertex->setRight(newRight);

    return true;
}


static bool threeFlooredFrac(Vertex* vertex)
{
    if (vertex->type() != VertexType::OPERATION
        ||
        vertex->number() != DIV)
        return false;


    // (a/b)/c = a/(b*c)
    if(ISDIV(vertex->left())
       &&
       !ISDIV(vertex->right())
       )
    {
        auto a = vertex->left()->left(),
             b = vertex->left()->right(),
             c = vertex->right();

        vertex->swapSons();

        vertex->right()->setRight(c);
        vertex->right()->setLeft(b);
        vertex->setLeft(a);
        vertex->right()->setNumber(MUL);
        return true;
    }

    // a/(b/c) = (a*c) / b
    if(ISDIV(vertex->right())
       &&
       !ISDIV(vertex->left())
       )
    {
        auto a = vertex->left(),
             b = vertex->right()->left(),
             c = vertex->right()->right();

        vertex->swapSons();
        vertex->left()->setLeft(a);
        vertex->left()->setNumber(MUL);
        vertex->setRight(b);
        return true;
    }

    return false;
}


static bool nullMultiplication(Vertex* vertex)
{
    if (vertex->type() == VertexType::OPERATION &&
        vertex->number() == MUL)
    {
        if (ISNUL(vertex->left()))
        {
            vertex->cutRight();
            return true;
        }


        if (ISNUL(vertex->right()))
        {
            vertex->cutLeft();
            return true;
        }
    }

    return false;
}


static bool nullDivision(Vertex* vertex)
{
    if (vertex->type() == VertexType::OPERATION &&
        vertex->number() == DIV)
    {
        if (ISNUL(vertex->left()))
        {
            vertex->cutRight();
            return true;
        }
    }

    return false;
}


static bool nullPower(Vertex* vertex)
{
    if (vertex->type() == VertexType::OPERATION &&
        vertex->number() == POW)
    {
        if (ISNUL(vertex->left()))
        {
            vertex->cutRight();
            return true;
        }


        if (ISNUL(vertex->right()))
        {
            vertex->cutLeft();
            vertex->setNumber(1);
            return true;
        }
    }

    if (vertex->type() == VertexType::OPERATION &&
        vertex->number() == LOG)
    {
        if (ISONE(vertex->left()))
        {
            vertex->cutRight();
            vertex->left()->setNumber(0);
            return true;
        }
    }

    return false;
}


static bool idempotent(Vertex* vertex)
{
    if (vertex->type() != VertexType::OPERATION)
        return false;

    switch (vertex->number())
    {
    case ADD:
        if (ISNUL(vertex->left()))
        {
            vertex->cutLeft();
            return true;
        }

    case SUB:
        if (ISNUL(vertex->right()))
        {
            vertex->cutRight();
            return true;
        }

        return false;

    case MUL:
        if (ISONE(vertex->left()))
        {
            vertex->cutLeft();
            return true;
        }

    case DIV:
        if (ISONE(vertex->right()))
        {
            vertex->cutRight();
            return true;
        }

        return false;

    case POW:
        if (ISONE(vertex->right()) || ISONE(vertex->left()))
        {
            vertex->cutRight();
            return true;
        }

        return false;

    default:
        return false;
    }
}


static bool rotateConst(Vertex* vertex)
{
    if (!IS(MUL, vertex))
        return false;

    if(vertex->left() ->type() != VertexType::CONSTANT
       &&
       vertex->right()->type() == VertexType::CONSTANT)
    {
        vertex->swapSons();
        return true;
    }

    if (vertex->left()->type() != VertexType::CONSTANT
        &&
        IS(MUL, vertex->right())
        &&
        vertex->right()->left()->type() == VertexType::CONSTANT)
    {
        auto x = vertex->left();
        auto y = vertex->right()->left();
        vertex->setLeft(y);
        vertex->right()->setLeft(x);
        return true;
    }

    if (vertex->right()->type() != VertexType::CONSTANT
        &&
        IS(MUL, vertex->left())
        &&
        vertex->left()->right()->type() == VertexType::CONSTANT)
    {
        auto x = vertex->right();
        auto y = vertex->left()->right();
        vertex->setRight(y);
        vertex->left()->setRight(x);
        return true;
    }

    return false;
}


static bool reduce(Vertex* vertex)
{
    if (!IS(DIV, vertex))
        return false;

    if (!IS(MUL, vertex->left()))
    {
        if (deepCompare(vertex->left(), vertex->right()))
        {
            Vertex::deleteVertex(vertex->left());
            Vertex::deleteVertex(vertex->right());
            vertex->setType(VertexType::CONSTANT);
            vertex->setNumber(1);
            return true;
        }

        if (!IS(MUL, vertex->right()))
            return false;

        if (deepCompare(vertex->left(), vertex->right()->left()))
        {
            Vertex::deleteVertex(vertex->left()->left());
            Vertex::deleteVertex(vertex->left()->right());
            vertex->left()->setType(VertexType::CONSTANT);
            vertex->left()->setNumber(1);
            vertex->right()->cutLeft();
            return true;
        }

        if (deepCompare(vertex->left(), vertex->right()->right()))
        {
            Vertex::deleteVertex(vertex->left()->left());
            Vertex::deleteVertex(vertex->left()->right());
            vertex->left()->setType(VertexType::CONSTANT);
            vertex->left()->setNumber(1);
            vertex->right()->cutRight();
            return true;
        }
    }

    else
    {
        if (!IS(MUL, vertex->right()))
        {

            if (deepCompare(vertex->right(), vertex->left()->left()))
            {
                Vertex::deleteVertex(vertex->right()->left());
                Vertex::deleteVertex(vertex->right()->right());
                vertex->right()->setType(VertexType::CONSTANT);
                vertex->right()->setNumber(1);
                vertex->left()->cutLeft();
                return true;
            }

            if (deepCompare(vertex->right(), vertex->left()->right()))
            {
                Vertex::deleteVertex(vertex->right()->left());
                Vertex::deleteVertex(vertex->right()->right());
                vertex->right()->setType(VertexType::CONSTANT);
                vertex->right()->setNumber(1);
                vertex->left()->cutRight();
                return true;
            }
        }

        else
        {
            if (deepCompare(vertex->left()->left(), vertex->right()->left()))
            {
                vertex->left()->cutLeft();
                vertex->right()->cutLeft();
                return true;
            }

            if (deepCompare(vertex->left()->right(), vertex->right()->left()))
            {
                vertex->left()->cutRight();
                vertex->right()->cutLeft();
                return true;
            }

            if (deepCompare(vertex->left()->left(), vertex->right()->right()))
            {
                vertex->left()->cutLeft();
                vertex->right()->cutRight();
                return true;
            }

            if (deepCompare(vertex->left()->right(), vertex->right()->right()))
            {
                vertex->left()->cutRight();
                vertex->right()->cutRight();
                return true;
            }
        }
    }

    return false;
}


static bool addToSub(Vertex* vertex)
{
    if (IS(ADD, vertex)
        &&
        IS(MUL, vertex->right())
        &&
        (vertex->right()->left()->type() == VertexType::CONSTANT && vertex->right()->left()->number() < 0)
       )
    {
        vertex->right()->left()->setNumber(-vertex->right()->left()->number());
        vertex->setNumber(SUB);
        return true;    
    }

    return false;
}


static bool subToAdd(Vertex* vertex)
{
    if (IS(SUB, vertex)
        &&
        IS(MUL, vertex->right())
        &&
        (vertex->right()->left()->type() == VertexType::CONSTANT && vertex->right()->left()->number() < 0)
        )
    {
        vertex->right()->left()->setNumber(-vertex->right()->left()->number());
        vertex->setNumber(ADD);
        return true;
    }

    return false;
}


static bool sub0(Vertex* vertex)
{
    if (IS(SUB, vertex)
        &&
        ISNUL(vertex->left()))
    {
        vertex->setNumber(MUL);
        vertex->left()->setNumber(-1);
        return true;
    }

    return false;
}


static bool mulconst(Vertex* vertex)
{
    if (IS(MUL, vertex)
        &&
        IS(MUL, vertex->right())
        &&
        vertex->left()->type() == VertexType::CONSTANT
        &&
        vertex->right()->left()->type() == VertexType::CONSTANT)
    {
        auto val = vertex->right()->left()->number();
        val *= vertex->left()->number();
        vertex->right()->left()->setNumber(val);
        vertex->cutLeft();
        return true;
    }

    return false;
}


//static bool giveSimilarMul(Vertex* vertex)
//{
//    if (IS(ADD, vertex))
//    {
//        if (IS(MUL, vertex->left()) && IS(MUL, vertex->left()))
//        {
//            if(deepCompare(vertex->left()->right(), vertex->right()->right()))
//
//        }
//    }
//}






bool simplify(Vertex* vertex, FILE* file)
{
    if (vertex == nullptr
        ||
        vertex->type() != VertexType::OPERATION)
        return false;

    bool anythingChanged = false;
    int counter = 0;

    do
    {
        anythingChanged  = false;
        anythingChanged |= simplify(vertex->left(), file);
        anythingChanged |= simplify(vertex->right(), file);
        anythingChanged |= convolution(vertex);
        anythingChanged |= idempotent(vertex);
        anythingChanged |= nullMultiplication(vertex);
        anythingChanged |= nullDivision(vertex);
        anythingChanged |= nullPower(vertex);
        anythingChanged |= rotateConst(vertex);
        anythingChanged |= addToSub(vertex);
        anythingChanged |= subToAdd(vertex);
        anythingChanged |= sub0(vertex);
        anythingChanged |= reduce(vertex);
        anythingChanged |= mulconst(vertex);
        anythingChanged |= fourFlooredFrac(vertex);
        anythingChanged |= threeFlooredFrac(vertex);
        
    }
    while(anythingChanged && ++counter);

    if (!!counter)
        tryToSimplify(file, vertex);

    return !!counter;
}
