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
    FILE *dat_fp = fopen_checked("schema.dat", "wb+");
    if (!dat_fp) return errno;

    byte_t *p = (byte_t*)buffer;

    size32_t field_count = 3;
    memcpy(p, &field_count, sizeof(size32_t));
    buffer_cur += sizeof(size32_t);

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
    memcpy(p + buffer_cur, (char*)field_name, field_name_len);
    (p + buffer_cur)[field_name_len] = '\0';
    buffer_cur += (field_name_len + 1);

    if (write(fileno(dat_fp), p, buffer_cur) < 0) {
        perror("write");
        return -1;
    }

    fclose_checked(dat_fp);
    return 0;
}