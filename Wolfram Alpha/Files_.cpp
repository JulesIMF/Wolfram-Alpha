/*
Copyright (c) 2020  MIPT

Module Name:
    Files_.cpp

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "Line.h"
//#include "Comparasion.h"

//#define JULESIMF_DEBUG
#define JULESIMF_NO_OUTPUT

int write(char const* fileName, Line* strings, int nStrings, char const* mode)
{
#ifdef    JULESIMF_DEBUG
	assert(fileName);
	assert(strings);
	assert(mode); //Что??
#endif //!JULESIMF_DEBUG

#ifndef   JULESIMF_NO_OUTPUT
	printf("Output file opened\n");
#endif //!JULESIMF_NO_OUTPUT


	//Файл с данными
	int is_stdout = !strcmp(fileName, "?");
	FILE* file = is_stdout ? stdout : fopen(fileName, mode);


	if (!file)
		return 1;

#ifndef   JULESIMF_NO_OUTPUT
	printf("Writing output...\n");
#endif //!JULESIMF_NO_OUTPUT

	//Поехали выводить
	for (int i = 0; i != nStrings; i++)
	{
	#ifdef    JULESIMF_DEBUG
		assert(strings[i].string);
	#endif //!JULESIMF_DEBUG
		if (!(i & 3) && i)
			fprintf(file, "\n");

		fprintf(file, "%s\n", strings[i].string);
	}

#ifndef   JULESIMF_NO_OUTPUT
	printf("Output file closed\n");
#endif //!JULESIMF_NO_OUTPUT

	if (!is_stdout)
		fclose(file);
	return 0;
}


int separateStrings(void* source, int fileSize, Line* destination, int nStrings)
{
#ifdef    JULESIMF_DEBUG
	assert(source);
	assert(destination);
#endif //!JULESIMF_DEBUG

	char* text = (char*)source;
	int   iDestination = 0; //Итератор для destination
	char* currentString = 0;
	int   currentStringLength = 0;
	int nSyllables = 0;

	for (int iText = 0; iText != fileSize + 1; iText++)
	{
		//Если iDestinаtion слишком большой, то JOPA
		if (iDestination >= nStrings)
			return 1;

		//Это если мы в прошлый раз запихнули строку
		if (!currentString)
			currentString = text + iText;

		if (text[iText] == '\n' || !text[iText])
		{
			text[iText] = '\0';
			Line newString =
			{ currentString, currentStringLength };

			destination[iDestination] = newString;
			iDestination++;
			currentString = 0;
			currentStringLength = 0;
			nSyllables = 0;
		}
		else
		{
			//nSyllables += !!isVowel(text[iText]);
			currentStringLength++;
		}
	}

	if (iDestination != nStrings)
		return 1;

	return 0;
}


int getFileSize(FILE* file)
{
#ifdef    JULESIMF_DEBUG
	assert(file);
#endif //!JULESIMF_DEBUG

	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	return fileSize;
}


char* translateFileIntoRam(char const* fileName, int* pFileSize, int* nStrings)
{
	//Файл с данными
	FILE* file = fopen(fileName, "r");
	if (!file)
	{
		//printf("Failed to open program \"%s\"\n", fileName);
		return 0;
	}
	else
	{
	#ifndef   JULESIMF_NO_OUTPUT
		printf("Input file opened\n");
	#endif //!JULESIMF_NO_OUTPUT
	}

#ifdef    JULESIMF_DEBUG
	assert(file);
	assert(pFileSize);
	assert(nStrings);
#endif //!JULESIMF_DEBUG
	int fileSize = getFileSize(file);
	void* translatedFile = malloc(fileSize + 2); //на всякий случай
	if (!translatedFile)
		return 0;

	int RETURN_fread = fread(translatedFile, 1, fileSize, file);
	((char*)translatedFile)[RETURN_fread] = '\0'; //По новой спецификации функции (чисто поржать)
	fseek(file, 0, SEEK_SET);

	int nNewLines = 0;
	for (int i = 0; i != RETURN_fread; i++)
	{
		if (((char*)translatedFile)[i] == '\n')
			nNewLines++;
	}
	*nStrings = nNewLines + 1;
	*pFileSize = RETURN_fread;

#ifndef   JULESIMF_NO_OUTPUT
	printf("Input file closed\n");
#endif //!JULESIMF_NO_OUTPUT

	fclose(file);
	return (char*)translatedFile;
}
