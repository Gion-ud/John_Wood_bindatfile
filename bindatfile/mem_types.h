#pragma once

#include <global_intdef.h>
#include <stdio.h>
#include "ser_types.h"
#include <stdbool.h>
#include <tlv/tlv.h>


enum KVDAT_OpenMode {
    MODE_CREATE    = 0,
    MODE_OPEN      = 1,
};

typedef struct _dat_file_obj {
    FILE                   *fp;             // inheritance doable
//  char                   *filename;       // must be inited n freed
//  FILE                   *fp_tmp;
//  char                   *filename_tmp;   // must be inited n freed
    DAT_FILE_HEADER         fileheader;
    DAT_FILE_FOOTER         filefooter;
//  DAT_INDEX_ENTRY        *indextable;
    uoff32_t               *offtable;
    size32_t                entrycap;
//  bool                    modified;
    uoff32_t                dat_file_cur;
} DAT_FILE_OBJECT;


