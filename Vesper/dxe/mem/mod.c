#include "ctx.h"

// https://discord.gg/sourcesheaven

typedef INT32 (__fastcall *ctx_sw_t)(INT32);

VOID FlipWinloadCtx(INT32 which)
{
    ctx_sw_t fn;
    UINT64 va;

    va = g_ctx.winload + RVA_CTX_SW;
    if (va == RVA_CTX_SW)
        return;

    fn = (ctx_sw_t)(UINTN)va;
    fn(which);
}

UINT64 WalkMz(UINT64 addr)
{
    UINTN i;

    addr &= ~0xFFFull;
    for (i = 0; i < MZ_WALK_PAGES; i++) {
        if (*(UINT16 *)(UINTN)addr == DOS_MAGIC)
            return addr;
        addr -= 0x1000ull;
    }
    return 0;
}

mod_ent FindLoadedMod(LIST_ENTRY *head, CONST CHAR16 *name)
{
    LIST_ENTRY *it;
    mod_ent *m;
    mod_ent empty;
    UINTN want;
    UINTN i;

    SetMem(&empty, sizeof(empty), 0);
    if (!head || !name)
        return empty;

    want = 0;
    while (name[want])
        want++;

    i = 0;
    for (it = head->ForwardLink; it != head && i < MOD_WALK_MAX; it = it->ForwardLink, i++) {
        m = MOD_FROM_LINK(it, mod_ent, link);
        if (!m || !m->name.p)
            continue;
        if (m->name.len / sizeof(CHAR16) != want)
            continue;
        if (CompareMem(m->name.p, name, want * sizeof(CHAR16)) == 0)
            return *m;
    }
    return empty;
}
