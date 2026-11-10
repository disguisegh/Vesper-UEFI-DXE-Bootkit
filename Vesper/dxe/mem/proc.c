#include "ctx.h"

// https://discord.gg/sourcesheaven

typedef INT32 (*lookup_t)(VOID *, VOID *);
typedef UINT64 (*secbase_t)(INT64);
typedef VOID *(*pool_t)(UINT32, UINT64);
typedef INT32 (*entry_t)(VOID *, VOID *);

UINT64 GetProcImage(UINT64 pid)
{
    lookup_t lookup;
    secbase_t secbase;
    UINT64 eproc;

    lookup = (lookup_t)(UINTN)(g_ctx.ntos + RVA_LOOKUP);
    eproc = 0;
    lookup((VOID *)(UINTN)pid, &eproc);
    if (!eproc)
        return 0;

    secbase = (secbase_t)(UINTN)(g_ctx.ntos + RVA_SECBASE);
    return secbase((INT64)eproc);
}

VOID LoadRawPe(VOID *raw, UINT64 n)
{
    dos_hdr *dos;
    nt_hdr *nt;
    sec_hdr *sec;
    pool_t pool;
    entry_t ep;
    UINT64 dest;
    UINT64 rel;
    UINT64 end;
    UINT64 slide;
    UINT64 cnt;
    UINT64 j;
    UINT16 s;
    UINT16 *fix;
    UINT16 kind;
    UINT16 off;
    UINT64 *patch;
    rel_blk *blk;

    if (!raw || !n)
        return;

    dos = (dos_hdr *)raw;
    if (dos->magic != DOS_MAGIC)
        return;

    nt = (nt_hdr *)((UINT8 *)raw + dos->lfanew);
    if (nt->sig != NT_MAGIC)
        return;

    pool = (pool_t)(UINTN)(g_ctx.ntos + RVA_POOL);
    dest = (UINT64)(UINTN)pool(0, nt->oh.imgsz);
    if (!dest)
        return;

    SetMem((VOID *)(UINTN)dest, nt->oh.imgsz, 0);
    CopyMem((VOID *)(UINTN)dest, raw, nt->oh.hdrsz);

    sec = FIRST_SEC(nt);
    for (s = 0; s < nt->fh.nsec; s++) {
        CopyMem((VOID *)(UINTN)(dest + sec->va),
                (VOID *)((UINT8 *)raw + sec->file),
                sec->raw);
        sec++;
    }

    if (nt->oh.dir[DIR_BASEREL].sz) {
        rel = nt->oh.dir[DIR_BASEREL].va;
        end = rel + nt->oh.dir[DIR_BASEREL].sz;
        slide = dest - nt->oh.imgbase;
        while (rel < end) {
            blk = (rel_blk *)(UINTN)(dest + rel);
            if (blk->block < sizeof(rel_blk))
                break;
            cnt = (blk->block - sizeof(rel_blk)) / 2;
            fix = (UINT16 *)(blk + 1);
            for (j = 0; j < cnt; j++) {
                kind = (UINT16)(fix[j] >> 12);
                off = (UINT16)(fix[j] & 0xFFF);
                if (kind == REL_DIR64) {
                    patch = (UINT64 *)(UINTN)(dest + blk->va + off);
                    *patch += slide;
                }
            }
            rel += blk->block;
        }
    }

    ep = (entry_t)(UINTN)(dest + nt->oh.ep);
    ep((VOID *)(UINTN)dest, NULL);
}
