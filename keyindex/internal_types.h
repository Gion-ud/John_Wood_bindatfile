#pragma once

#include <global_intdef.h>
#include <typeflags.h>
#include "ser_types.h"

enum ByteOrder {
    BYTE_ORDER_LE = 0,
    BYTE_ORDER_BE = 1,
};

#define VERSION_MAJOR   1
#define VERSION_MINOR   0
#define ENDIAN          BYTE_ORDER_LE

static const byte_t FILE_MAGIC[MAGIC_LEN] = { 0x01, 'K', ' ', 'I', 'D', 'X', '\n', '\0' };
static const byte_t EOF_MAGIC[MAGIC_LEN] = { 0xFF, '\n' , '.', 'E', 'O', 'F', '\n', 0x04 };
static const byte_t STRING_TABLE_MAGIC[MAGIC_LEN] = { '\n', '.', 'T', 'E', 'X', 'T', '\n', '\0' };
static const byte_t INDEX_TABLE_MAGIC[MAGIC_LEN] = { '\n', '.', 'I', 'N', 'D', 'E', 'X', '\0' };


#define MIN_ENTRY_CAP 16
