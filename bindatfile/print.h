#pragma once

#include "mem_types.h"
#include <posix_io.h>

static inline void DAT_FILE_print_fileheader(DAT_FILE_HEADER *header_p) {
    if (!header_p) return;
    byte_t *magic_bytes = (byte_t*)(&header_p->magic);
    char buffer[1024] = {0};
    off32_t cur = 0;
/*
    cur += snprintf(
        (char*)buffer + cur, sizeof(buffer) - cur,
        "\n"
        "# file header\n"
    );
*/
    cur += snprintf(
        (char*)buffer + cur, sizeof(buffer) - cur,
        "%-27s %-4s    %s\n"
        "%-27s %4zu    %02X %02X %02X %02X %02X %02X %02X %02X\n"
        "%-27s %4zu    %d\n"
        "%-27s %4zu    %d\n"
        "%-27s %4zu    0x%.4x\n"
        "%-27s %4zu    %u\n"
        "%-27s %4zu    %u\n"
        "%-27s %4zu    %u\n"
        "%-27s %4zu    0x%08x\n"
        "%-27s %4zu    0x%08x\n"
        "%-27s %4zu    0x%08x\n"
        "%-27s %4zu    ",
        "field_name", "size", "value",
        "fileheader.magic", sizeof(header_p->magic), magic_bytes[0],magic_bytes[1],magic_bytes[2],magic_bytes[3],magic_bytes[4],magic_bytes[5],magic_bytes[6],magic_bytes[7],
        "fileheader.version_major", sizeof(header_p->version_major), header_p->version_major,
        "fileheader.version_minor", sizeof(header_p->version_minor), header_p->version_minor,
        "fileheader.flags", sizeof(header_p->flags), header_p->flags,
        "fileheader.headersize", sizeof(header_p->headersize), header_p->headersize,
        "fileheader.footersize", sizeof(header_p->footersize), header_p->footersize,
        "fileheader.entrycount", sizeof(header_p->entrycount), header_p->entrycount,
        "fileheader.offtableoff", sizeof(header_p->offtableoff), header_p->offtableoff,
        "fileheader.datasectionoff", sizeof(header_p->datasectionoff), header_p->datasectionoff,
        "fileheader.footeroff", sizeof(header_p->footeroff), header_p->footeroff,
        "fileheader.timestamp", sizeof(header_p->timestamp)
    );

    struct tm *t = localtime((time_t*)&header_p->timestamp);
    cur += snprintf(
        (char*)buffer + cur, sizeof(buffer) - cur,
        "%04d-%02d-%02d %02d:%02d:%02d\n\n",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec
    );
    int ret = write(STDOUT_FILENO, (char*)buffer, cur);
    if (ret < 0) perror("write");
}
static inline void DAT_FILE_print_filefooter(DAT_FILE_FOOTER *footer_p) {
    if (!footer_p) return;
    byte_t *magic_bytes = (byte_t*)(&footer_p->magic);
    char buffer[256] = {0};
    off32_t cur = 0;

    cur += snprintf(
        (char*)buffer + cur, sizeof(buffer) - cur,
        "%-27s %-4s    %s\n"
        "%-27s %4zu    0x%.08x\n"
        "%-27s %4zu    %02X %02X %02X %02X %02X %02X %02X %02X\n\n",
        "field_name", "size", "value",
        "filefooter.crc32", sizeof(footer_p->crc32), footer_p->crc32,
        "filefooter.magic", sizeof(footer_p->magic), magic_bytes[0],magic_bytes[1],magic_bytes[2],magic_bytes[3],magic_bytes[4],magic_bytes[5],magic_bytes[6],magic_bytes[7]
    );

    int ret = write(STDOUT_FILENO, (char*)buffer, cur);
    if (ret < 0) perror("write");
}
static inline void DAT_FILE_print_offtable(uoff32_t *offtable, size32_t entrycnt) {
    if (!offtable) return;
    puts("index    offset");
    char line[32] = {0};
    off32_t len = 0;
    int ret = 0;
    for (size32_t i = 0; i < entrycnt; ++i) {
        if (!offtable[i]) {
            len = snprintf((char*)line, sizeof(line), "%5u    (NULL)\n", i);
        } else {
            len = snprintf((char*)line, sizeof(line), "%5u    0x%.08x\n", i, offtable[i]);
        }
        ret = write(STDOUT_FILENO, (char*)line, len);
        if (ret < 0) perror("write");
    }
    ret = write(STDOUT_FILENO, (char*)"\n", 1);
    if (ret < 0) perror("write");
}
