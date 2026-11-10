#ifndef WINLOAD_H_
#define WINLOAD_H_

#include <Uefi.h>

// https://discord.gg/sourcesheaven

#define MOD_FROM_LINK(p, t, f) ((t *)((CHAR8 *)(p) - (UINTN)(&((t *)0)->f)))

typedef struct {
    UINT16 len;
    UINT16 cap;
    CHAR16 *p;
} wide_t;

typedef struct {
    LIST_ENTRY link;
    VOID *xptr;
    UINT32 xsz;
    VOID *gp;
    VOID *dbg;
    VOID *base;
    VOID *entry;
    UINT32 imgsz;
    wide_t full;
    wide_t name;
    UINT32 flags;
    UINT16 loads;
    UINT16 sig;
    VOID *sect;
    UINT32 csum;
    UINT32 covsz;
    VOID *cov;
    VOID *imp;
    VOID *spare;
    UINT32 rawsz;
    UINT32 stamp;
} mod_ent;

typedef struct {
    UINT32 maj;
    UINT32 min;
    UINT32 size;
    UINT32 secver;
    LIST_ENTRY load;
    LIST_ENTRY mem;
    LIST_ENTRY bootdrv;
    LIST_ENTRY elam;
    LIST_ENTRY core;
    LIST_ENTRY corex;
    LIST_ENTRY tpm;
    UINT64 kstack;
    UINT64 prcb;
    UINT64 proc;
    UINT64 th;
    UINT32 kssz;
    UINT32 regl;
    VOID *reg;
    VOID *cfg;
    CHAR8 *arc_boot;
    CHAR8 *arc_hal;
    CHAR8 *nt_boot;
    CHAR8 *nt_hal;
    CHAR8 *opt;
    VOID *nls;
    VOID *disk;
    VOID *ext;
    CHAR8 *stat;
    CHAR8 *osdata;
    CHAR8 *win;
} ldr_block_t;

#endif
