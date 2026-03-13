#include <libc_chked.h>
#include "bindatfile/bindatfile.h"
#include "bindatfile/print.h"
#include <sys/stat.h>
#include <unistd.h>


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
    if (entrycnt < 0) {
        printerrf("DAT_get_datfile_entrycount failed\n");
        return -1;
    }
    if (DAT_FILE_OBJECT_init(&d_obj, fp, entrycnt, FILE_DEFAULT) < 0) {
        printerrf("DAT_FILE_OBJECT_init failed\n");
        return -1;
    }
    if (!DAT_FILE_load_sections(&d_obj)) {
        printerrf("DAT_FILE_load_sections failed\n");
        return -1;
    }
    if (!DAT_FILE_validate_integrity(&d_obj.fileheader, &d_obj.filefooter, d_obj.fp)) {
        printerrf("DAT_FILE_validate_integrity failed: crc mismatch\n");
        return -1;
    }

    printf("\n<fileheader at offset 0x%.08x>\n", 0);
    DAT_FILE_print_fileheader(&d_obj.fileheader);
    printf("<filefooter at offset 0x%.08x>\n", d_obj.fileheader.footeroff);
    DAT_FILE_print_filefooter(&d_obj.filefooter);
    printf("<offtable at offset 0x%.08x>\n", d_obj.fileheader.offtableoff);
    DAT_FILE_print_offtable(d_obj.offtable, d_obj.fileheader.entrycount);


    DAT_FILE_OBJECT_deinit(&d_obj);
    fclose(fp);
    return 0;
}