#include "datfile.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <posix_io.h>
#include <stdio.h>

byte_t mem_buf[BUFFER_SIZE] = {0};

const char *str_arr[] = {
    "Hitsounds",
    "Hitsounds/drum",
    "Hitsounds/drum/hitclap",
    "Hitsounds/drum/hitfinish",
    "Hitsounds/drum/hitnormal",
    "Hitsounds/drum/hitwhistle",
    "Hitsounds/normal",
    "Hitsounds/normal/hitclap",
    "Hitsounds/normal/hitfinish",
    "Hitsounds/normal/hitnormal",
    "Hitsounds/normal/hitwhistle",
    "Hitsounds/soft",
    "Hitsounds/soft/hitclap",
    "Hitsounds/soft/hitfinish",
    "Hitsounds/soft/hitnormal",
    "Hitsounds/soft/hitwhistle",
};


const word_t entrycount = arr_elem_num(str_arr);
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

    struct mem_buffer indextable_inmem, datasection_inmem;
    indextable_membuf_init(&indextable_inmem, mem_buf, entrycount);
    datasection_membuf_init(&datasection_inmem, mem_buf, sizeof(mem_buf), entrycount);

    DAT_INDEX_ENTRY indexentry = {0};
    size_t str_len = 0;

    for (int i = 0; i < entrycount; ++i) {
        str_len = strlen(str_arr[i]);
        indexentry = (DAT_INDEX_ENTRY) {
            .type   = TYPE_TEXT,
            .flags  = ENTRY_OCCUPIED,
            .len    = str_len,
            .off    = get_datasectionoff(entrycount) + datasection_inmem.mem_cur,
        };
        mem_buffer_copy(&indextable_inmem, &indexentry, sizeof(DAT_INDEX_ENTRY));
        mem_buffer_copy(&datasection_inmem, (void*)str_arr[i], str_len);
    }

    uoff32_t filefooter_off = get_footeroff(entrycount, datasection_inmem.mem_cur);

    DAT_FILE_HEADER *fileheader_p = FILEHEADER_PTR(&mem_buf);
    DAT_FILE_FOOTER *filefooter_p = FILEFOOTER_PTR(&mem_buf,filefooter_off);
    DAT_FILE_HEADER_fill(
        fileheader_p,
        0,
        entrycount,
        filefooter_off
    );
    DAT_FILE_FOOTER_fill(filefooter_p, (byte_t*)&mem_buf, filefooter_off);

    size32_t filesize = get_filesize(filefooter_off);
    if (write_full(fd, &mem_buf, filesize) != filesize) {
        perror("write did not return total filesize");
        return errno;
    }

    close(fd);
}

