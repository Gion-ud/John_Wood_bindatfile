#include <global.h>
#include <typeflags.h>
#include <posix_io.h>
#include <libc_chked.h>

typedef __attribute__((packed)) struct _array_header {
    dword_t     magic;
    byte_t      type;
    byte_t      type_size;
    byte_t      dim_cnt;
    size32_t    data_off;
    byte_t      reserved;
} ARRAY_HEADER;
//  dword_t     dim_vec[dim_cnt];
//  pad to 8 bytes
//  Fuck big endian they need to conv to le themselves


int ARRAY_serialise(
    void       *arr_p,
    byte_t      type,
    byte_t      type_size,
    byte_t      dim_cnt,
    size32_t    dim_vec[],
    byte_t     *out_arr_buf_p
) {
    if (is_null(arr_p) || is_null(dim_vec) || is_null(out_arr_buf_p)) return -1;
    
    
}

int main(void) {
    
}