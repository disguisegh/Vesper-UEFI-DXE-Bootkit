#include "ctx.h"

// https://discord.gg/sourcesheaven

typedef UINT64 (*ranges_t)(VOID);

UINT64 TranslateVa(UINT64 va, UINT64 cr3)
{
    UINT64 pml4, pdpt, pd, pt;
    UINT64 i0, i1, i2, i3, off;
    UINT64 e0, e1, e2, e3;

    if (!va || !cr3)
        return 0;

    i0 = (va >> 39) & 0x1FFull;
    i1 = (va >> 30) & 0x1FFull;
    i2 = (va >> 21) & 0x1FFull;
    i3 = (va >> 12) & 0x1FFull;
    off = va & 0xFFFull;

    e0 = 0;
    pml4 = (cr3 & MASK_4K) + i0 * 8;
    ReadPhys(&e0, pml4, sizeof(e0));
    if ((e0 & 1) == 0)
        return 0;

    e1 = 0;
    pdpt = (e0 & MASK_4K) + i1 * 8;
    ReadPhys(&e1, pdpt, sizeof(e1));
    if ((e1 & 1) == 0)
        return 0;
    if (e1 & (1ull << 7))
        return (e1 & MASK_1G) | (va & OFF_1G);

    e2 = 0;
    pd = (e1 & MASK_4K) + i2 * 8;
    ReadPhys(&e2, pd, sizeof(e2));
    if ((e2 & 1) == 0)
        return 0;
    if (e2 & (1ull << 7))
        return (e2 & MASK_2M) | (va & OFF_2M);

    e3 = 0;
    pt = (e2 & MASK_4K) + i3 * 8;
    ReadPhys(&e3, pt, sizeof(e3));
    if ((e3 & 1) == 0)
        return 0;

    return (e3 & MASK_4K) | off;
}

UINT64 GetProcCr3(UINT64 pid)
{
    ranges_t get_ranges;
    UINT64 image;
    UINT64 table;
    UINT64 base;
    UINT64 span;
    UINT64 pages;
    UINT64 phys;
    UINT64 i;
    UINT64 pa;
    UINT32 slot;
    UINT16 mz;

    image = GetProcImage(pid);
    if (!image)
        return 0;

    get_ranges = (ranges_t)(UINTN)(g_ctx.ntos + RVA_RANGES);
    table = get_ranges();
    if (!table)
        return 0;

    for (slot = 0;; slot++) {
        base = *(UINT64 *)(UINTN)(table + slot * 16);
        span = *(UINT64 *)(UINTN)(table + slot * 16 + 8);
        if (!base && !span)
            break;
        if (!base || !span)
            continue;

        pages = span / 0x1000ull;
        phys = base;
        for (i = 0; i < pages; i++, phys += 0x1000ull) {
            pa = TranslateVa(image, phys);
            if (!pa)
                continue;
            mz = 0;
            ReadPhys(&mz, pa, sizeof(mz));
            if (mz == DOS_MAGIC)
                return phys;
        }
    }
    return 0;
}
