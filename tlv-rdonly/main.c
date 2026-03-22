#include "datfile.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <posix_io.h>

#include "schema.h"

#define arr_elem_num(arr) (sizeof(arr) / sizeof(arr[0]))

byte_t _mem_buf[BUFFER_SIZE] = {0};
struct mem_buffer buf = {
    .mem        = (byte_t*)_mem_buf,
    .mem_cur    = 0,
    .mem_end    = sizeof(_mem_buf),
};

off32_t mem_buffer_copy(
    struct mem_buffer  *buf_p,
    void               *src,
    size32_t            src_len
) {
    if (is_null(buf_p) || is_null(src)) return -1;
    memcpy(buf_p->mem + buf_p->mem_cur, src, src_len);
    buf_p->mem_cur += src_len;
    return buf_p->mem_cur;
}


const char *str_arr[] = {
    "Hitsounds",
    "Hitsounds/drum",
    "Hitsounds/drum/hitclap",
    "Hitsounds/drum/hitfinish",
    "Hitsounds/drum/hitnormal",
    "Hitsounds/drum/hitwhistle",
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
    DAT_FILE_OBJECT d_obj = {
        .fd             = fd,
        .entrycount     = entrycount,
        .mem_buf        = &buf,
    };

    DAT_FILE_HEADER *fileheader_p   = FILEHEADER_PTR(d_obj.mem_buf->mem);
    DAT_INDEX_ENTRY *indextable_p   = INDEXTABLE_PTR(d_obj.mem_buf->mem);
    void            *datasection_p  = DATASECTION_PTR(d_obj.mem_buf->mem, d_obj.entrycount);

    puts("crash");
    for (int i = 0; i < entrycount; ++i) {
        mem_buffer_copy(d_obj.mem_buf, (void*)str_arr[i], strlen(str_arr[i]));
    }

    DAT_FILE_FOOTER *filefooter_p   = FILEFOOTER_PTR(d_obj.mem_buf->mem, d_obj.mem_buf->mem_cur);
    DAT_FILE_HEADER_fill(
        fileheader_p,
        0,
        d_obj.entrycount,
        d_obj.entrysize,
        d_obj.mem_buf->mem_cur
    );

    DAT_FILE_FOOTER_fill(filefooter_p, (byte_t*)d_obj.mem_buf->mem, d_obj.mem_buf->mem_cur);

    if (write_full(fd, buf.mem, buf.mem_cur) != buf.mem_cur) {
        perror("write");
        return errno;
    }

    close(fd);
}