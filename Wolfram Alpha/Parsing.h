/*
Copyright (c) 2020  MIPT

Module Name:
    Parsing.h

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#ifndef WOLFRAM_PARSING
#define WOLFRAM_PARSING

#include "Vertex.h"
#include "StringsHolder.h"

void throwParsingException(char const* message, char const* place);

Vertex*     getTree(char const* treeDefinition, bool table[], Vertex* parent = nullptr);
void        generatePicture(Vertex* tree);
#endif //!WOLFRAM_PARSING