#include "bindatfile.h"

#define is_null(p) _is_null((p), #p)

static inline bool _is_null(const void *p, const char *p_sym) {
    if (!p) {
        print_err_msg("%s: NULL ptr\n", p_sym);
        return true;
    }
    return false;
}

FILE *fopen_checked(const char *filename, const char *openmode);
FILE *fclose_checked(FILE *fp);
void *malloc_checked(size_t mem_size);
void *malloc_checked(size_t mem_size);
longlong_t fread_checked(void *buf, size_t type_size, size_t cnt, FILE *fp);
longlong_t fwrite_checked(void *buf, size_t type_size, size_t count, FILE *fp);

FILE *fopen_checked(const char *filename, const char *openmode) {
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
FILE *fclose_checked(FILE *fp) {
    if (fp) fclose(fp);
    return NULL;
}

void *malloc_checked(size_t mem_size) {
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
void *free_checked(void *p) {
    if (p) free(p);
    return NULL;
}

longlong_t fread_checked(void *buf, size_t type_size, size_t count, FILE *fp) {
    if (is_null(buf) || is_null(fp)) return 0;
    if (!type_size || !count) {
        printerrf("fread: type_size and count cannot be 0\n");
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
longlong_t fwrite_checked(void *buf, size_t type_size, size_t count, FILE *fp) {
    if (is_null(buf) || is_null(fp)) return 0;
    if (!type_size || !count) {
        printerrf("fread: type_size and count cannot be 0\n");
        return -1;
    }
    size_t fwrite_count = fwrite(buf, type_size, count, fp);
    if (fwrite_count != count) {
        printerrf("fwrite failed\n");
        return -1;
    }
    return (longlong_t)fwrite_count;
}







typedef struct _dat_file_obj {
    FILE                   *fp;
    FILE                   *fp_tmp;
    char                   *filename; // must be inited n freed
    char                   *filename_tmp; // must be inited n freed
    DAT_FILE_HEADER         fileheader;
    DAT_INDEX_ENTRY        *indextable;
    LPBuffer               *key_arr;
    DAT_ENTRY_INMEM        *entry_arr;
    uoff32_t               *entry_relative_offtable;
    size32_t                entrycap;
    bool                    modified;
    uoff32_t                dat_file_cur;
} DAT_FILE_OBJECT;

int DAT_FILE_OBJECT_init(DAT_FILE_OBJECT *_this, size32_t entrycap) {
    
}