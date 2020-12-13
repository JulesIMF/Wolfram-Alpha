/*
Copyright (c) 2020  MIPT

Module Name:
    Main.cpp

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include "Vertex.h"
#include "Parsing.h"
#include "Files.h"
#include <clocale>
#include <ctime>
#include "Latex.h"
#include "Derivative.h"
#include "Simplify.h"
#include "Grammar.h"

#define EXPR_FILE "Expr.txt"

Vertex* getTreeFromMemory(bool table[], char const* fileName)
{
    char* tran = nullptr;
    int fileSize = 0, nStrings = 0;
    tran = (char*)translateFileIntoRam(
        fileName,
        &fileSize,
        &nStrings);
    if (tran == nullptr)
        throwParsingException("no file found", "");

    Parser parser;
    Vertex* tree = parser.getG(tran, table);
    free(tran);
    return tree;
}


int countChars(bool table[256])
{
    int cnt = 0;
    for (int i = 0; i != 256; i++)
        cnt += table[i];

    return cnt;
}

char nextSymbol(bool table[256], int i = -1)
{
    static int nextPosition = 0;

    if (!i)
        nextPosition = 0;

    for (; !table[nextPosition]; nextPosition++)
        ;
    return nextPosition++;
}


int main(int n, char const** param)
{
    setlocale(LC_ALL, "RUS");
    srand(time(nullptr));
    bool    table       [256] = { };
    Vertex* derivatives [256] = { };


    FILE* latexFile = fopen(TEX_FILE, "w");
    makeHeader(latexFile);

    auto tree = getTreeFromMemory(table, (n > 1) ? param[1] : EXPR_FILE);
    putToLatex(tree, latexFile);
    auto nVariables = countChars(table);


    if (nVariables >= 2)
    {
        fprintf(latexFile, "�� ������ ��� �� ������� %d ����������, ��, ������� �� ������ ��������"
            " �� �����������, � �� ������ ������������. ������� �� ������� ����� ������� �����������.\\newline\\newline", nVariables);

        for (int i = 0; i != nVariables; i++)
        {
            if (i) goNext(latexFile);
            auto thisVar = nextSymbol(table);
            fprintf(latexFile, "������� ������� ����������� �� %c.\\newline\n", thisVar);
            auto derivative = getDerivative(latexFile, tree, thisVar);
            atFirst(latexFile, derivative);
            simplify(derivative, latexFile);
            hereWeGet(latexFile);
            derivatives[i] = derivative;
        }

        fprintf(latexFile, "��, �������, ��� �� ������ �������� ������ ������������, �� ������� ��� � ������� � �����:\\newline");

        beginMath(latexFile);

        for (int i = 0; i != nVariables; i++)
        {
            if (i) fprintf(latexFile, "+");
            putDifferentialToLatex(derivatives[i], latexFile, nextSymbol(table, i));
        }

        endMath(latexFile);
    }


    else
    {
        if (!nVariables) table['x'] = true;

        auto derivative = getDerivative(latexFile, tree, nextSymbol(table));
        atFirst(latexFile, derivative);
        simplify(derivative, latexFile);
        hereWeGet(latexFile);
    }
    fprintf(latexFile, "�������, ������� ������� �������"
        "\\newline \\begin{center}*�������*\\end{center}\\newpage\n"
        "\\begin{center}*���-�� �� ����� ��������*\\end{center}\\newline\n"
        "�������: \"�� �� ����, ������ �������� ��� ����� �� ���� �� ��������?\"\\newline\n"
        "���� �.: \"��� ������ ��� ��� ��������� ���, ����, mipt telecom ������.\\newline\n"
        "�������: \"� �� ������? \"\\newline\n"
        "���� �.: \"����� ������ ����� �� https://github.com/JulesIMF/Wolfram-Alpha, ��� ����� ������� � �������� � ���������� ���� ��������������.\"\\newline\n"
        "������: \\ \"����� � ���������? � ������� �� ��� ����� � ���?\"\\newline\n"
        "���� �.: \"��� ���� � ���� ����, ��� ������ ������� �������� ������ ���������, ����� ���������� ��������� ������ �����������, � ����� ����� ���-������ ����������.\"\\newline\n"
        "������: \\ \"�� ��, � ������ �� �����, ����� �������� ����.\"\\newline\n"
        "���� A.: \"�, ������, ��������!\"\\newline\n"
        "���� �.: \"׸? ������ � ���� ��������?\"\\newline\n"
        "���� A.: \"��� ��� �� ���� �������. � �� ����� �����, ��� ����� ������� ����������� ���� �����?\"\\newline\n"
        "���� �.: \"�� ��, ��� �� �������� ��� ������ ������������ ����� �����.\"\\newline\n"
        "���� A.: \"���������, ���� ������������!\"\n"
    );
    
    endLatex(latexFile);

    printf("���������� PDF...");
    generatePdf();
    printf("\n������!");
}