#include <global.h>
#include <libc_chked.h>
#include <tlv/tlv.h>
#include <hash_index/hash_index.h>
#include <posix_io.h>

const char *key_arr[] = {
    ".data",
    ".text",
    ".rdata",
    ".rodata",
    ".pdata",
    ".xdata",
    ".idata",
    ".reloc",
    ".bss",
    "DAT",
    "PAK",
    ".EOF",
    ".index",
    "open",
    "read",
    "write",
    "fork",
    "mmap",
    "header",
    "footer",
    "libc",
    "malloc",
    "printf",
    "calloc",
    "exit",
    "atexit",
    "strlen",
    "memset",
    "memcpy",
    "abort",
    "_errno",
    "libc.so.6",
    "_exit",
    "ntdll",
    "free",
    "KERNEL32.DLL",
    "vcruntime140.dll",
    "GCC",
    "ELF",
    "GNU",
    "msvcrt.dll",
    "ntoskrnl.exe",
    "NtWriteFile",
    "/usr/bin/bash"
};
const size_t key_count = sizeof(key_arr) / sizeof(key_arr[0]);

int HASH_INDEX_get_key_idx(
    HASH_INDEX_OBJECT   *this,
    const char          *key_arr[],
    char                *key
) {
    size_t key_len  = strlen(key);
    hash_t key_hash = this->ht_ops->hash((byte_t*)key, key_len);
    int    h_idx    = key_hash & (this->ht_size - 1);

    size32_t probe_cnt = 0;
    while (
        probe_cnt < this->ht_size &&
        this->ht_key_idx_arr[h_idx]
    ) {
        if (memcmp(key, key_arr[this->ht_key_idx_arr[h_idx]], key_len) == 0)
            return this->ht_key_idx_arr[h_idx];
        ++probe_cnt; ++h_idx;
        h_idx &= (this->ht_size - 1);
    }
    return -1;
}

int main() {
    printf("key_count=%zu\n", key_count);
    int ret = write(STDOUT_FILENO, "key table:\n", 11);
    puts(" idx  len    data");
    for (size_t i = 0; i < key_count; ++i) {
        printf("%4zu %4u    %s\n",
            i, (ulong_t)strlen(key_arr[i]), (char*)key_arr[i]
        );
    }
    putchar('\n');

    HASH_INDEX_OBJECT ht_obj = {0};
    ret = HASH_INDEX_OBJECT_init(&ht_obj, 64, 32);
    if ((unsigned int)ret & (1u << (sizeof(int) * 8 - 1)))
        return -1;

    hash_t key_hash = 0;
    for (size_t i = 0; i < key_count; ++i) {
        key_hash = ht_obj.ht_ops->hash((byte_t*)key_arr[i], strlen(key_arr[i]));
        ret = ht_obj.ht_ops->insert(&ht_obj, key_hash, i);
        if (ret < 0) {
            printerrf("ht_obj.ht_ops->insert failed\n");
            continue;
        }
        //printf("key_hash=0x%.16llx;h_idx=%d;h_ret=%d\n", key_hash, (int)key_hash % ht_obj.ht_size, ret);
    }

    size_t key_len = 0;
    char key_buffer[64] = {0};
    while (LOOP_RUNNING) {
        puts("Enter key:");
        if (!fgets((char*)key_buffer, 64, stdin)) {
            printerrf("fgets\n");
            break;
        }
        key_len = strlen(key_buffer);

        if (key_buffer[key_len - 1] == '\n') key_buffer[key_len-- - 1] = 0;
        if (key_buffer[key_len - 1] == '\r') key_buffer[key_len-- - 1] = 0;
        if (!key_buffer[0]) break;
        ret = HASH_INDEX_get_key_idx(&ht_obj, key_arr, (char*)key_buffer);
        printf("idx:%d\n", ret);
    }


    HASH_INDEX_OBJECT_deinit(&ht_obj);

    return 0;
}