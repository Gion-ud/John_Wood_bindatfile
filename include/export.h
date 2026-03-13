#ifndef EXPORT_H
#define EXPORT_H

#if defined(_MSC_VER)
    #ifdef DLL_BUILD
        #define LIB_API __declspec(dllexport)
    #else
        #define LIB_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__) || defined(__ICC)
    #define LIB_API __attribute__((visibility("default")))
#else
    #define LIB_API
#endif


#endif