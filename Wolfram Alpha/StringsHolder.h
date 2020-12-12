/*
Copyright (c) 2020  MIPT

Module Name:
    StringsHolder.h

Abstract:    
    
Author:
    JulesIMF

Last Edit:
    29.11.2020 16:27

Edit Notes:
    
*/

#ifndef WOLFRAM_STRINGSHOLDER
#define WOLFRAM_STRINGSHOLDER


class StringsHolder
{
protected:
    char*       buffer_;
    size_t      capacity_;
    size_t      size_;

    size_t      roundPow2_(size_t x);

public:
    class StringIter
    {
    protected:
        StringsHolder*      holder;
        size_t              begin;

    public:
        char const*         get(void) const;
        static StringIter   makeStringIter(StringsHolder* holder_, size_t begin_);

    };


    static StringsHolder*   newStringsHolder(void);

    StringIter              insert(char const* source, size_t length);
};

#endif //!WOLFRAM_STRINGSHOLDER
