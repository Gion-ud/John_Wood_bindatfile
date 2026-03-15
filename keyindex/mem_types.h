#pragma once

#include <global_intdef.h>
#include <stdio.h>
#include "ser_types.h"
#include <stdbool.h>
#include <tlv/tlv.h>


typedef struct _dat_file_obj {
    FILE                   *fp;
    INDEX_FILE_HEADER       fileheader;
    INDEX_FILE_FOOTER       filefooter;
    INDEX_ENTRY            *indextable;
    LPBuffer                key_stringtable_buf;
    size32_t                entrycap;
    uoff32_t                idx_file_cur;
} INDEX_FILE_OBJECT;

