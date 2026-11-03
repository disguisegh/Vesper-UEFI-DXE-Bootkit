#include "ei.h"

typedef INT32 (*lookup_fn)(VOID *, VOID *);
typedef UINT64 (*secbase_fn)(INT64);
typedef VOID *(*pool_fn)(UINT32, UINT64);
typedef INT32 (*entry_fn)(VOID *, VOID *);

UINT64 ei_img(UINT64 pid)
{
	lookup_fn lk;
	secbase_fn sb;
	UINT64 proc;

	lk = (lookup_fn)(UINTN)(ei.nt + EI_NT_LOOKUP);
	proc = 0;
	lk((VOID *)(UINTN)pid, &proc);
	if (!proc)
		return 0;
	sb = (secbase_fn)(UINTN)(ei.nt + EI_NT_SECBASE);
	return sb((INT64)proc);
}

VOID ei_pe(VOID *img, UINT64 n)
{
	ei_dos *dos;
	ei_nt *nt;
	ei_sec *sec;
	pool_fn pool;
	entry_fn ep;
	UINT64 dest, rel, end, slide, cnt, j;
	UINT16 s, *fix, kind, off;
	UINT64 *patch;
	ei_rel *blk;

	if (!img || !n)
		return;
	dos = (ei_dos *)img;
	if (dos->mz != EI_MZ)
		return;
	nt = (ei_nt *)((UINT8 *)img + dos->e_lfanew);
	if (nt->sig != EI_PE)
		return;
	pool = (pool_fn)(UINTN)(ei.nt + EI_NT_POOL);
	dest = (UINT64)(UINTN)pool(0, nt->oh.imgsz);
	if (!dest)
		return;
	SetMem((VOID *)(UINTN)dest, nt->oh.imgsz, 0);
	CopyMem((VOID *)(UINTN)dest, img, nt->oh.hdrsz);
	sec = EI_SEC0(nt);
	for (s = 0; s < nt->fh.nsec; s++) {
		CopyMem((VOID *)(UINTN)(dest + sec->va),
			(VOID *)((UINT8 *)img + sec->file),
			sec->raw);
		sec++;
	}
	if (nt->oh.dir[EI_REL].sz) {
		rel = nt->oh.dir[EI_REL].va;
		end = rel + nt->oh.dir[EI_REL].sz;
		slide = dest - nt->oh.imgbase;
		while (rel < end) {
			blk = (ei_rel *)(UINTN)(dest + rel);
			if (blk->block < sizeof(ei_rel))
				break;
			cnt = (blk->block - sizeof(ei_rel)) / 2;
			fix = (UINT16 *)(blk + 1);
			for (j = 0; j < cnt; j++) {
				kind = (UINT16)(fix[j] >> 12);
				off = (UINT16)(fix[j] & 0xFFF);
				if (kind == EI_DIR64) {
					patch = (UINT64 *)(UINTN)(dest + blk->va + off);
					*patch += slide;
				}
			}
			rel += blk->block;
		}
	}
	ep = (entry_fn)(UINTN)(dest + nt->oh.ep);
	ep((VOID *)(UINTN)dest, NULL);
}
