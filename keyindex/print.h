#pragma once

#include "ser_types.h"
#include <stdio.h>
#include <time.h>

static inline void INDEX_FILE_print_fileheader(INDEX_FILE_HEADER *fileheader_p) {
    byte_t *magic_bytes = (byte_t*)&fileheader_p->magic;
    printf(
        "%-27s %4s    %s\n"
        "%-27s %4zu    %02X %02X %02X %02X %02X %02X %02X %02X\n"
        "%-27s %4zu    %u\n"
        "%-27s %4zu    %u\n"
        "%-27s %4zu    0x%.4x\n"
        "%-27s %4zu    %u\n"
        "%-27s %4zu    %u\n"
        "%-27s %4zu    0x%.8x\n"
        "%-27s %4zu    0x%.8x\n"
        "%-27s %4zu    0x%.8x\n"
        "%-27s %4zu    %u\n"
        "%-27s %4zu    ",
        "field_name", "size", "value",
        "fileheader.magic", sizeof(fileheader_p->magic), magic_bytes[0], magic_bytes[1], magic_bytes[2], magic_bytes[3], magic_bytes[4], magic_bytes[5], magic_bytes[6], magic_bytes[7],
        "fileheader.version_major", sizeof(fileheader_p->version_major), fileheader_p->version_major,
        "fileheader.version_minor", sizeof(fileheader_p->version_minor), fileheader_p->version_minor,
        "fileheader.flags", sizeof(fileheader_p->flags), fileheader_p->flags,
        "fileheader.headersize", sizeof(fileheader_p->headersize), fileheader_p->headersize,
        "fileheader.footersize", sizeof(fileheader_p->footersize), fileheader_p->footersize,
        "fileheader.stringtableoff", sizeof(fileheader_p->stringtableoff), fileheader_p->stringtableoff,
        "fileheader.indextableoff", sizeof(fileheader_p->indextableoff), fileheader_p->indextableoff,
        "fileheader.footeroff", sizeof(fileheader_p->footeroff), fileheader_p->footeroff,
        "fileheader.entrycount", sizeof(fileheader_p->entrycount), fileheader_p->entrycount,
        "fileheader.timestamp", sizeof(fileheader_p->timestamp)
    );
    //fflush(stdout);
    struct tm *t = localtime((time_t*)&fileheader_p->timestamp);
    printf(
        "%04d-%02d-%02d %02d:%02d:%02d\n\n",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec
    );
}
static inline void INDEX_FILE_print_filefooter(INDEX_FILE_FOOTER *filefooter_p) {
    byte_t *magic_bytes = (byte_t*)&filefooter_p->magic;
    printf(
        "%-27s %4s    %s\n"
        "%-27s %4zu    0x%.8x\n"
        "%-27s %4zu    %02X %02X %02X %02X %02X %02X %02X %02X\n\n",
        "field_name", "size", "value",
        "filefooter.crc32", sizeof(filefooter_p->crc32), filefooter_p->crc32,
        "filefooter.magic", sizeof(filefooter_p->magic), magic_bytes[0], magic_bytes[1], magic_bytes[2], magic_bytes[3], magic_bytes[4], magic_bytes[5], magic_bytes[6], magic_bytes[7]
    );
}

static inline void INDEX_FILE_print_indexentry(INDEX_ENTRY *indextable, int idx) {
    printf(
        "%-27s %4s    %s\n"
        "indexentry%.4d.%-12s %4zu    0x%.8x\n"
        "indexentry%.4d.%-12s %4zu    %u\n"
        "indexentry%.4d.%-12s %4zu    0x%.8x\n"
        "indexentry%.4d.%-12s %4zu    0x%.8x\n"
        "indexentry%.4d.%-12s %4zu    0x%.4x\n\n",
        "field_name", "size", "value",
        idx, "key_hash", sizeof(indextable[idx].key_hash), indextable[idx].key_hash,
        idx, "key_len",  sizeof(indextable[idx].key_len),  indextable[idx].key_len,
        idx, "key_off",  sizeof(indextable[idx].key_off),  indextable[idx].key_off,
        idx, "data_off", sizeof(indextable[idx].data_off), indextable[idx].data_off,
        idx, "flags",    sizeof(indextable[idx].flags),    indextable[idx].flags
    );
}

static inline void INDEX_FILE_print_indextable(INDEX_ENTRY *indextable, word_t entrycount) {
    printf(
        "%-4s    %-10s    %-7s    %-10s    %-10s    %-6s\n",
        "idx", "key_hash", "key_len", "key_off", "data_off", "flags"
    );
    for (word_t idx = 0; idx < entrycount; ++idx)
        printf(
            "%.4d    "
            "0x%.8x    "
            "%7u    "
            "0x%.8x    "
            "0x%.8x    "
            "0x%.4x\n",
            idx,
            indextable[idx].key_hash,
            indextable[idx].key_len,
            indextable[idx].key_off,
            indextable[idx].data_off,
            indextable[idx].flags
        );
    putchar('\n');
}