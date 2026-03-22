#pragma once

#include "recarrdat.h"
#include <stdio.h>

void DAT_FILE_print_fileheader(DAT_FILE_HEADER *fhdr_p) {
    byte_t *magic_p = (byte_t*)&fhdr_p->magic;
    printf(
        "%-31s %02X %02X %02X %02X\n"
        "%-31s 0x%.04x\n"
        "%-31s 0x%.04x\n"
        "%-31s %u\n"
        "%-31s %u\n"
        "%-31s %u\n"
        "%-31s %u\n"
        "%-31s %u\n"
        "%-31s 0x%.08x\n"
        "%-31s 0x%.08x\n"
        "%-31s ",
        "fileheader.magic", magic_p[0], magic_p[1], magic_p[2], magic_p[3],
        "fileheader.version", fhdr_p->version,
        "fileheader.flags", fhdr_p->flags,
        "fileheader.padsize", fhdr_p->padsize,
        "fileheader.entrycount", fhdr_p->entrycount,
        "fileheader.entrysize", fhdr_p->entrysize,
        "fileheader.headersize", fhdr_p->headersize,
        "fileheader.footersize", fhdr_p->footersize,
        "fileheader.datasectionoff", fhdr_p->datasectionoff,
        "fileheader.footeroff", fhdr_p->footeroff,
        "fileheader.timestamp"
    );
    struct tm *t = localtime((time_t*)&fhdr_p->timestamp);
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

void DAT_FILE_print_filefooter(DAT_FILE_FOOTER *fftr_p) {
    byte_t *magic_p = (byte_t*)&fftr_p->magic;
    printf(
        "%-31s 0x%.08x\n"
        "%-31s %02X %02X %02X %02X\n",
        "filefooter.crc32", fftr_p->crc32,
        "filefooter.magic", magic_p[0], magic_p[1], magic_p[2], magic_p[3]
    );
}
