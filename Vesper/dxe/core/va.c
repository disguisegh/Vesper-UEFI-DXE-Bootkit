#include "ctx.h"

// https://discord.gg/sourcesheaven

STATIC VOID EFIAPI VaChangeCallback(EFI_EVENT ev, VOID *ctx)
{
    (VOID)ev;
    (VOID)ctx;

    if (!g_ctx.systab)
        return;
    if (!g_ctx.systab->RuntimeServices)
        return;

    g_ctx.systab->RuntimeServices->ConvertPointer(0, (VOID **)&g_ctx.handler);
    g_ctx.systab->RuntimeServices->ConvertPointer(0, (VOID **)&g_ctx.hook_va);
    WriteAbsJmp(g_ctx.hook_va, g_ctx.handler);
}

VOID InstallVaNotify(VOID)
{
    if (!g_ctx.systab)
        return;
    if (!g_ctx.systab->BootServices)
        return;

    g_ctx.systab->BootServices->CreateEvent(
        EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE,
        TPL_NOTIFY,
        VaChangeCallback,
        NULL,
        &g_ctx.va_event);
}
