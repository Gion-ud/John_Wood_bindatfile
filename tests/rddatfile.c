#include "bindatfile/bindatfile.h"
#include <unistd.h>
#include "libc_chked.h"
#include <sys/stat.h>
#include "bindatfile/print.h"

#include "schema.h"
static byte_t buffer[BUFFER_SIZE] = {0};

#define progname argv[0]
#define filename argv[1]

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printerrf("Error: invalid argc (argc=%d)\n", argc);
        printerrf("Usage: %s <filename>\n", progname);
        return -1;
    }
    if (!filename[0]) {
        printerrf("filename cannot be empty\n");
        return -1;
    }

    DAT_FILE_OBJECT d_obj = {0};
    FILE *fp = fopen_checked(filename, "rb+");
    if (!fp) return errno;
    int entrycnt = DAT_get_datfile_entrycount(fp);
    //printf("entrycnt=0x%.8x\n", entrycnt);
    if (entrycnt < 0) {
        printerrf("DAT_get_datfile_entrycount failed\n");
        return -1;
    }
    if (DAT_FILE_OBJECT_init(&d_obj, fp, entrycnt, FILE_DEFAULT) < 0) {
        printerrf("DAT_FILE_OBJECT_init failed\n");
        DAT_FILE_OBJECT_deinit(&d_obj);
        return -1;
    }
    if (!DAT_FILE_load_sections(&d_obj)) {
        printerrf("DAT_FILE_load_sections failed\n");
        DAT_FILE_OBJECT_deinit(&d_obj);
        return -1;
    }
    if (!DAT_FILE_validate_integrity(&d_obj.fileheader, &d_obj.filefooter, d_obj.fp)) {
        printerrf("DAT_FILE_validate_integrity failed: crc mismatch\n");
        DAT_FILE_OBJECT_deinit(&d_obj);
        return -1;
    }


    putchar('\n');
    struct data_entry *entry = NULL;
    DAT_ENTRY_HEADER entryhdr = {0};
    for (int i = 0; i < entrycnt; ++i) {
        if (DAT_FILE_read_entry_header(&d_obj, i, &entryhdr) < 0) return -1;
        if (DAT_FILE_read_entry_data(&d_obj, i, entryhdr.len, buffer) < 0) return -1;
        entry = (struct data_entry *)(byte_t*)buffer;
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

    DAT_FILE_OBJECT_deinit(&d_obj);
    fclose_checked(fp);
    return 0;
}