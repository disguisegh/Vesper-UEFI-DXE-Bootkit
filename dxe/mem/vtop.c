#include "ei.h"

typedef UINT64 (*ranges_fn)(VOID);

UINT64 ei_vtop(UINT64 va, UINT64 dtb)
{
	UINT64 i0, i1, i2, i3, off;
	UINT64 a, b, c, d;

	if (!va || !dtb)
		return 0;
	i0 = (va >> 39) & 0x1FFull;
	i1 = (va >> 30) & 0x1FFull;
	i2 = (va >> 21) & 0x1FFull;
	i3 = (va >> 12) & 0x1FFull;
	off = va & 0xFFFull;

	a = 0;
	ei_rd(&a, (dtb & EI_M4K) + i0 * 8, sizeof a);
	if (!(a & 1))
		return 0;
	b = 0;
	ei_rd(&b, (a & EI_M4K) + i1 * 8, sizeof b);
	if (!(b & 1))
		return 0;
	if (b & (1ull << 7))
		return (b & EI_M1G) | (va & EI_O1G);
	c = 0;
	ei_rd(&c, (b & EI_M4K) + i2 * 8, sizeof c);
	if (!(c & 1))
		return 0;
	if (c & (1ull << 7))
		return (c & EI_M2M) | (va & EI_O2M);
	d = 0;
	ei_rd(&d, (c & EI_M4K) + i3 * 8, sizeof d);
	if (!(d & 1))
		return 0;
	return (d & EI_M4K) | off;
}

UINT64 ei_dtb(UINT64 pid)
{
	ranges_fn rf;
	UINT64 img, tab, base, span, pages, phys, i, pa;
	UINT32 slot;
	UINT16 mz;

	img = ei_img(pid);
	if (!img)
		return 0;
	rf = (ranges_fn)(UINTN)(ei.nt + EI_NT_RANGES);
	tab = rf();
	if (!tab)
		return 0;
	for (slot = 0;; slot++) {
		base = *(UINT64 *)(UINTN)(tab + slot * 16);
		span = *(UINT64 *)(UINTN)(tab + slot * 16 + 8);
		if (!base && !span)
			break;
		if (!base || !span)
			continue;
		pages = span / 0x1000ull;
		phys = base;
		for (i = 0; i < pages; i++, phys += 0x1000ull) {
			pa = ei_vtop(img, phys);
			if (!pa)
				continue;
			mz = 0;
			ei_rd(&mz, pa, sizeof mz);
			if (mz == EI_MZ)
				return phys;
		}
	}
	return 0;
}
