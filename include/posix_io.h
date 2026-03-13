#pragma once
#include <sys/stat.h>

#ifdef _MSC_VER
    #include <io.h>
    #define stat    _stat
    #define open    _open
    #define write   _write
    #define read    _read
    #define fileno  _fileno
    #define STDIN_FILENO    0
    #define STDOUT_FILENO   1
    #define STDERR_FILENO   2
#else
    #include <unistd.h>
    #include <dirent.h>
    #include <fcntl.h>
#endif
