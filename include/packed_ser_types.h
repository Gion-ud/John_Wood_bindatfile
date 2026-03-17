#include <global_intdef.h>
#include <typeflags.h>

//#pragma pack(push, 1)


typedef struct
__attribute__((packed)) _packed_arr {
    word_t      itemtype;
    word_t      flags;
    word_t      typesize;
    size32_t    itemcount;
    word_t      reserved;
} PACKED_ARRAY_HEADER;


typedef struct
__attribute__((packed)) _packed_matrix {
    word_t      itemtype;
    word_t      flags;
    word_t      typesize;
    size32_t    x;
    size32_t    y;
    word_t      reserved;
} PACKED_MATRIX_HEADER;

typedef struct
__attribute__((packed)) _packed_tensor {
    word_t      itemtype;
    word_t      flags;
    word_t      typesize;
    size32_t    x;
    size32_t    y;
    size32_t    z;
    word_t      reserved;
} PACKED_TENSOR_HEADER;

typedef struct
__attribute__((packed)) _nth_dim_packed_tensor {
    word_t      itemtype;
    word_t      flags;
    size32_t    typesize;
    size32_t    ndim;
    size32_t    dim_arr[];
} PACKED_NTH_DIM_TENSOR_HEADER;


typedef float float3[3];
typedef float float4[4];

__attribute__((packed))
struct packed_struct_field_header {
    word_t      field_type;
    word_t      field_size;
    uoff32_t    field_off;
};



//#pragma pack(pop)