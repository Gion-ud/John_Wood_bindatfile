#include <bindatfile/bindatfile.h>
#include <unistd.h>
#include <libc_chked.h>
#include <sys/stat.h>
#include <bindatfile/print.h>
#include <packed_ser_types.h>

static byte_t buffer[BUFFER_SIZE] = {0};
static off32_t buffer_cur = 0;
//static off32_t buffer_end = BUFFER_SIZE;

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

    FILE *dat_fp = fopen_checked("schema.dat", "rb+");
    if (!dat_fp) return errno;
    int dat_fd = fileno(dat_fp);

    struct stat st;
    fstat(dat_fd, &st);
    if (
        read(dat_fd, (byte_t*)buffer, st.st_size) < 0
    )
        return errno;

    size32_t field_count = *(size32_t*)(byte_t*)buffer;
    buffer_cur += sizeof(size32_t);
    struct packed_struct_field_header *field_header_p = NULL;

    char *str = (char*)buffer + sizeof(size32_t) +
        field_count * sizeof(struct packed_struct_field_header);

    puts("field_type\tfield_size\tfield_off\tfield_name\n");
    for (size32_t i = 0; i < field_count; ++i) {
        field_header_p = (struct packed_struct_field_header *)((byte_t*)buffer + buffer_cur);
        printf(
            "0x%.4x\t%4u\t%4u\t%s\n",
            field_header_p->field_type,
            field_header_p->field_size,
            field_header_p->field_off,
            str
        );
        buffer_cur += sizeof(struct packed_struct_field_header);
        str += strlen(str) + 1;
    }


    fclose_checked(dat_fp);
    return 0;
}