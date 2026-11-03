#include "ei.h"

#define EI_FV 0x4856465Fu

typedef struct {
	UINT64 base;
	UINT64 len;
	UINT64 free_at;
	UINT64 free_n;
	BOOLEAN ok;
} ei_hole;

STATIC EFI_STATUS ei_holes(ei_hole *out, UINT32 cap, UINT32 *n)
{
	UINTN bytes, dsz, key;
	UINT32 ver, found;
	VOID *buf;
	EFI_STATUS st;
	UINT8 *w;
	UINTN i, off;
	UINT32 sig;
	UINT64 fvlen;

	if (!ei.st || !out || !n)
		return EFI_INVALID_PARAMETER;
	bytes = 0;
	dsz = 0;
	key = 0;
	ver = 0;
	ei.st->BootServices->GetMemoryMap(&bytes, NULL, &key, &dsz, &ver);
	bytes += dsz * 4;
	buf = NULL;
	ei.st->BootServices->AllocatePool(EfiBootServicesData, bytes, &buf);
	if (!buf)
		return EFI_OUT_OF_RESOURCES;
	st = ei.st->BootServices->GetMemoryMap(
		&bytes, (EFI_MEMORY_DESCRIPTOR *)buf, &key, &dsz, &ver);
	if (EFI_ERROR(st)) {
		ei.st->BootServices->FreePool(buf);
		return st;
	}
	found = 0;
	w = (UINT8 *)buf;
	for (i = 0; i < bytes / dsz && found < cap; i++) {
		EFI_MEMORY_DESCRIPTOR *d;
		UINT64 region, probe;

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
			if (sig != EI_FV)
				continue;
			fvlen = 0;
			CopyMem(&fvlen, (VOID *)(UINTN)(probe + 0x18), 8);
			if (!fvlen || fvlen > region)
				continue;
			if (probe + fvlen <= probe + 0x1000 || found >= cap)
				continue;
			out[found].base = probe;
			out[found].len = fvlen;
			out[found].free_at = probe + 0x1000;
			out[found].free_n = fvlen - 0x1000;
			out[found].ok = (out[found].free_n > 0) ? TRUE : FALSE;
			found++;
		}
	}
	ei.st->BootServices->FreePool(buf);
	*n = found;
	return EFI_SUCCESS;
}

EFI_STATUS ei_put(CONST UINT8 *p, UINT64 n, UINT64 *pa)
{
	ei_hole h[8];
	UINT32 cnt, i;
	EFI_STATUS st;

	if (!p || !n || !pa)
		return EFI_INVALID_PARAMETER;
	cnt = 0;
	SetMem(h, sizeof h, 0);
	st = ei_holes(h, 8, &cnt);
	if (EFI_ERROR(st) || !cnt)
		return EFI_ERROR(st) ? st : EFI_NOT_FOUND;
	for (i = 0; i < cnt; i++) {
		if (!h[i].ok || h[i].free_n < n)
			continue;
		CopyMem((VOID *)(UINTN)h[i].free_at, p, (UINTN)n);
		*pa = h[i].free_at;
		return EFI_SUCCESS;
	}
	return EFI_OUT_OF_RESOURCES;
}
