#ifndef TYPEFLAGS_H
#define TYPEFLAGS_H

enum _TYPE_FLAGS {
    TYPE_I8         = 0x10,
    TYPE_I16        = 0x11,
    TYPE_I32        = 0x12,
    TYPE_I64        = 0x13,
    TYPE_I128       = 0x14,

    TYPE_U8         = 0x20,
    TYPE_U16        = 0x21,
    TYPE_U32        = 0x22,
    TYPE_U64        = 0x23,
    TYPE_U128       = 0x24,

    TYPE_F16        = 0x30,
    TYPE_F32        = 0x31,
    TYPE_F64        = 0x32,
    TYPE_F80_X87    = 0x33,
    TYPE_F128       = 0x34,
};

enum TypeFlags {
    TYPE_NONE       = 0x00,
    TYPE_TEXT       = 0x01,
    TYPE_BLOB       = 0x02,
    TYPE_BOOL       = 0x40,

    TYPE_INT        = TYPE_I32,
    TYPE_UINT       = TYPE_U32,

    TYPE_CHAR       = TYPE_I8,
    TYPE_SHORT      = TYPE_I16,
    TYPE_LONG       = TYPE_I32,
    TYPE_LONGLONG   = TYPE_I64,

    TYPE_UCHAR      = TYPE_U8,
    TYPE_USHORT     = TYPE_U16,
    TYPE_ULONG      = TYPE_U32,
    TYPE_ULONGLONG  = TYPE_U64,

    TYPE_HALF       = TYPE_F16,
    TYPE_FLOAT      = TYPE_F32,
    TYPE_DOUBLE     = TYPE_F64,
//  TYPE_LONGDOUBLE = TYPE_F80_X87,
    TYPE_QUAD       = TYPE_F128,
};

#define type_sym(type)  (#type)

enum TypeSize {
    TYPE_NONE_SIZE  = 0,
    TYPE_BOOL_SIZE  = 1,

    TYPE_INT_SIZE   = 4,
    TYPE_UINT_SIZE  = 4,

    TYPE_CHAR_SIZE      = 1,
    TYPE_SHORT_SIZE     = 2,
    TYPE_LONG_SIZE      = 4,
    TYPE_LONGLONG_SIZE  = 8,
    TYPE_INT128_SIZE    = 16,

    TYPE_UCHAR_SIZE     = 1,
    TYPE_USHORT_SIZE    = 2,
    TYPE_ULONG_SIZE     = 4,
    TYPE_ULONGLONG_SIZE = 8,
    TYPE_UINT128_SIZE   = 16,

    TYPE_HALF_SIZE      = 2,
    TYPE_FLOAT_SIZE     = 4,
    TYPE_DOUBLE_SIZE    = 8,
//  TYPE_LONGDOUBLE_SIZE = 10,
    TYPE_QUAD_SIZE      = 16,
};

enum TypeFlagAliases1 {
    TYPE_INT8       = 0x10,
    TYPE_INT16      = 0x11,
    TYPE_INT32      = 0x12,
    TYPE_INT64      = 0x13,
    TYPE_INT128     = 0x14,

    TYPE_UINT8      = 0x20,
    TYPE_UINT16     = 0x21,
    TYPE_UINT32     = 0x22,
    TYPE_UINT64     = 0x23,
    TYPE_UINT128    = 0x24,

    TYPE_FLOAT16    = 0x30,
    TYPE_FLOAT32    = 0x31,
    TYPE_FLOAT64    = 0x32,
    TYPE_FLOAT128   = 0x34,
};

enum TypeFlagAliasesAsm {
    TYPE_BYTE       = TYPE_I8,
    TYPE_WORD       = TYPE_I16,
    TYPE_DWORD      = TYPE_I32,
    TYPE_QWORD      = TYPE_I64,
};

enum DerivedTypeFlags { // only applies for val
    TYPE_ARRAY      = 0xa0,
    TYPE_STRUCT     = 0xb0,
    TYPE_UNION      = 0xc0,
};

/*
// Regular type encoding: [data]
// Array encoding:  (ubyte_t)TYPE_ARRAY, [item_type][item_size][item_count] \
                    [item[0]][item[1]]...[item[item_count - 1]]
// Array size (TreeNode::val_size) = 1 + 1 + 4 + 4 + item_size * item_count
//                                   sizeof(byte_t) + sizeof(ulong_t) + sizeof(ulong_t) + item_size * item_count
// struct encoding: (ubyte_t)TYPE_STRUCT, [field_count] \
                    [field0_type][field0_size][field0_data] \
                    [field1_type][field1_size][field1_data] \
                    ... \
                    [fieldn_type][fieldn_size][fieldn_data]
// struct size: sizeof(ulong_t) + {\
                    for (ulong_t i = 0; i < field_count; ++i) \
                        $ += 1 + sizeof(ulong_t) + sizeof(field[i]) \
                }

//(or just allocate a buffer)
*/


#endif
