/*
Copyright (c) 2020  MIPT

Module Name:
    Files.h

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#pragma once
#include <stdio.h>
#include "Line.h"
/**
 * \brief ��������� ���� � RAM � ��������� 0 ����� ����, ��� ���� ����� 0 ���� ��� ���� ��������� ����.
 *
 * \param fileName ����������� ����
 * \param fileSize ��������� �� ����������, � ������� ����� ������� ������ �����
 * \param nStrings ��������� �� ����������, � ������� ����� �������� ���������� �����
 * \return ��������� �� ������� ������, � ������� �������� ����, 0 � ������ ������
 */
char* translateFileIntoRam(char const* fileName, int* pFileSize, int* nStrings);


/**
 * \brief ������� ������ ����� � ����.
 * \param fileName ������ � ������ ����� ("?" ��� stdout)
 * \param strings ������ �����
 * \param nStrings ��������� ����� � �������
 * \param mode ����� �������� �����
 * \return 0 � ������ ��������� ����������, 1 � ������ ������ �������� �����
 */
int write(char const* fileName, Line* strings, int nStrings, char const* mode);


/**
 * \brief ���������� ������ �����
 * \param file ����������� ����
 * \return ������ ����� � ������.
 */
int getFileSize(FILE* file);


/**
 * \brief ��������� source �� ������ � destination.
 * \param source ��������� �� ��������������� ����
 * \param fileSize ������ ���������������� �����
 * \param destination ��������� �� ������ �� ��������
 * \param nStrings ���������� �����
 * \warning source �� ��������������!!
 * \warning ������ *stringsLengths ��������������!!
 * \return 0 � ������ ��������� ����������, 1 ���� �������� �������������� ���������, 2 ���� ���� ������ ������� 100
 */
int separateStrings(void* source, int fileSize, Line* destination, int nStrings);
