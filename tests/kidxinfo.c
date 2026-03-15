#include <keyindex/keyindex.h>
#include <hash_index/hash_index.h>
#include <posix_io.h>
#include <libc_chked.h>
#include <keyindex/print.h>

byte_t buffer[BUFFER_SIZE] = {0};

int main(void) {
    FILE *fp = fopen_checked("index.dat", "rb+");
    if (!fp) return errno;
    INDEX_FILE_OBJECT i_obj = {0};
    int entrycnt = INDEX_FILE_get_entrycount(fp);
    int ret = INDEX_FILE_OBJECT_init(&i_obj, fp, entrycnt, 0);
    if (ret < 0) {
        printerrf("INDEX_FILE_OBJECT_init failed\n");
        fp = fclose_checked(fp);
        return -1;
    }
    ret = INDEX_FILE_load_sections(&i_obj);
    if (ret < 0) {
        printerrf("INDEX_FILE_load_sections failed\n");
        return -1;
    }

    printf("<fileheader at offset 0x%.8x>\n", 0);
    INDEX_FILE_print_fileheader(&i_obj.fileheader);
    printf("<filefooter at offset 0x%.8x>\n", i_obj.fileheader.footeroff);
    INDEX_FILE_print_filefooter(&i_obj.filefooter);
    printf("<indextable at offset 0x%.8x>\n", i_obj.fileheader.indextableoff);
    INDEX_FILE_print_indextable(i_obj.indextable, entrycnt);

    INDEX_FILE_load_keystringtable(&i_obj, (byte_t*)buffer);

    printf("<stringtable at offset 0x%.8x>\n", i_obj.fileheader.stringtableoff);
    puts("idx  string");
    char *str = NULL;
    size32_t str_len = 0;
    for (word_t i = 0; i < entrycnt; ++i) {
        str = (char*)INDEX_FILE_stringtable_get_key_rdonly(&i_obj, i, &str_len);
        printf("%.4u %.*s\n",
            i, (int)str_len, str
        );
    }
    putchar('\n');

    INDEX_FILE_OBJECT_deinit(&i_obj);
    fp = fclose_checked(fp);
    return 0;
}