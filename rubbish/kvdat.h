#ifndef KVDAT_H
#define KVDAT_H

#include <global_intdef.h>
#include <typeflags.h>
#include <stdbool.h>

typedef struct _tlv_obj {
    ulong_t     type;
    size32_t    len;
    void       *data;
} TLVDataObject;

enum KVDAT_OpenMode {
    MODE_CREATE    = 0,
    MODE_OPEN      = 1,
};


#define KVDAT_INIT_FAILED -1

typedef struct _dat_file_obj KVDAT_DatFileObject;

int KVDAT_init(
    KVDAT_DatFileObject    *_this,
    const char             *filepath,
    int                     entrycap,
    int                     openmode
);
bool KVDAT_commit(KVDAT_DatFileObject *_this);
void KVDAT_deinit(KVDAT_DatFileObject *_this);

#endif