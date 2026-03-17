#include "bindatfile/bindatfile.h"
#include <unistd.h>
#include "libc_chked.h"
#include <sys/stat.h>
#include "bindatfile/print.h"

static byte_t buffer[BUFFER_SIZE] = {0};
static off32_t buffer_cur = 0;
//static off32_t buffer_end = BUFFER_SIZE;

int main(void) {
    DAT_FILE_OBJECT d_obj = {0};
    FILE *dat_fp = fopen_checked("schema.dat", "wb+");
    if (!dat_fp) return errno;

    if (DAT_FILE_OBJECT_init(&d_obj, dat_fp, 1, FILE_DEFAULT) < 0) {
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




    DAT_FILE_OBJECT_commit(&d_obj);
    DAT_FILE_OBJECT_deinit(&d_obj);
    fclose_checked(dat_fp);
    return 0;
}