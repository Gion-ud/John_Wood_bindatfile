#include "bindatfile/bindatfile.h"
#include <unistd.h>
#include "libc_chked.h"
#include <sys/stat.h>
#include "bindatfile/print.h"

#include "schema.h"
//static byte_t buffer[BUFFER_SIZE] = {0};

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


int main(void) {
    DAT_FILE_OBJECT d_obj = {0};
    FILE *dat_fp = fopen_checked("data.bin", "wb+");
    if (!dat_fp) return errno;

    if (DAT_FILE_OBJECT_init(&d_obj, dat_fp, 48, FILE_DEFAULT) < 0) {
        printerrf("DAT_FILE_OBJECT_init failed\n");
        return -1;
    }

    printerrf("init: success\n");

    DAT_ENTRY_HEADER entry_header = {0};

    for (size_t i = 0; i < entry_arr_len; ++i) {
        print_dbg_msg("Entry:%zu\n", i);
        entry_header.type = TYPE_BLOB;
        entry_header.len = sizeof(struct data_entry);
        if (DAT_FILE_write_entry(&d_obj, &entry_header, (byte_t*)&entry_arr[i]) < 0)
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

    DAT_FILE_OBJECT_deinit(&d_obj);
    fclose_checked(dat_fp);
    return 0;
}