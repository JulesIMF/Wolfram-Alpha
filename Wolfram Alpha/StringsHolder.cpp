/*
Copyright (c) 2020  MIPT

Module Name:
    StringsHolder.cpp

Abstract:    
    Реализует непрерывное хранилище строк

Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#include <cstdlib>
#include "StringsHolder.h"


size_t StringsHolder::roundPow2_(size_t x)
{
    size_t ans = 1;
    while (ans < x)
        ans <<= 1;
    return ans;
}


StringsHolder* StringsHolder::newStringsHolder(void)
{
    StringsHolder* holder = (StringsHolder*)calloc(1, sizeof(StringsHolder));
    holder->capacity_ = 1;
    holder->size_ = 1;
    holder->buffer_ = (char*)calloc(1, sizeof(char));
    holder->buffer_[0] = '\0';
    return holder;
}


char const* StringsHolder::StringIter::get(void) const
{
    return &(holder->buffer_[begin]);
}


StringsHolder::StringIter StringsHolder::StringIter::makeStringIter(StringsHolder* holder_, size_t begin_)
{
    StringsHolder::StringIter iter;
    iter.holder = holder_;
    iter.begin = begin_;
    return iter;
}


StringsHolder::StringIter StringsHolder::insert(char const* source, size_t length)
{
    size_t newSize = size_ + length + 1;

    size_t oldSize = size_;

    if (newSize > capacity_)
    {
        capacity_ = roundPow2_(newSize);
        buffer_ = (char*)realloc(buffer_, capacity_);
    }

    char* returned = buffer_;

    for (size_t i = 0; i != length && source[i]; i++)
    {
        buffer_[size_++] = source[i];
    }

    buffer_[size_++] = '\0';

    return StringsHolder::StringIter::makeStringIter(this, oldSize);
}