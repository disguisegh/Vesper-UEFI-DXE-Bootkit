#include "ei.h"

typedef INT32 (__fastcall *sw_fn)(INT32);

VOID ei_world(INT32 w)
{
	sw_fn f;
	UINT64 va;

	va = ei.osldr + EI_LDR_SW;
	if (va == EI_LDR_SW)
		return;
	f = (sw_fn)(UINTN)va;
	f(w);
}

UINT64 ei_mz(UINT64 a)
{
	UINTN i;

	a &= ~0xFFFull;
	for (i = 0; i < EI_MZ_MAX; i++) {
		if (*(UINT16 *)(UINTN)a == EI_MZ)
			return a;
		a -= 0x1000ull;
	}
	return 0;
}

ei_ldr ei_mod(LIST_ENTRY *h, CONST CHAR16 *n)
{
	LIST_ENTRY *it;
	ei_ldr *m;
	ei_ldr z;
	UINTN c, i;

	SetMem(&z, sizeof z, 0);
	if (!h || !n)
		return z;
	c = 0;
	while (n[c])
		c++;
	i = 0;
	for (it = h->ForwardLink; it != h && i < EI_LDR_MAX; it = it->ForwardLink, i++) {
		m = EI_CONT(it, ei_ldr, link);
		if (!m || !m->name.p)
			continue;
		if (m->name.len / sizeof(CHAR16) != c)
			continue;
		if (CompareMem(m->name.p, n, c * sizeof(CHAR16)) == 0)
			return *m;
	}
	return z;
}
