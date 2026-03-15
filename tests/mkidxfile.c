#include <keyindex/keyindex.h>
#include <hash_index/hash_index.h>
#include <posix_io.h>
#include <libc_chked.h>

const char *key_arr[] = {
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
const size_t key_arr_len = sizeof(key_arr) / sizeof(key_arr[0]);


int main() {
    FILE *fp = fopen_checked("index.dat", "wb+");
    if (!fp) return errno;
    INDEX_FILE_OBJECT i_obj = {0};
    INDEX_FILE_OBJECT_init(&i_obj, fp, 32, 0);
    HASH_INDEX_OBJECT ht_obj = {0};
    HASH_INDEX_OBJECT_init(&ht_obj, 32, 16);

    int ret = 0;
    hash_t key_hash = 0;
    size32_t key_len = 0;
    for (size_t i = 0; i < key_arr_len; ++i) {
        key_len = strlen(key_arr[i]);
        key_hash = hash((byte_t*)key_arr[i], key_len);
        ret = INDEX_FILE_write_entry(&i_obj, (byte_t*)key_arr[i], key_len, key_hash, 0, 0);
        if (ret < 0) {
            printerrf("INDEX_FILE_write_entry failed\n");
            continue;
        }
        ret = HASH_INDEX_insert(&ht_obj, key_hash, i);
        if (ret < 0) {
            printerrf("HASH_INDEX_insert failed\n");
        }
        printf("key%.4zu=%s\n", i, key_arr[i]);
    }
    INDEX_FILE_OBJECT_commit(&i_obj);


    HASH_INDEX_OBJECT_deinit(&ht_obj);
    INDEX_FILE_OBJECT_deinit(&i_obj);
    fp = fclose_checked(fp);
    return 0;
}