#include "datfile.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <posix_io.h>
#include <stdio.h>

#include "schema.h"

byte_t mem_buf[BUFFER_SIZE] = {0};

const char *filename = "data.bin";

int main(void) {
    int fd = open(filename,
        O_RDONLY
#ifdef _WIN32
        | O_BINARY
#endif
        ,
        0644
    );
    if (fd < 0) {
        perror("open");
        return errno;
    }

    off32_t filesize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if (read_full(fd, &mem_buf, filesize) != filesize) return errno;

    DAT_FILE_HEADER *fileheader_p = FILEHEADER_PTR(&mem_buf);
    DAT_FILE_FOOTER *filefooter_p = FILEFOOTER_PTR(&mem_buf, fileheader_p->footeroff);
    dword_t entrycount = fileheader_p->entrycount;

    if (fileheader_p->magic != *(dword_t*)magic_bytes) {
        printerrf("file magic mismatch\n");
        close(fd);
        return -1;
    }
    if (compute_mem_crc32(&mem_buf, fileheader_p->footeroff) != filefooter_p->crc32) {
        printerrf("file corrupted: crc32 mismatch\n");
        close(fd);
        return -1;
    }
    if (filefooter_p->magic != *(dword_t*)eof_magic_bytes) {
        printerrf("eof magic mismatch\n");
        close(fd);
        return -1;
    }

    struct mem_buffer indextable_inmem, datasection_inmem;
    indextable_membuf_init(&indextable_inmem, mem_buf, entrycount);
    datasection_membuf_init(&datasection_inmem, mem_buf, sizeof(mem_buf), entrycount);


    DAT_INDEX_ENTRY indexentry = {0};
    size_t str_len = 0;

    char str[256] = {0};
    puts("\n\t\ttype\tflags\tlen\toff\tpayload");
    for (int i = 0; i < entrycount; ++i) {
        memcpy(
            &indexentry,
            (byte_t*)mem_buf + fileheader_p->indextableoff + i * sizeof(DAT_INDEX_ENTRY),
            sizeof(DAT_INDEX_ENTRY)
        );
        memcpy(str, (byte_t*)mem_buf + indexentry.off, indexentry.len);
        printf(
            "Entry%.4u:\t0x%.4x\t0x%.4x\t%u\t0x%.8x\t",
            i, indexentry.type, indexentry.flags, indexentry.len, indexentry.off
        );
        printf("%.*s\n", indexentry.len, str);
    }

    close(fd);
    return 0;
}