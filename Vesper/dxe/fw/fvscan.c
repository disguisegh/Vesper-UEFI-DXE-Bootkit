#include "ctx.h"

// https://discord.gg/sourcesheaven

#define FV_SIG 0x4856465Fu

typedef struct {
    UINT64 base;
    UINT64 len;
    UINT64 slack;
    UINT64 slack_n;
    BOOLEAN ok;
} fv_hole;

STATIC EFI_STATUS CollectHoles(fv_hole *out, UINT32 cap, UINT32 *n)
{
    UINTN bytes;
    UINTN dsz;
    UINTN key;
    UINT32 ver;
    UINT32 found;
    VOID *buf;
    EFI_STATUS st;
    UINT8 *w;
    UINTN i;
    UINTN off;
    UINT32 sig;
    UINT64 fvlen;

    if (!g_ctx.systab || !out || !n)
        return EFI_INVALID_PARAMETER;

    bytes = 0;
    dsz = 0;
    key = 0;
    ver = 0;
    g_ctx.systab->BootServices->GetMemoryMap(&bytes, NULL, &key, &dsz, &ver);
    bytes += dsz * 4;

    buf = NULL;
    g_ctx.systab->BootServices->AllocatePool(EfiBootServicesData, bytes, &buf);
    if (!buf)
        return EFI_OUT_OF_RESOURCES;

    st = g_ctx.systab->BootServices->GetMemoryMap(
        &bytes, (EFI_MEMORY_DESCRIPTOR *)buf, &key, &dsz, &ver);
    if (EFI_ERROR(st)) {
        g_ctx.systab->BootServices->FreePool(buf);
        return st;
    }

    found = 0;
    w = (UINT8 *)buf;
    for (i = 0; i < bytes / dsz && found < cap; i++) {
        EFI_MEMORY_DESCRIPTOR *d;
        UINT64 region;
        UINT64 probe;

        d = (EFI_MEMORY_DESCRIPTOR *)(w + i * dsz);
        if (d->Type != EfiConventionalMemory &&
            d->Type != EfiACPIMemoryNVS &&
            d->Type != EfiACPIReclaimMemory)
            continue;

        region = d->NumberOfPages * 4096;
        for (off = 0; off + 64 <= region; off += 16) {
            probe = d->PhysicalStart + off;
            sig = 0;
            CopyMem(&sig, (VOID *)(UINTN)probe, 4);
            if (sig != FV_SIG)
                continue;

            fvlen = 0;
            CopyMem(&fvlen, (VOID *)(UINTN)(probe + 0x18), 8);
            if (!fvlen || fvlen > region)
                continue;
            if (probe + fvlen <= probe + 0x1000)
                continue;
            if (found >= cap)
                continue;

            out[found].base = probe;
            out[found].len = fvlen;
            out[found].slack = probe + 0x1000;
            out[found].slack_n = fvlen - 0x1000;
            out[found].ok = (out[found].slack_n > 0) ? TRUE : FALSE;
            found++;
        }
    }

    g_ctx.systab->BootServices->FreePool(buf);
    *n = found;
    return EFI_SUCCESS;
}

EFI_STATUS StashBytes(CONST UINT8 *p, UINT64 n, UINT64 *out_pa)
{
    fv_hole holes[8];
    UINT32 cnt;
    UINT32 i;
    EFI_STATUS st;

    if (!p || !n || !out_pa)
        return EFI_INVALID_PARAMETER;

    cnt = 0;
    SetMem(holes, sizeof(holes), 0);
    st = CollectHoles(holes, 8, &cnt);
    if (EFI_ERROR(st) || !cnt)
        return EFI_ERROR(st) ? st : EFI_NOT_FOUND;

    for (i = 0; i < cnt; i++) {
        if (!holes[i].ok)
            continue;
        if (holes[i].slack_n < n)
            continue;
        CopyMem((VOID *)(UINTN)holes[i].slack, p, (UINTN)n);
        *out_pa = holes[i].slack;
        return EFI_SUCCESS;
    }
    return EFI_OUT_OF_RESOURCES;
}
