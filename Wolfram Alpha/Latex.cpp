/*
Copyright (c) 2020  MIPT

Module Name:
    Latex.cpp

Abstract:    
    ������ ��� ������ ��������� � ������� Latex

Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include "Vertex.h"
#include "Latex.h"
#include "ErrorMessage.h"

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

static void putVertexToLatex(Vertex* vertex, FILE* file);

static bool isAddSub(Vertex* vertex)
{
    return (vertex) && vertex->type() == VertexType::OPERATION &&
           (
               vertex->number() == ADD ||
               vertex->number() == SUB
           );
}


static bool isComplex(Vertex* vertex)
{
    return (vertex &&
        vertex->type() != VertexType::CONSTANT &&
        vertex->type() != VertexType::VARIABLE);
}


//-------------------------------------------------------
//-------------������������ ���������--------------------
//-------------------------------------------------------


static void putConstToLatex(Vertex* vertex, FILE* file)
{
    fprintf(file, "%d", vertex->number());
}


static void putVarToLatex(Vertex* vertex, FILE* file)
{
    fprintf(file, "%c", vertex->number());
}


//-------------------------------------------------------
//-----------------------��������------------------------
//-------------------------------------------------------


static void putAddToLatex(Vertex* vertex, FILE* file)
{
    putVertexToLatex(vertex->left(), file);
    fprintf(file, " + ");
    putVertexToLatex(vertex->right(), file);
}


static void putSubToLatex(Vertex* vertex, FILE* file)
{
    putVertexToLatex(vertex->left(), file);
    fprintf(file, " - ");
    if (vertex->right() && vertex->right()->type() == VertexType::CONSTANT && vertex->right()->number() < 0)
        fprintf(file, "\\left(");
    putVertexToLatex(vertex->right(), file);
    if (vertex->right() && vertex->right()->type() == VertexType::CONSTANT && vertex->right()->number() < 0)
        fprintf(file, "\\right)");
}


static void putMulToLatex(Vertex* vertex, FILE* file)
{
    assert(vertex->left());
    if (vertex->left()->type() == VertexType::CONSTANT && vertex->left()->number() == -1)
        fprintf(file, "-");

    else
    {
        if (isAddSub(vertex->left())) fprintf(file, "\\left(");
        putVertexToLatex(vertex->left(), file);
        if (isAddSub(vertex->left())) fprintf(file, "\\right)");
    }

    if(vertex->left() && 
        (vertex->left()->type() != VertexType::CONSTANT  || 
         vertex->right()->type() == VertexType::CONSTANT ||
         vertex->right()->type() == VertexType::OPERATION &&
            vertex->right()->left()->type() == VertexType::CONSTANT))
        fprintf(file, " \\cdot ");

    if (isAddSub(vertex->right())) fprintf(file, "\\left(");
    putVertexToLatex(vertex->right(), file);
    if (isAddSub(vertex->right())) fprintf(file, "\\right)");
}


static void putDivToLatex(Vertex* vertex, FILE* file)
{
    fprintf(file, "\\frac{");
    putVertexToLatex(vertex->left(), file);
    fprintf(file, "}{");
    putVertexToLatex(vertex->right(), file);
    fprintf(file, "}");
}


static void putPowToLatex(Vertex* vertex, FILE* file)
{
    if (isComplex(vertex->left()))
        fprintf(file, "\\left(");

    putVertexToLatex(vertex->left(), file);

    if (isComplex(vertex->left()))
        fprintf(file, "\\right)");

    fprintf(file, "^{");
    putVertexToLatex(vertex->right(), file);
    fprintf(file, "}");
}

static void putFncToLatex(Vertex* vertex, FILE* file, char const* function)
{
    fprintf(file, "%s{", function);

    if (isComplex(vertex->left()))
        fprintf(file, "\\left(");

    putVertexToLatex(vertex->left(), file);

    if (isComplex(vertex->left()))
        fprintf(file, "\\right)");

    fprintf(file, "}");
}


static void putSinToLatex(Vertex* vertex, FILE* file)
{
    putFncToLatex(vertex, file, "\\sin");
}


static void putCosToLatex(Vertex* vertex, FILE* file)
{
    putFncToLatex(vertex, file, "\\cos");
}


static void putArcsinToLatex(Vertex* vertex, FILE* file)
{
    putFncToLatex(vertex, file, "\\arcsin");
}


static void putArccosToLatex(Vertex* vertex, FILE* file)
{
    putFncToLatex(vertex, file, "\\arccos");
}


static void putSqrtToLatex(Vertex* vertex, FILE* file)
{
    fprintf(file, "%s{", "\\sqrt");
    putVertexToLatex(vertex->left(), file);
    fprintf(file, "}");
}


static void putLogToLatex(Vertex* vertex, FILE* file)
{
    putFncToLatex(vertex, file, "\\ln");
}


static void putExpToLatex(Vertex* vertex, FILE* file)
{
    if (!isComplex(vertex->left()))
    {
        fprintf(file, "e^");

        if(vertex->left()->type() == VertexType::CONSTANT)
            fprintf(file, "%d", vertex->left()->number());

        else
            fprintf(file, "%c", vertex->left()->number());

        return;
    }       

    putFncToLatex(vertex, file, "exp");
}


static void putVertexToLatex(Vertex* vertex, FILE* file)
{
    assert(vertex);

    if (vertex->type() == VertexType::CONSTANT)
    {
        putConstToLatex(vertex, file);
        return;
    }


    if (vertex->type() == VertexType::VARIABLE)
    {
        putVarToLatex(vertex, file);
        return;
    }

    switch (vertex->number())
    {
    case ADD:
        putAddToLatex(vertex, file);
        return;
    case SUB:
        putSubToLatex(vertex, file);
        return;
    case MUL:
        putMulToLatex(vertex, file);
        return;
    case DIV:
        putDivToLatex(vertex, file);
        return;
    case SIN:
        putSinToLatex(vertex, file);
        return;
    case COS:
        putCosToLatex(vertex, file);
        return;
    case ASN:
        putArcsinToLatex(vertex, file);
        return;
    case ACS:
        putArccosToLatex(vertex, file);
        return;
    case LOG:
        putLogToLatex(vertex, file);
        return;
    case EXP:
        putExpToLatex(vertex, file);
        return;
    case SQR:
        putSqrtToLatex(vertex, file);
        return;
    case POW:
        putPowToLatex(vertex, file);
        return;
    default:
        errorMessage("unknown operation \"%c\"", vertex->number());
        abort();
    }
}

void beginMath(FILE* file)
{
    fprintf(file, "\\begin{dmath*}\n");
    //fprintf(file, "\\[\n");
}


void endMath(FILE* file)
{
    //fprintf(file, "\\]\n");
    fprintf(file, "\n\\end{dmath*}\n");
}


void tryToSimplify(FILE* file, Vertex* vertex)
{
    switch (rand() % 3)
    {
    default:
        fprintf(file, "������� ��������� ��������� ��� *��� �� �����* ���������. ��������� (�����-���) ��� ���:\n");
        break;
    case 1:
        fprintf(file, "��������� �������, ������� ���������, ����� �� ���-�� ��� *��� �� �����* ��������� �������� �������? �����, ������� �������:\n");
        break;
    case 2:
        fprintf(file, "�� ��� ��� *��� �� �����* ���������, ��������, ����� �������� �������, � ������ ��� ���:\n");
        break;
    }

    putToLatex(vertex, file);
}


void atFirst(FILE* file, Vertex* vertex)
{
    switch (rand() % 3)
    {
    default:
        fprintf(file, "��, ��� �� �����:\n");
        break;
    case 1:
        fprintf(file, "������� ��������� ��� �� ��� �����:\n");
        break;
    case 2:
        fprintf(file, "���, �������, ������� ��������� ����:\n");
        break;
    }

    putToLatex(vertex, file);
}



void hereWeDifferentiate(FILE* file)
{
    switch (rand() % 3)
    {
    default:
        fprintf(file, "���� �����, ��� ��� ��� ����������� �����?\\newline\n");
        break;
    case 1:
        fprintf(file, "������ ��� ������, ����������� ����� ��� �����:\\newline\n");
        break;
    case 2:
        fprintf(file, "����� � ����� �����������? �� ��� �����:\\newline\n");
        break;
    }
}


void hereWeGet(FILE* file)
{
    switch (rand() % 3)
    {
    default:
        fprintf(file, "��������� ��������� ������� �� ��� � �������� �����������.\\newline\n");
        break;
    case 1:
        fprintf(file, "��, ������, ��������, ��������� ������� ��� �������.\\newline\n");
        break;
    case 2:
        fprintf(file, "�����, ��������� ������� � �������� ����������.\\newline\n");
        break;
    }
}


void goNext(FILE* file)
{
    switch (rand() % 3)
    {
    default:
        fprintf(file, "�������, ��������� ������.\n");
        break;
    }
}


void putToLatex(Vertex* tree, FILE* file)
{
    beginMath(file);
    putVertexToLatex(tree, file);
    endMath(file);
}


void putDifferentialToLatex(Vertex* tree, FILE* file, char var)
{
    if(tree->type() == VertexType::OPERATION && 
        (tree->number() == ADD || tree->number() == SUB))
        fprintf(file, "\\left(");

    if(!(tree->type() == VertexType::CONSTANT && tree->number() == 1))
        putVertexToLatex(tree, file);

    if (tree->type() == VertexType::OPERATION &&
        (tree->number() == ADD || tree->number() == SUB))
        fprintf(file, "\\right)");

    if(!(tree->type() == VertexType::CONSTANT))
        fprintf(file, "\\cdot");
        
    fprintf(file, " d%c", var);
}

void putDerivativeToLatex(Vertex* tree, Vertex* derivative, FILE* file, char var)
{
    
    fprintf(file, "\\[\\left(");

    putVertexToLatex(tree, file);

    fprintf(file, "\\right)");
    
    fprintf(file, "^{\\prime}_{%c} = ", var);

    putVertexToLatex(tree, file);

    fprintf(file, "\\]\\newline\n");
}

void makeHeader(FILE* file)
{
    fprintf(file,
        "\\documentclass[12pt]{article}\n"
        "\\usepackage[english, russian] {babel}\n"
        "\\usepackage[CP1251] {inputenc}\n"
        "\\usepackage{amsmath}\n"
        "\\usepackage{amssymb}\n"
        "\\usepackage{breqn}\n"
        "\\usepackage{parskip}\n"
        "\\usepackage{vmargin}\n"
        "\\setmarginsrb{2 cm}{1 cm} {2 cm} {1 cm} {1 cm}{1 cm}{1 cm}{1 cm}\n"
        "\\usepackage[T2A]{fontenc}\n"
        "\\pagestyle{fancy}\n"
        "\\author{���������� �. �.}\n"
        "\\title{������������ �������� �� �����������������}"
        "\\date{94 ������� 2024 �.}"
        "\\begin{document}\n"
        "\\maketitle\n"
        "\\newline\n"
        "���, ������������, ��������� �������! �� ���, ��������� �� ������� �� ��������� �������?\\newline \\begin{center}*���������� ������*\\end{center} \\newline �� ��� �������� ���� �� ���������, ������� ���������. � ������� ��� �� ������������ �� ��� ����� ��� �������, ������� � �������. ����� ���� ����� ����������� ��� ����� �������:\n");
}


void endLatex(FILE* file)
{
    fprintf(file,
        "\\end{document}");

    fclose(file);
}


void generatePdf(void)
{
    system("pdflatex -synctex=1 -interaction=nonstopmode " TEX_FILE " > NUL");
    system("start " PDF_FILE);
}