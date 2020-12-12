/*
Copyright (c) 2020  MIPT

Module Name:
    Algorithm.h

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#ifndef JULESIMF_ALGORITHM
#define JULESIMF_ALGORITHM

template<typename T>
static size_t binarySearch(T const* data, size_t size, T value)
{
    size_t begin = 0,
        end = size;

    size_t mid = 0;
    while (begin - end)
    {
        mid = (begin + end) >> 1;

        if (data[mid] == value)
            return mid;

        if (data[mid] > value)
            end = mid;
        else
            begin = mid + 1;
    }

    return size;
}

#endif //!JULESIMF_ALGORITHM