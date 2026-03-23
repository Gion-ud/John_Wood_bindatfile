#pragma once

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
//#include <dirent.h>
#endif
#include "global_intdef.h"

static inline ssize_t read_full(int fd, void *buf, size_t buf_len) {
    ssize_t n_read_total = 0;
    ssize_t ret = 0;
    while (n_read_total < buf_len) {
        ret = read(fd, (byte_t*)buf + n_read_total, buf_len - n_read_total);
        if (ret < 0) {
        if (errno == EINTR)
            continue;
            perror("read");
            return -1;
        } else if (!ret) {
            printerrf("EOF reached\n");
            break;
        }
        n_read_total += ret;
    }
    return n_read_total;
}

static inline ssize_t write_full(int fd, void *buf, size_t buf_len) {
    ssize_t n_written_total = 0;
    ssize_t ret = 0;
    while (n_written_total < buf_len) {
        ret = write(fd, (byte_t*)buf + n_written_total, buf_len - n_written_total);
        if (ret < 0) {
        if (errno == EINTR)
            continue;
            perror("write");
            return -1;
        }
        n_written_total += ret;
    }
    return n_written_total;
}

