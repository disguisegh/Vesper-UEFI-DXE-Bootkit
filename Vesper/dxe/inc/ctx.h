#ifndef CTX_H_
#define CTX_H_

// https://discord.gg/sourcesheaven
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "rva.h"
#include "winload.h"
#include "packet.h"
#include "pe.h"

typedef struct {
    EFI_SYSTEM_TABLE *systab;
    UINT64 winload;
    UINT64 ntos;
    UINT64 hook_va;
    UINT64 handler;
    EFI_EVENT va_event;
} boot_ctx;

extern boot_ctx g_ctx;

VOID InstallEbsHook(VOID);
VOID InstallVaNotify(VOID);
VOID FlipWinloadCtx(INT32 which);
UINT64 WalkMz(UINT64 addr);
mod_ent FindLoadedMod(LIST_ENTRY *head, CONST CHAR16 *name);
VOID WriteAbsJmp(UINT64 where, UINT64 to);
INT32 HandleRequest(INT64 packet, VOID *unused1, INT32 unused2);
VOID ReadPhys(VOID *dst, UINT64 pa, UINT64 n);
VOID WritePhys(UINT64 pa, UINT64 n, UINT64 src);
UINT64 TranslateVa(UINT64 va, UINT64 cr3);
UINT64 GetProcImage(UINT64 pid);
UINT64 GetProcCr3(UINT64 pid);
VOID LoadRawPe(VOID *raw, UINT64 n);
EFI_STATUS PersistBind(VOID);
EFI_STATUS StashBytes(CONST UINT8 *p, UINT64 n, UINT64 *out_pa);

#endif
