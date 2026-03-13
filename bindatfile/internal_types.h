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

static const byte_t FILE_MAGIC[MAGIC_LEN] = { 0x01, 'B', 'D', 'A', 'T', '\r', '\n', '\0' };
static const byte_t EOF_MAGIC[MAGIC_LEN] = { '\r', '\n', 0x1A , '.', 'E', 'O', 'F', 0x04 };

static const byte_t DATA_SECTION_MAGIC[MAGIC_LEN] = { '.', 'D', 'A', 'T', 'A', '\r', '\n', 0x1F };
static const byte_t ENTRY_MAGIC[MAGIC_LEN] = { '.', 'E', 'N', 'T', 'R', 'Y', '\r', '\n' };

#define MIN_ENTRY_CAP 16
