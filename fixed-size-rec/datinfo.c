#include "recarrdat.h"
#include "prdatinfo.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include "schema.h"

const char *filename = "data.bin";

int main(void) {
    int fd = open(filename,
        O_RDONLY
#ifdef _WIN32
        | O_BINARY
#endif
    );
    if (fd < 0) {
        perror("open");
        return errno;
    }
    struct stat st = {0};
    fstat(fd, &st);

    void *f_mem = mmap(
        NULL,
        st.st_size,
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0
    );
    if (f_mem == MAP_FAILED) {
        perror("mmap");
        return errno;
    }

    DAT_FILE_HEADER *fileheader_p = (DAT_FILE_HEADER*)f_mem;
    void *data_section_p = (void*)((byte_t*)f_mem + fileheader_p->datasectionoff);
    DAT_FILE_FOOTER *filefooter_p = (DAT_FILE_FOOTER*)((byte_t*)f_mem + fileheader_p->footeroff);

    if (filefooter_p->crc32 != compute_mem_crc32(f_mem, fileheader_p->footeroff)) {
        printerrf("dat file corrupted: crc32 mismatch\n");
        munmap(f_mem, st.st_size);
        close(fd);
        return -1;
    }

    printf("<fileheader @ offset 0x%.8x>\n", 0);
    DAT_FILE_print_fileheader(fileheader_p);
    printf("<filefooter @ offset 0x%.8x>\n", fileheader_p->footeroff);
    DAT_FILE_print_filefooter(filefooter_p);



    munmap(f_mem, st.st_size);
    close(fd);
    return 0;
}