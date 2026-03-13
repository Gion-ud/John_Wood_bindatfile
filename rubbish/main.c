#include "bindatfile.h"




int main() {
    DAT_FILE_OBJECT d_obj = {0};
    DAT_init_fileheader(&d_obj.fileheader, FILE_DEFAULT);
    d_obj.filename      = "test.dat";
    d_obj.entrycap      = 16;
    d_obj.indextable    = (DAT_INDEX_ENTRY*)malloc_checked(d_obj.entrycap * sizeof(DAT_INDEX_ENTRY));
    if (!d_obj.indextable) return -1;
    d_obj.key_arr       = (LPBuffer*)malloc_checked(d_obj.entrycap * sizeof(LPBuffer));
    if (!d_obj.key_arr) return -1;

    return 0;
}
