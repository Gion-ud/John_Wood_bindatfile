#pragma once
#include <stddef.h>
#include <stdint.h>

typedef unsigned char       byte_t;
typedef signed char         char_t;
typedef unsigned char       uchar_t;

typedef unsigned short      word_t;
typedef signed short        short_t;
typedef unsigned short      ushort_t;

typedef unsigned int        dword_t;
typedef signed int          long_t;
typedef unsigned int        ulong_t;

typedef unsigned long long  qword_t;
typedef signed long long    longlong_t;
typedef unsigned long long  ulonglong_t;

typedef ulong_t             size32_t;
typedef ulonglong_t         size64_t;

#ifndef __MINGW32__
typedef long_t              off32_t;
typedef longlong_t          off64_t;
#endif

typedef ulong_t             uoff32_t;
typedef ulonglong_t         uoff64_t;


typedef unsigned long long  ull_t;
