#pragma once

#include <global.h>

#define is_null(p) _is_null((p), #p)

static inline bool _is_null(const void *p, const char *p_sym) {
    if (!p) {
        printerrf("%s: NULL ptr\n", p_sym);
        return true;
    }
    return false;
}


static inline FILE *fopen_checked(const char *filename, const char *openmode) {
    if (is_null(filename) || is_null(openmode)) {
        printerrf("fopen: filename and openmode cannot be NULL\n");
        return NULL;
    }
    FILE *fp = fopen(filename, openmode);
    if (!fp) {
        printerrf("fopen %s failed: %s\n", filename, strerror(errno));
        return NULL;
    }
    return fp;
}
static inline FILE *fclose_checked(FILE *fp) {
    if (fp) fclose(fp);
    return NULL;
}

static inline void *malloc_checked(size_t mem_size) {
    if (!mem_size) {
        printerrf("malloc: mem_size cannot be 0\n");
        return NULL;
    }
    void *mem = malloc(mem_size);
    if (!mem) {
        printerrf("malloc failed\n");
        return NULL;
    };
    return mem;
}
static inline void *free_checked(void *p) {
    if (p) free(p);
    return NULL;
}

static inline longlong_t fread_checked(void *buf, size_t type_size, size_t count, FILE *fp) {
    if (is_null(buf) || is_null(fp)) return 0;
    if (!type_size) {
        printerrf("fread: type_size cannot be 0\n");
        return -1;
    }
    size_t fread_count = fread(buf, type_size, count, fp);
    if (fread_count != count) {
        if (feof(fp)) {
            printerrf("fread: EOF reached\n");
        } else if (ferror(fp)) {
            printerrf("io error: %s\n", strerror(errno));
        }
        return -1;
    }
    return (longlong_t)fread_count;
}
static inline longlong_t fwrite_checked(void *buf, size_t type_size, size_t count, FILE *fp) {
    if (is_null(buf) || is_null(fp)) return 0;
    if (!type_size) {
        printerrf("fwrite: type_size cannot be 0\n");
        return -1;
    }
    size_t fwrite_count = fwrite(buf, type_size, count, fp);
    if (fwrite_count != count) {
        printerrf("fwrite failed\n");
        return -1;
    }
    return (longlong_t)fwrite_count;
}

