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
        fprintf(latexFile, "Ну точнее это же функция %d переменных, да, поэтому мы должны записать"
            " не производную, а ее полный дифференциал. Давайте по очереди брать частные производные.\\newline\\newline", nVariables);

        for (int i = 0; i != nVariables; i++)
        {
            if (i) goNext(latexFile);
            auto thisVar = nextSymbol(table);
            fprintf(latexFile, "Возьмем частную производную по %c.\\newline\n", thisVar);
            auto derivative = getDerivative(latexFile, tree, thisVar);
            atFirst(latexFile, derivative);
            simplify(derivative, latexFile);
            hereWeGet(latexFile);
            derivatives[i] = derivative;
        }

        fprintf(latexFile, "Ну, коллеги, раз мы должны записать полный дифференциал, то давайте его и запишем в итоге:\\newline");

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
    fprintf(latexFile, "Коллеги, давайте сделаем перерыв"
        "\\newline \\begin{center}*перерыв*\\end{center}\\newpage\n"
        "\\begin{center}*где-то во время перерыва*\\end{center}\\newline\n"
        "Алексей: \"Че за рофл, почему вольфрам эту штуку за пару не посчитал?\"\\newline\n"
        "Иван К.: \"Дак потому что тут интернета нет, алло, mipt telecom только.\\newline\n"
        "Алексей: \"И че делать? \"\\newline\n"
        "Иван Д.: \"Зайди короче потом на https://github.com/JulesIMF/Wolfram-Alpha, там прога которая в оффлайне в символьном виде дифференцирует.\"\\newline\n"
        "Леонид: \\ \"Прямо в симольном? И сколько ты это писал и как?\"\\newline\n"
        "Иван Д.: \"Это лаба у Деда была, там короче сначала строится дерево выражения, потом рекурсивно строитсся дерево производной, а после этого как-нибудь упрощается.\"\\newline\n"
        "Леонид: \\ \"Ну ок, я ничего не понял, зайду посмотрю хоть.\"\\newline\n"
        "Егор A.: \"О, нифига, работает!\"\\newline\n"
        "Иван К.: \"Чё? Откуда у тебя интернет?\"\\newline\n"
        "Егор A.: \"Дак это не мипт телеком. А чё Джулс слышь, эта штука частные производные тоже берет?\"\\newline\n"
        "Иван Д.: \"Ну да, там же написано что полный дифференциал брать умеет.\"\\newline\n"
        "Егор A.: \"Нормально, буду пользоваться!\"\n"
    );
    
    endLatex(latexFile);

    printf("Генерируем PDF...");
    generatePdf();
    printf("\nГотово!");
}