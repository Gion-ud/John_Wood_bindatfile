#include <global.h>
#include <typeflags.h>
#include <posix_io.h>
#include <libc_chked.h>
#include <packed_ser_types.h>

#include "schema.h"

struct data_entry entry_arr[] = {
    {
        .salt_conc          = 0,
        .gms_mass_before    = {2.07,2.07,2.11},
        .gms_mass_after     = {4.44,4.38,4.53},
    },
    {
        .salt_conc          = 0.2,
        .gms_mass_before    = {1.88,1.93,2.04},
        .gms_mass_after     = {4.52,4.60,4.82},
    },
    {
        .salt_conc          = 0.4,
        .gms_mass_before    = {1.92,2.06,2.05},
        .gms_mass_after     = {4.67,5.05,4.93},
    },
    {
        .salt_conc          = 0.6,
        .gms_mass_before    = {2.07,1.92,1.96},
        .gms_mass_after     = {4.72,4.37,4.56},
    },
    {
        .salt_conc          = 0.8,
        .gms_mass_before    = {2.07,2.07,1.95},
        .gms_mass_after     = {4.39,4.32,4.25},
    },
};
const size_t entry_arr_len = sizeof(entry_arr) / sizeof(entry_arr[0]);

byte_t buf[BUFFER_SIZE] = {0};
off32_t buf_cur = 0;

const dword_t *magic_p = (dword_t*)(byte_t []){ 0xFF, 'A', 'R', 'R' };

int main(void) {
    memcpy((byte_t*)buf + buf_cur, magic_p, sizeof(*magic_p));
    buf_cur += sizeof(*magic_p);
    *(PACKED_ARRAY_HEADER*)((byte_t*)buf + buf_cur) = (PACKED_ARRAY_HEADER) {
        .itemtype = TYPE_STRUCT_PACKED,
        .typesize = sizeof(struct data_entry),
        .itemcount = entry_arr_len,
        .flags = 0,
    };
    buf_cur += sizeof(PACKED_ARRAY_HEADER);
    memcpy((byte_t*)buf + buf_cur, &entry_arr, sizeof(entry_arr));
    buf_cur += sizeof(entry_arr);

    int fd = open("dataset.bin", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return errno;
    }
    int ret = write(fd, (byte_t*)buf, buf_cur);
    if (ret < 0) {
        perror("write");
        return errno;
    }


    lseek(fd, sizeof(*magic_p), SEEK_SET);
    PACKED_ARRAY_HEADER arr_hdr = {0};
    ret = read(fd, &arr_hdr, sizeof(arr_hdr));
    if (ret < 0) {
        perror("read");
        return errno;
    }

    for (size32_t i = 0; i < arr_hdr.itemcount; ++i) {
        ret = read(fd, &buf, sizeof(arr_hdr.typesize));
        if (ret < 0) {
            perror("read");
            return errno;
        }
        struct data_entry *entry = (struct data_entry *)(byte_t*)buf;
        printf("Entry%.4d: %4f, [%.4f %.4f %.4f], [%.4f %.4f %.4f]\n",
            i,
            entry->salt_conc,
            entry->gms_mass_before[0],
            entry->gms_mass_before[1],
            entry->gms_mass_before[2],
            entry->gms_mass_after[0],
            entry->gms_mass_after[1],
            entry->gms_mass_after[2]
        );
    }

    close(fd);

    return 0;
}