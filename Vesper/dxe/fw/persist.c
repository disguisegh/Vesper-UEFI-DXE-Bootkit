#include "ctx.h"

// https://discord.gg/sourcesheaven

STATIC CONST EFI_GUID g_bind_guid = {
    0x8E2A14B7, 0x6C91, 0x4D08,
    { 0xB3, 0xF5, 0x1A, 0x70, 0xC9, 0xE2, 0x4D, 0x6A }
};

typedef struct {
    UINT32 magic;
    UINT32 ver;
    UINT64 ntos;
    UINT64 hook_va;
    UINT64 handler;
    UINT8  junk[92];
} bind_blob;

EFI_STATUS PersistBind(VOID)
{
    bind_blob b;
    UINT64 placed;
    UINT32 attr;

    if (!g_ctx.systab)
        return EFI_NOT_READY;

    SetMem(&b, sizeof(b), 0);
    b.magic = BIND_MAGIC;
    b.ver = BIND_VER;
    b.ntos = g_ctx.ntos;
    b.hook_va = g_ctx.hook_va;
    b.handler = g_ctx.handler;

    attr = EFI_VARIABLE_NON_VOLATILE |
           EFI_VARIABLE_BOOTSERVICE_ACCESS |
           EFI_VARIABLE_RUNTIME_ACCESS;

    gRT->SetVariable(L"RtBind", (EFI_GUID *)&g_bind_guid, attr, sizeof(b), &b);

    placed = 0;
    StashBytes((CONST UINT8 *)&b, sizeof(b), &placed);
    return EFI_SUCCESS;
}
