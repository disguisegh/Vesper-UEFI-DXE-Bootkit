#ifndef EI_COFF_H_
#define EI_COFF_H_

#include <Uefi.h>

#define EI_MZ    0x5A4Du
#define EI_PE    0x00004550u
#define EI_REL   5u
#define EI_DIR64 10u

#pragma pack(push, 1)

typedef struct {
	UINT16 mz;
	UINT16 z[29];
	INT32  e_lfanew;
} ei_dos;

typedef struct {
	UINT16 machine;
	UINT16 nsec;
	UINT32 stamp;
	UINT32 sym;
	UINT32 nsym;
	UINT16 opth;
	UINT16 chars;
} ei_fh;

typedef struct {
	UINT32 va;
	UINT32 sz;
} ei_dd;

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
	ei_dd  dir[16];
} ei_oh;

typedef struct {
	UINT32 sig;
	ei_fh  fh;
	ei_oh  oh;
} ei_nt;

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
} ei_sec;

typedef struct {
	UINT32 va;
	UINT32 block;
} ei_rel;

#pragma pack(pop)

#define EI_SEC0(nt) \
	((ei_sec *)((UINT8 *)(nt) + sizeof(UINT32) + sizeof(ei_fh) + (nt)->fh.opth))

#endif
