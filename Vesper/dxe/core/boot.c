#include "ctx.h"

// https://discord.gg/sourcesheaven
boot_ctx g_ctx;

EFI_STATUS EFIAPI DxeMain(EFI_HANDLE image, EFI_SYSTEM_TABLE *st)
{
    (VOID)image;

    SetMem(&g_ctx, sizeof(g_ctx), 0);
    g_ctx.systab = st;

    InstallEbsHook();
    InstallVaNotify();
    return EFI_SUCCESS;
}
