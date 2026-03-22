#pragma once

#include <global_intdef.h>
#include <typeflags.h>
#include <compute_file_crc32.h>

#define VERSION     ((word_t)0x0200)
#define PAD_SIZE    4

#define PAD_OFF_ALIGN(off, pad_size) ((off + pad_size - 1) &~ (pad_size - 1))
#define PAD_ALIGN8(off, pad_size) ((off + 7) &~ 7)


enum FileFlags {
    FILE_DEFAULT    = 0,
    FILE_COMPRESSED = (1u << 0),
    FILE_ENCRYPTED  = (1u << 1),
};

enum EntryFlags {
    ENTRY_DEFAULT       = 0,
    ENTRY_DELETED       = (1u << 0),
    ENTRY_COMPRESSED    = (1u << 1),
    ENTRY_ENCRYPTED     = (1u << 2),
};

typedef struct _dat_file_header {
    dword_t     magic;          // 4
    word_t      version;        // 2
    word_t      flags;          // 2
    word_t      padsize;        // 2
    dword_t     entrycount;     // 4
    size32_t    headersize;     // 4
    size32_t    footersize;     // 4
    uoff32_t    indextableoff;  // 4
    uoff32_t    datasectionoff; // 4
    uoff32_t    footeroff;      // 4
    longlong_t  timestamp;      // 8
} __attribute__((packed)) DAT_FILE_HEADER;

typedef struct _dat_idx_entry {
    word_t      type;
    word_t      flags;
    size32_t    len;
    uoff32_t    off;
} __attribute__((packed)) DAT_INDEX_ENTRY;

typedef struct _dat_file_footer {
    ulong_t     crc32;  // 4
    dword_t     magic;  // 8
} __attribute__((packed)) DAT_FILE_FOOTER;


struct mem_buffer {
    void       *mem;
    uoff32_t    mem_cur;
    uoff32_t    mem_end;
};

typedef struct _dat_file_object {
    int                 fd;
    struct mem_buffer  *mem_buf;
    uoff32_t            indextableoff;
    uoff32_t            datasectionoff;
    uoff32_t            footeroff;
    word_t              entrycount;
    size32_t            entrysize;
} DAT_FILE_OBJECT;

static const byte_t *magic_bytes = (byte_t*)(byte_t[]) { 0xFF, 'D', 'A', 'T' };
static const byte_t *eof_magic_bytes = (byte_t*)(byte_t[]) { '.', 'E', 'O', 'F' };


static inline uoff32_t get_indextableoff() {
    return PAD_OFF_ALIGN(sizeof(DAT_FILE_HEADER), PAD_SIZE);
}
static inline uoff32_t get_datasectionoff(word_t entrycount) {
    return get_indextableoff() + PAD_OFF_ALIGN(entrycount * sizeof(DAT_INDEX_ENTRY), PAD_SIZE);
}
static inline uoff32_t get_footeroff(uoff32_t buf_end_cur) {
    return PAD_OFF_ALIGN(buf_end_cur, PAD_SIZE);
}

static inline uoff32_t get_filesize(uoff32_t buf_end_cur) {
    return
        get_footeroff(buf_end_cur) + sizeof(DAT_FILE_FOOTER);
}

#define FILEHEADER_PTR(base_addr) \
    (DAT_FILE_HEADER*)(base_addr)

#define INDEXTABLE_PTR(base_addr) \
    (DAT_INDEX_ENTRY*)((byte_t*)base_addr + get_indextableoff())

#define DATASECTION_PTR(base_addr, n_entry) \
    (void*)((byte_t*)base_addr + get_datasectionoff(n_entry))

#define FILEFOOTER_PTR(base_addr, buf_cur) \
    (DAT_FILE_FOOTER*)((byte_t*)base_addr + get_footeroff(buf_cur))


void DAT_FILE_HEADER_fill(
    DAT_FILE_HEADER    *f_hdr_p,
    word_t              flags,
    word_t              entrycount,
    size32_t            entrysize,
    uoff32_t            footeroff
) {
    if (!f_hdr_p) return;
    *f_hdr_p = (DAT_FILE_HEADER) {
        .magic          = *(dword_t*)magic_bytes,
        .version        = VERSION,
        .flags          = flags,
        .padsize        = PAD_SIZE,
        .entrycount     = entrycount,
        .headersize     = sizeof(DAT_FILE_HEADER),
        .footersize     = sizeof(DAT_FILE_FOOTER),
        .indextableoff  = get_indextableoff(),
        .datasectionoff = PAD_OFF_ALIGN(sizeof(DAT_FILE_HEADER), PAD_SIZE),
        .footeroff      = footeroff,
        .timestamp      = (time_t)time(NULL),
    };
}

void DAT_FILE_FOOTER_fill(
    DAT_FILE_FOOTER    *f_ftr_p,
    byte_t             *buf,
    size32_t            footeroff
) {
    if (!f_ftr_p) return;
    f_ftr_p->crc32 = compute_mem_crc32(buf, footeroff);
    f_ftr_p->magic = *(dword_t*)eof_magic_bytes;
}