#include "bindatfile/bindatfile.h"
#include <unistd.h>
#include "libc_chked.h"
#include <sys/stat.h>
#include "bindatfile/print.h"

#include "schema.h"
static byte_t buffer[BUFFER_SIZE] = {0};


const struct float3_packed point_arr[16] = {
    {0.0f, 0.0f, 0.0f},
    {1.0f, 0.5f, -1.0f},
    {2.0f, -0.5f, 3.0f},
    {-1.0f, 1.0f, 0.0f},
    {0.5f, -1.5f, 2.5f},
    {1.5f, 2.0f, -0.5f},
    {-2.0f, 0.0f, 1.0f},
    {3.0f, -1.0f, -2.0f},
    {-0.5f, 0.5f, 0.5f},
    {2.5f, 1.5f, -1.5f},
    {0.0f, -2.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {-1.0f, -1.0f, -1.0f},
    {0.75f, -0.25f, 0.5f},
    {-0.5f, 2.0f, -0.75f},
    {1.25f, -1.25f, 0.0f}
};
size_t point_cnt = sizeof(point_arr) / sizeof(struct float3_packed);

__attribute__((packed)) struct struct_field_header {
    byte_t type;
    byte_t type_size;
};
__attribute__((packed)) struct array_header {
    byte_t  type;
    byte_t  type_size;
    dword_t count;
};


// current file layout:
// (offsets are 32 bit based)
/*
[FileHeader]
[Records]
    Entry 0001: type|flag|len|(byte_t[])data[len]
    ...
[OffsetTable]
    offset[0001]
    ...

[FilFooter]
*/

int main(void) {
    DAT_FILE_OBJECT d_obj = {0};
    FILE *dat_fp = fopen_checked("output/data.bin", "wb+");
    if (!dat_fp) return errno;

    if (DAT_FILE_OBJECT_init(&d_obj, dat_fp, 48, FILE_DEFAULT) < 0) {
        printerrf("DAT_FILE_OBJECT_init failed\n");
        return -1;
    }

    printerrf("init: success\n");

    DAT_ENTRY_HEADER entry_header = {0};

    entry_header.len = sizeof(struct float3_packed);
    entry_header.type = TYPE_BLOB;

    for (size_t i = 0; i < point_cnt; ++i) {
        print_dbg_msg("Entry:%zu\n", i);
        entry_header.type = TYPE_STRUCT;
        entry_header.len = sizeof(struct float3_packed);
        if (DAT_FILE_write_entry(&d_obj, &entry_header, (byte_t*)&point_arr[i]) < 0)
            break;
    }


    /*
    DAT_FILE_delete_entry(&d_obj, 0);
    DAT_FILE_delete_entry(&d_obj, 1);
    DAT_FILE_delete_entry(&d_obj, 4);
    DAT_FILE_delete_entry(&d_obj, 6);
    DAT_FILE_delete_entry(&d_obj, 7);
    DAT_FILE_delete_entry(&d_obj, 13);
    DAT_FILE_delete_entry(&d_obj, 16);
    */


    DAT_FILE_OBJECT_commit(&d_obj);

    if (write(STDOUT_FILENO, (char*)"\n", 1) != 1) {
        perror("write");
        DAT_FILE_OBJECT_deinit(&d_obj);
        fclose_checked(dat_fp);
        return -1;
    }
    for (size_t i = 0; i < point_cnt; ++i) {
        if (DAT_FILE_read_entry(&d_obj, i, &entry_header, buffer) < 0) continue;
        printf("Entry%.4zu: [%.4f %.4f %.4f]\n",
            i,
            *(float*)((byte_t*)buffer + offsetof(struct float3_packed, x)),
            *(float*)((byte_t*)buffer + offsetof(struct float3_packed, y)),
            *(float*)((byte_t*)buffer + offsetof(struct float3_packed, z))
        );
    }

    DAT_FILE_OBJECT_deinit(&d_obj);
    fclose_checked(dat_fp);
    return 0;
}