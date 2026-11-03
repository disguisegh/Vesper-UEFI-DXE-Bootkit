#ifndef EI_H_
#define EI_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>

#include "off.h"
#include "ldr.h"
#include "req.h"
#include "coff.h"

typedef struct {
	UINT64 src;
	UINT64 dst;
	BOOLEAN on;
} ei_leaf;

typedef struct {
	EFI_SYSTEM_TABLE *st;
	UINT64 osldr;
	UINT64 nt;
	UINT64 site;
	UINT64 isr;
	EFI_EVENT va;
	VOID *smm;
	UINTN smm_n;
	BOOLEAN smm_ok;
	BOOLEAN nv_ok;
	ei_leaf leaf[EI_LEAVES];
	UINT32 nleaf;
} ei_state;

extern ei_state ei;

VOID ei_hook_exit(VOID);
VOID ei_hook_va(VOID);
VOID ei_world(INT32 w);
UINT64 ei_mz(UINT64 a);
ei_ldr ei_mod(LIST_ENTRY *h, CONST CHAR16 *n);
VOID ei_bind(UINT64 site, UINT64 isr);
INT32 ei_svc(INT64 slot, VOID *u, INT32 k);
VOID ei_rd(VOID *out, UINT64 pa, UINT64 n);
VOID ei_wr(UINT64 pa, UINT64 n, UINT64 src);
UINT64 ei_vtop(UINT64 va, UINT64 dtb);
UINT64 ei_img(UINT64 pid);
UINT64 ei_dtb(UINT64 pid);
VOID ei_pe(VOID *img, UINT64 n);
EFI_STATUS ei_nv_open(VOID);
EFI_STATUS ei_nv_put(VOID);
EFI_STATUS ei_smi(VOID);
EFI_STATUS ei_put(CONST UINT8 *p, UINT64 n, UINT64 *pa);
EFI_STATUS ei_leaf_mk(UINT64 src, UINT64 *dst);

#endif
