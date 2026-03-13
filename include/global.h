#ifndef GLOBAL_H
#define GLOBAL_H

// Explicitly ban BE
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #error "BE systems are NOT supported"
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || defined(__ARMEB__)
    #error "BE systems are NOT supported"
#endif


#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>

#include "global_intdef.h"

#define printerrf(...) fprintf(stderr, __VA_ARGS__);fflush(stderr)

#define DEBUG

#ifdef DEBUG
#define print_dbg_msg(...) fprintf(stderr, __VA_ARGS__);fflush(stderr)
#else
#define print_dbg_msg(...)
#endif


#define LOOP_RUNNING    1
#define BUFFER_SIZE     4096
#define FILE_PATH_LEN   256


#define ESC "\x1b"
//;[
#define COLOUR_BLACK   "[30m"
#define COLOUR_RED     "[31m"
#define COLOUR_GREEN   "[32m"
#define COLOUR_YELLOW  "[33m"
#define COLOUR_BLUE    "[34m"
#define COLOUR_MAGENTA "[35m"
#define COLOUR_CYAN    "[36m"
#define COLOUR_WHITE   "[37m"
#define RESET_COLOUR   "[0m"



#endif