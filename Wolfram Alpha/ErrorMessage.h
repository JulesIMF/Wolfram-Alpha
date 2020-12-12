/*
Copyright (c) 2020  MIPT

Module Name:
    ErrorMessage.h

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#ifndef JULESIMF_ERRORMESSAGE
#define JULESIMF_ERRORMESSAGE
static enum class ConsoleColor
{
    Black,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Brown,
    LightGray,
    DarkGray,
    LightBlue,
    LightGreen,
    LightCyan,
    LightRed,
    LightMagenta,
    Yellow,
    White
};
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
static void setColor(ConsoleColor text)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)(text));
}

static int errorMessage(char const* format, ...)
{
    va_list list;
    __crt_va_start(list, format);
    setColor(ConsoleColor::LightRed);
    fprintf(stderr, "Error: ");
    int returned = vfprintf(stderr, format, list);
    setColor(ConsoleColor::LightGray);
    return returned;
}

static void printfMagenta(char const* format, ...)
{
    setColor(ConsoleColor::LightMagenta);
    va_list list = nullptr;
    va_start(list, format);
    vprintf(format, list);
    va_end(list);
    setColor(ConsoleColor::LightGray);
}

#endif // !JULESIMF_ERRORMESSAGE
