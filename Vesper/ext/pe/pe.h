#ifndef PE_H_
#define PE_H_

#include <Uefi.h>

// https://discord.gg/sourcesheaven

#define DOS_MAGIC  0x5A4Du
#define NT_MAGIC   0x00004550u
#define DIR_BASEREL  5u
#define REL_DIR64    10u

#pragma pack(push, 1)

typedef struct {
    UINT16 magic;
    UINT16 pad[29];
    INT32  lfanew;
} dos_hdr;

typedef struct {
    UINT16 machine;
    UINT16 nsec;
    UINT32 stamp;
    UINT32 sym;
    UINT32 nsym;
    UINT16 opth;
    UINT16 chars;
} file_hdr;

typedef struct {
    UINT32 va;
    UINT32 sz;
} data_dir;

typedef struct {
    UINT16 magic;
    UINT8  lmaj;
    UINT8  lmin;
    UINT32 code;
    UINT32 idata;
    UINT32 udata;
    UINT32 ep;
    UINT32 codebase;
    UINT64 imgbase;
    UINT32 salign;
    UINT32 falign;
    UINT16 osmaj;
    UINT16 osmin;
    UINT16 imaj;
    UINT16 imin;
    UINT16 smaj;
    UINT16 smin;
    UINT32 win32;
    UINT32 imgsz;
    UINT32 hdrsz;
    UINT32 csum;
    UINT16 sub;
    UINT16 dll;
    UINT64 stkres;
    UINT64 stkcom;
    UINT64 heapres;
    UINT64 heapcom;
    UINT32 lflags;
    UINT32 ndir;
    data_dir dir[16];
} opt_hdr;

typedef struct {
    UINT32 sig;
    file_hdr fh;
    opt_hdr oh;
} nt_hdr;

typedef struct {
    CHAR8  name[8];
    UINT32 vsz;
    UINT32 va;
    UINT32 raw;
    UINT32 file;
    UINT32 rel;
    UINT32 ln;
    UINT16 nrel;
    UINT16 nln;
    UINT32 chars;
} sec_hdr;

typedef struct {
    UINT32 va;
    UINT32 block;
} rel_blk;

#pragma pack(pop)

#define FIRST_SEC(nt) \
    ((sec_hdr *)((UINT8 *)(nt) + sizeof(UINT32) + sizeof(file_hdr) + (nt)->fh.opth))

#endif
