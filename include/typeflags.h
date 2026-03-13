#ifndef TYPEFLAGS_H
#define TYPEFLAGS_H

#define typeflag_sym(type)  (#type)

enum TypeFlags {
    TYPE_NONE       = 0x00,
    TYPE_TEXT       = 0x01,
    TYPE_BLOB       = 0x02,
//  TYPE_BOOL       = 0x03,

    TYPE_INT8       = 0x10,
    TYPE_INT16      = 0x11,
    TYPE_INT32      = 0x12,
    TYPE_INT64      = 0x13,
    TYPE_INT128     = 0x14,

    TYPE_UINT8      = TYPE_INT8     + 0x08,
    TYPE_UINT16     = TYPE_INT16    + 0x08,
    TYPE_UINT32     = TYPE_INT32    + 0x08,
    TYPE_UINT64     = TYPE_INT64    + 0x08,
    TYPE_UINT128    = TYPE_INT128   + 0x08,

    TYPE_FLOAT16    = 0x20,
    TYPE_FLOAT32    = 0x21,
    TYPE_FLOAT64    = 0x22,
    TYPE_FLOAT128   = 0x23,
};

enum TypeSizes {
    TYPE_BYTE_SIZE  = 1,
    TYPE_WORD_SIZE  = 2,
    TYPE_DWORD_SIZE = 4,
    TYPE_QWORD_SIZE = 8,
    TYPE_INT128_SIZE = 16,
};

enum TypeFlagAliasesAsm {
    TYPE_BYTE       = TYPE_UINT8,
    TYPE_WORD       = TYPE_UINT16,
    TYPE_DWORD      = TYPE_UINT32,
    TYPE_QWORD      = TYPE_UINT64,
};

enum DerivedTypeFlags {
    TYPE_ARRAY      = (0x01 << 8),
    TYPE_STRUCT     = 0x31,
    TYPE_UNION      = 0x32,
};


#define TFLAG_LOB_RSVD_MAX TYPE_UNION
#define TFLAG_HIB_RSVD_MAX TYPE_ARRAY

static inline int typeflag_sizeof(byte_t flags) {
    if (flags > TFLAG_LOB_RSVD_MAX || flags < TYPE_INT8)
        return -1;
    switch (flags) {
        case TYPE_INT8:
        case TYPE_UINT8:
            return TYPE_BYTE_SIZE;
        case TYPE_INT16:
        case TYPE_UINT16:
        case TYPE_FLOAT16:
            return TYPE_WORD_SIZE;
        case TYPE_INT32:
        case TYPE_UINT32:
        case TYPE_FLOAT32:
            return TYPE_DWORD_SIZE;
        case TYPE_INT64:
        case TYPE_UINT64:
        case TYPE_FLOAT64:
            return TYPE_QWORD_SIZE;
        case TYPE_INT128:
        case TYPE_UINT128:
        case TYPE_FLOAT128:
            return TYPE_INT128_SIZE;
        default:
            return -1;
    }
}


#endif
