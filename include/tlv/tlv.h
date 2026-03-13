// kvdb.h
#ifndef TLV_H
#define TLV_H

#include "global.h"
#include "typeflags.h"
#include "export.h"

typedef struct _tlv_obj {
    ulong_t     type;
    size32_t    len;
    void       *data;
} TLVBuffer;

typedef struct _lp_buf {
    size32_t    len;
    void       *data;
} LPBuffer;


#endif