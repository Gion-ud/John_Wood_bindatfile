#include "recarrdat.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include "schema.h"

/*
static byte_t _mem_buf[BUFFER_SIZE * 4] = {0};
struct mem_buffer buf = {
    .mem        = (byte_t*)_mem_buf,
    .mem_cur    = 0,
    .mem_end    = sizeof(_mem_buf),
};
*/

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

const word_t entrycount = sizeof(entry_arr) / sizeof(entry_arr[0]);
const size32_t entrysize = sizeof(struct data_entry);
const char *filename = "data.bin";

int main(void) {
    int fd = open(filename,
        O_RDWR | O_CREAT
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
    DAT_FILE_OBJECT d_obj = {
        .fd             = fd,
        .entrycount     = entrycount,
        .entrysize      = sizeof(struct data_entry),
        .datasectionoff = get_datasectionoff(),
        .footeroff      = get_footeroff(entrycount, entrysize),
        .mem_buf        = NULL,
        .mem_cur        = 0,
        .mem_size       = get_filesize(entrycount, entrysize),
    };

    if (ftruncate(fd, d_obj.mem_size) != 0) {
        printerrf("ftruncate");
        return errno;
    }
    d_obj.mem_buf = mmap(
        NULL,
        d_obj.mem_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );
    if (d_obj.mem_buf == MAP_FAILED) {
        perror("mmap");
        return errno;
    }

    void *data_section_p = (void*)((byte_t*)d_obj.mem_buf + d_obj.datasectionoff);
    memcpy(data_section_p, &entry_arr, sizeof(entry_arr));

    DAT_FILE_HEADER *fileheader_p = (DAT_FILE_HEADER *)d_obj.mem_buf;
    DAT_FILE_HEADER_fill(
        fileheader_p,
        0,
        d_obj.entrycount,
        d_obj.entrysize,
        d_obj.footeroff
    );
    DAT_FILE_FOOTER *filefooter_p = (DAT_FILE_FOOTER *)((byte_t*)d_obj.mem_buf + d_obj.footeroff);
    DAT_FILE_FOOTER_fill(filefooter_p, d_obj.mem_buf, d_obj.footeroff);
    msync(d_obj.mem_buf, d_obj.mem_size, MS_ASYNC);
    munmap(d_obj.mem_buf, d_obj.mem_size);
    close(fd);
}