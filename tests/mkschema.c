#include "bindatfile/bindatfile.h"
#include <unistd.h>
#include "libc_chked.h"
#include <sys/stat.h>
#include "bindatfile/print.h"

#include "schema.h"
static byte_t buffer[BUFFER_SIZE] = {0};
static off32_t buffer_cur = 0;
//static off32_t buffer_end = BUFFER_SIZE;

int main(void) {
    DAT_FILE_OBJECT d_obj = {0};
    FILE *dat_fp = fopen_checked("schema.dat", "wb+");
    if (!dat_fp) return errno;

    if (DAT_FILE_OBJECT_init(&d_obj, dat_fp, 1, FILE_DEFAULT) < 0) {
        printerrf("DAT_FILE_OBJECT_init failed\n");
        return -1;
    }

    byte_t *p = (byte_t*)buffer;
    struct packed_struct_header *hdr_p = (struct packed_struct_header*)p;
    buffer_cur += sizeof(struct packed_struct_header);
    hdr_p->field_count = 3;

    struct packed_struct_field_header field_hdr_arr[] = {
        {
            .field_type = TYPE_FLOAT32,
            .field_size = sizeof(float),
            .field_off  = offsetof(struct data_entry, salt_conc),
        },
        {
            .field_type = TYPE_FLOAT32 | TYPE_ARRAY,
            .field_size = sizeof(float3),
            .field_off  = offsetof(struct data_entry, gms_mass_before),
        },
        {
            .field_type = TYPE_FLOAT32 | TYPE_ARRAY,
            .field_size = sizeof(float3),
            .field_off  = offsetof(struct data_entry, gms_mass_after),
        },
    };

    memcpy(p + buffer_cur, &field_hdr_arr, sizeof(field_hdr_arr));
    buffer_cur += sizeof(field_hdr_arr);

    char *field_name = "salt_conc";
    size32_t field_name_len = strlen(field_name);
    memcpy(p + buffer_cur, (char*)field_name, field_name_len);
    (p + buffer_cur)[field_name_len] = '\0';
    buffer_cur += (field_name_len + 1);

    field_name = "gms_mass_before";
    field_name_len = strlen(field_name);
    memcpy(p + buffer_cur, (char*)field_name, field_name_len);
    (p + buffer_cur)[field_name_len] = '\0';
    buffer_cur += (field_name_len + 1);

    field_name = "gms_mass_after";
    field_name_len = strlen(field_name);
    memcpy(p + buffer_cur, (char*)field_name, field_name_len + 1);
    (p + buffer_cur)[field_name_len] = '\0';
    buffer_cur += (field_name_len + 1);

    DAT_ENTRY_HEADER entry_header = {0};
    entry_header.type = TYPE_BLOB;
    entry_header.len = buffer_cur;

    if (DAT_FILE_write_entry(&d_obj, &entry_header, p) < 0) return -1;


    DAT_FILE_OBJECT_commit(&d_obj);
    DAT_FILE_OBJECT_deinit(&d_obj);
    fclose_checked(dat_fp);
    return 0;
}