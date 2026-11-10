#include "ctx.h"

// https://discord.gg/sourcesheaven

typedef INT64 (__fastcall *nt_copy_t)(UINT64, UINT64, UINT64, UINT32, UINT64 *);
typedef UINT64 (__fastcall *nt_map_t)(UINT64, UINT64, UINT32);

VOID ReadPhys(VOID *dst, UINT64 pa, UINT64 n)
{
    nt_copy_t copy;
    UINT64 moved;

    if (!dst)
        return;
    if (!pa || !n)
        return;

    copy = (nt_copy_t)(UINTN)(g_ctx.ntos + RVA_COPY);
    moved = 0;
    copy((UINT64)(UINTN)dst, pa, n, COPY_PHYS, &moved);
}

VOID WritePhys(UINT64 pa, UINT64 n, UINT64 src)
{
    nt_map_t map;
    UINT64 va;

    if (!pa || !n)
        return;
    if (!src)
        return;

    map = (nt_map_t)(UINTN)(g_ctx.ntos + RVA_MAPIO);
    va = map(pa, n, MAP_RW);
    if (!va)
        return;

    CopyMem((VOID *)(UINTN)va, (VOID *)(UINTN)src, (UINTN)n);
}
