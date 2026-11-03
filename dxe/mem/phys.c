#include "ei.h"

typedef INT64 (__fastcall *copy_fn)(UINT64, UINT64, UINT64, UINT32, UINT64 *);
typedef UINT64 (__fastcall *mapio_fn)(UINT64, UINT64, UINT32);

VOID ei_rd(VOID *out, UINT64 pa, UINT64 n)
{
	copy_fn f;
	UINT64 moved;

	if (!out || !pa || !n)
		return;
	f = (copy_fn)(UINTN)(ei.nt + EI_NT_COPY);
	moved = 0;
	f((UINT64)(UINTN)out, pa, n, EI_COPY_PA, &moved);
}

VOID ei_wr(UINT64 pa, UINT64 n, UINT64 src)
{
	mapio_fn f;
	UINT64 m;

	if (!pa || !n || !src)
		return;
	f = (mapio_fn)(UINTN)(ei.nt + EI_NT_MAPIO);
	m = f(pa, n, EI_MAP_RW);
	if (!m)
		return;
	CopyMem((VOID *)(UINTN)m, (VOID *)(UINTN)src, (UINTN)n);
}
