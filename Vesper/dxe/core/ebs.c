#include "ctx.h"
#include <intrin.h>

// https://discord.gg/sourcesheaven

#define CTX_APP 0
#define CTX_FW  1

STATIC EFI_EXIT_BOOT_SERVICES g_old_ebs;

STATIC EFI_STATUS EFIAPI EbsCallback(EFI_HANDLE image, UINTN map_key)
{
    UINT64 rip;
    UINT64 blk_ptr;
    ldr_block_t *lb;
    mod_ent ntos;

    gBS->ExitBootServices = g_old_ebs;

    rip = (UINT64)(UINTN)_ReturnAddress();
    g_ctx.winload = WalkMz(rip);
    if (!g_ctx.winload)
        return g_old_ebs(image, map_key);

    blk_ptr = *(UINT64 *)(UINTN)(g_ctx.winload + RVA_LDR_BLOCK);
    lb = (ldr_block_t *)(UINTN)blk_ptr;
    if (!lb)
        return g_old_ebs(image, map_key);

    FlipWinloadCtx(CTX_APP);
    ntos = FindLoadedMod(&lb->load, L"ntoskrnl.exe");
    FlipWinloadCtx(CTX_FW);

    g_ctx.ntos = (UINT64)(UINTN)ntos.base;
    g_ctx.hook_va = g_ctx.ntos + RVA_HOOK;
    g_ctx.handler = (UINT64)(UINTN)HandleRequest;

    PersistBind(); // https://discord.gg/sourcesheaven
    return g_old_ebs(image, map_key);
}

VOID InstallEbsHook(VOID)
{
    g_old_ebs = gBS->ExitBootServices;
    gBS->ExitBootServices = EbsCallback;
}
