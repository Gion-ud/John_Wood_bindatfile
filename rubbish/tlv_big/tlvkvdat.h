#ifndef TLVKVDAT_H
#define TLVKVDAT_H

#include <global_intdef.h>
#include <typeflags.h>

typedef struct _tlv_obj {
    ulong_t     type;
    size32_t    len;
    void       *data;
} TLVDataObject;

enum TLVKVDAT_OpenMode {
    TLVKVDAT_MODE_CREATE    = 0,
    TLVKVDAT_MODE_OPEN      = 1,
};

#define TLVKVDAT_INIT_FAILED -1

typedef TLVKVDAT_DatFileObject *TLVKVDAT_Handle;

#endif