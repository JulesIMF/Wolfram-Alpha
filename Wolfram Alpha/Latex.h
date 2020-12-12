/*
Copyright (c) 2020  MIPT

Module Name:
    Latex.h

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    29.11.2020 18:19

Edit Notes:
    
*/

#ifndef WOLFRAM_LATEX
#define WOLFRAM_LATEX

#include <cstdio>
#include "Vertex.h"

#define TEX_FILE "article.tex"
#define PDF_FILE "article.pdf"

void generatePdf            (void);
void endLatex               (FILE* file);
void makeHeader             (FILE* file);
void putToLatex             (Vertex* tree, FILE* file);
void putDifferentialToLatex (Vertex* tree, FILE* file, char var);
void beginMath              (FILE* file);
void endMath                (FILE* file);
void tryToSimplify          (FILE* file, Vertex* vertex);
void hereWeGet              (FILE* file);
void hereWeDifferentiate    (FILE* file);
void goNext                 (FILE* file);
void atFirst                (FILE* file, Vertex* vertex);
void putDerivativeToLatex   (Vertex* tree, Vertex* derivative, FILE* file, char var);
#endif //!WOLFRAM_LATEX
