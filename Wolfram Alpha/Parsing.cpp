/*
Copyright (c) 2020  MIPT

Module Name:
    Parsing.cpp

Abstract:    
    Реализует парсинг деревьев выражений, записаных в формате Б05-033

Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include "ErrorMessage.h"
#include "Parsing.h"

#define     BEGIN_BRACKET   '['
#define     END_BRACKET     ']'
#define     BEGIN_QUOTE     '\"'
#define     END_QUOTE       '\"'
#define     DOT_FILE        "picture.dot"
#define     PICTURE_FILE    "picture.png"
#define     MAX_LENGTH      20

#define     DOT_FILE        "description.dot"
#define     PIC_FILE        "tree.png"


static int countUntilSlashN(char const* place)
{
    int ans = 0;
    while(*place != '\0' &&
          *place != '\n' &&
          ans    != MAX_LENGTH) ans++, place++;

    return ans;
}


void  throwParsingException(char const* message, char const* place)
{
    errorMessage("%s\n"
        "\n\t%.*s\n\t^~~\n",
        message,
        countUntilSlashN(place),
        place);

    system("pause");

    exit(-1);
}


//Эта функция необходима, т. к. кириллица валит стандартную ctype функцию
static bool isSpace_(char c)
{
    return 
        c == ' '  ||
        c == '\n' ||
        c == '\r' ||
        c == '\b' ||
        c == '\t';
}


/**
 * Продвигает указатель на treeDefinition, пропуская пробелы.
 * 
 * \param treeDefinition
 * \return 
 */
static char const* advance(char const* treeDefinition)
{
    char const* newDefinition = treeDefinition;
    while (*newDefinition && isSpace_(*newDefinition))
        newDefinition++;

    return newDefinition;
}


/**
 * Проверяет, является ли текущий разбираемый элемент листом.
 * 
 * \param treeDefinition указатель на то, что стоит после имени.
 * \return 
 */
static bool isLeaf(char const* treeDefinition)
{
    treeDefinition = advance(treeDefinition);
    return !(*treeDefinition == BEGIN_BRACKET);
}


static char const* getNodeNameEnd(char const* nodeBegin)
{
    nodeBegin = advance(nodeBegin);

    if (*nodeBegin == BEGIN_QUOTE)
    {
        nodeBegin++;
        while (*nodeBegin != END_QUOTE)
        {
            if (*(nodeBegin) == '\0')
                throwParsingException("unexpected end of vertex name", nodeBegin);
            nodeBegin++;
        }
        
        return nodeBegin + 1; //ясен пень, что это не бегин, а енд, но мы решили не заводить лишней переменной
    }

    throwParsingException("unexpected symbol met", nodeBegin);
}

static char const* getNodeEnd(char const* nodeBegin)
{
    nodeBegin = advance(nodeBegin);

    if(*nodeBegin == BEGIN_QUOTE)
        nodeBegin = getNodeNameEnd(nodeBegin);

    if (isLeaf(nodeBegin))
        return nodeBegin;

    nodeBegin = advance(nodeBegin);

    if (*nodeBegin != BEGIN_BRACKET)
        throwParsingException("unexpected symbol met", nodeBegin);

    int n = 1;

    while (n)
    {
        nodeBegin++;
        if (*nodeBegin == BEGIN_BRACKET)
            n++;
        if (*nodeBegin == END_BRACKET)
            n--;
        if (*(nodeBegin) == '\0')
            throwParsingException("unexpected end of file", nodeBegin);
    }

    return nodeBegin;
}


struct VertexPair
{
    VertexType  type_;
    int         number_;
};


static VertexPair getNodePair(char const* treeDefinition, bool table[])
{
    treeDefinition = advance(treeDefinition);
    if (*treeDefinition != BEGIN_QUOTE)
        throwParsingException("expected quotation mark", treeDefinition);

    treeDefinition++;
    if (*treeDefinition == END_QUOTE)
        throwParsingException("unexpected quotation mark", treeDefinition);


    
    VertexPair pair = {VertexType::OPERATION, 0};

    switch (*treeDefinition)
    {
    case '+':
    case '_':
    case '*':
    case '/':
    case 'l':
    case 's':
    case 'c':
    case 'e':
    case 'S':
    case 'C':
    case '^':
    case 'r':
        pair.number_ = *treeDefinition;
        return pair;

    default:
        if (isalpha(*treeDefinition))
        {
            pair.type_   = VertexType::VARIABLE;
            pair.number_ = *treeDefinition;

            if (islower(pair.number_))
                table[     pair.number_ - 'a'] = true;
            else
                table[28 + pair.number_ - 'A'] = true;

            return pair;
        }

        pair.type_      = VertexType::CONSTANT;
        sscanf(treeDefinition, "%d", &pair.number_);
        return pair;
    }
}


static void splitIntoTwoNodes(char const* begin, char const** first, char const** second)
{
    begin = advance(begin);

    if (*begin == BEGIN_BRACKET)
        begin = advance(begin + 1);

    if (*begin != BEGIN_QUOTE)
        throwParsingException("expected tree", begin);

    *first = begin;
    *second = getNodeEnd(begin);
    *second = advance(*second);
    if (**second == END_BRACKET)
    {
        (*second)++;
        *second = advance(*second);
    }
}


Vertex* getTree(char const* begin, bool table[], Vertex* parent)
{
    begin = advance(begin);

    if (*begin != BEGIN_QUOTE)
        return nullptr;

    auto pair = getNodePair(begin, table);
    begin = getNodeNameEnd(begin);

    Vertex* currentVertex = Vertex::newVertex(pair.type_, pair.number_, parent);
    if (isLeaf(begin))
        return currentVertex;

    begin = advance(begin);

    char const* beginOfFirstTree  = nullptr,
              * beginOfSecondTree = nullptr;
    splitIntoTwoNodes(begin, &beginOfFirstTree, &beginOfSecondTree);

    Vertex* firstTree  = getTree(beginOfFirstTree, table, currentVertex),
          * secondTree = getTree(beginOfSecondTree, table, currentVertex);
           
    if( !(firstTree && secondTree) )
        throwParsingException("expected two trees", begin);

    currentVertex->setLeft (firstTree);
    currentVertex->setRight(secondTree);
    return currentVertex;
}


void putVertexToDot(FILE* file, Vertex* vertex)
{
    if (vertex == nullptr)
        return;

    if (vertex->type() == VertexType::CONSTANT)
    {
        fprintf(file,
            "v%p[color = \"red\", "
            "fontcolor = \"white\", "
            "fillcolor = \"darkgray\", "
            "shape = rectangle, "
            "style=\"rounded, filled\" , "
            "label = \"type = const\\n%d\"];\n",
            vertex,
            vertex->number());
        return;
    }

    if (vertex->type() == VertexType::VARIABLE)
    {
        fprintf(file,
            "v%p[color = \"red\", "
            "fontcolor = \"white\", "
            "fillcolor = \"darkgray\", "
            "shape = rectangle, "
            "style=\"rounded, filled\" , "
            "label = \"type = var\\n%c\"];\n",
            vertex,
            vertex->number());
        return;
    }

    fprintf(file,
        "v%p[color = \"black\", "
        "fontcolor = \"red\", "
        "fillcolor = \"white\", "
        "shape = rectangle, "
        "label = \"type = oper\\n%c\"];\n",
        vertex,
        vertex->number());

    fprintf(file,
        "v%p -> v%p;\n"
        "v%p -> v%p;\n",
        vertex, vertex->left(),
        vertex, vertex->right());

    putVertexToDot(file, vertex->left());
    putVertexToDot(file, vertex->right());
}


void generatePicture(Vertex* tree)
{
    FILE* file = fopen(DOT_FILE, "w");
    fprintf(file, "digraph\n{\ndpi = 400;\n");
    putVertexToDot(file, tree);
    fprintf(file, "}");
    fclose(file);
    system("dot -Tpng " DOT_FILE " -o " PIC_FILE);
    system("start " PIC_FILE);
}