/*
Copyright (c) 2020  MIPT

Module Name:
    Derivative.cpp

Abstract:    
    Реализует взятие производной

Author:
    JulesIMF

Last Edit:
    29.11.2020 21:28

Edit Notes:
    
*/

#include <cassert>
#include "Vertex.h"
#include "Insert.h"
#include "ErrorMessage.h"
#include "Latex.h"

#define DERIVATIVE(vertex) getDerivative(file, vertex, variable)
#define COPY(vertex)       Vertex::makeCopy(vertex)
#define LEFT               vertex->left()
#define RIGHT              vertex->right()
#define SUPERPOSITION(var) NEW_MUL(var, DERIVATIVE(LEFT))

Vertex* getDerivative(FILE* file, Vertex* vertex, char variable)
{
    Vertex* derivative = nullptr;
    if (vertex->type() == VertexType::CONSTANT)
        derivative = NEW_VAL(0);
    
    else if (vertex->type() == VertexType::VARIABLE)
        derivative = NEW_VAL(vertex->number() == variable);

    //Операции

    else switch (vertex->number())
    {
    case ADD:
        derivative = NEW_ADD(DERIVATIVE(LEFT), DERIVATIVE(RIGHT));
        break;

    case SUB:
        derivative = NEW_SUB(DERIVATIVE(LEFT), DERIVATIVE(RIGHT));
        break;

    case MUL:
        derivative = NEW_ADD(
            NEW_MUL(DERIVATIVE(LEFT), COPY(RIGHT)), 
            NEW_MUL(COPY(LEFT), DERIVATIVE(RIGHT))
        );

    case DIV:
        derivative = NEW_DIV(
            NEW_SUB(
                NEW_MUL(DERIVATIVE(LEFT), COPY(RIGHT)),
                NEW_MUL(COPY(LEFT), DERIVATIVE(RIGHT))
            ),
            NEW_POW(
                COPY(RIGHT),
                NEW_VAL(2)
            )
        );
        break;

    case LOG:
        derivative = NEW_DIV(
            DERIVATIVE(LEFT),
            COPY(LEFT)
        );
        break;

    case POW:
        assert(LEFT);
        assert(RIGHT);

        //a^b
        if (RIGHT->type() == VertexType::CONSTANT && LEFT->type() == VertexType::CONSTANT)
        {
            derivative = NEW_VAL(0);
            break;
        }

        //x^a
        if (RIGHT->type() == VertexType::CONSTANT)
        {
            derivative = NEW_MUL(
                DERIVATIVE(LEFT),

                NEW_MUL(
                    COPY(RIGHT),

                    NEW_POW(
                        COPY(LEFT),
                        NEW_VAL(RIGHT->number() - 1)
                    )
                )
            );
            break;
        }

        //a^x
        if (LEFT->type() == VertexType::CONSTANT)
        {
            derivative = NEW_MUL(
                DERIVATIVE(RIGHT),
                NEW_MUL(
                    NEW_LOG(COPY(LEFT)),

                    COPY(RIGHT)
                ));
            break;
        }

        //x^x
        derivative = NEW_MUL(
            NEW_ADD(
                NEW_MUL(DERIVATIVE(RIGHT), NEW_LOG(COPY(LEFT))),
                NEW_DIV(COPY(RIGHT), COPY(LEFT))
            ),
            COPY(vertex)
        );
        break;

    case SIN:
        derivative = SUPERPOSITION(NEW_COS(COPY(LEFT)));
        break;

    case COS:
        derivative = SUPERPOSITION(NEW_MUL(NEW_VAL(-1), NEW_SIN(COPY(LEFT))));
        break;
        
        
    case EXP:
        derivative = SUPERPOSITION(COPY(vertex));
        break;

    case SQR:
        derivative = NEW_DIV(
            DERIVATIVE(LEFT),

            NEW_MUL(
                NEW_VAL(2),
                NEW_SQR(LEFT)
            )
        );
        break;

    case ASN:
        derivative = NEW_DIV(
            DERIVATIVE(LEFT),

            NEW_SQR(NEW_SUB(
                NEW_VAL(1),
                NEW_POW(COPY(LEFT), NEW_VAL(2))
            ))
        );
        break;

    case ACS:
        derivative = NEW_DIV(
            NEW_MUL(NEW_VAL(-1), DERIVATIVE(LEFT)),

            NEW_SQR(NEW_SUB(
                NEW_VAL(1),
                NEW_POW(COPY(LEFT), NEW_VAL(2))
            ))
        );
        break;

    default:
        errorMessage("unknown operator: \"%c\"", vertex->number());
        abort();
    }

    hereWeDifferentiate(file);
    putDerivativeToLatex(vertex, derivative, file, variable);
    return derivative;
}