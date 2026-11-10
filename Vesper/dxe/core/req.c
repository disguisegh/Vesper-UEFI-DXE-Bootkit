#include "ctx.h"

// https://discord.gg/sourcesheaven

STATIC UINT64 MixCookie(VOID)
{
    UINT64 hi = COOKIE_HI;
    UINT64 lo = COOKIE_LO;
    return (hi << 32) | lo;
}

VOID WriteAbsJmp(UINT64 where, UINT64 to)
{
    UINT8 *p;

    if (!where || !to)
        return;

    p = (UINT8 *)(UINTN)where;
    p[0] = 0xFF;
    p[1] = 0x25;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    p[5] = 0;
    CopyMem(p + 6, &to, 8);
}

INT32 HandleRequest(INT64 packet, VOID *unused1, INT32 unused2)
{
    req_t *r;

    (VOID)unused1;
    (VOID)unused2;

    if (!packet)
        return 1;

    r = (req_t *)(UINTN)packet;
    if (r->key != MixCookie())
        return 1;

    // https://discord.gg/sourcesheaven
    if (r->cmd == CMD_WRITE) {
        WritePhys(r->phys, r->len, r->data);
        return 1;
    }
    if (r->cmd == CMD_READ) {
        ReadPhys((VOID *)(UINTN)r->data, r->phys, r->len);
        return 1;
    }
    if (r->cmd == CMD_CR3) {
        r->cr3 = GetProcCr3(r->pid);
        return 1;
    }
    if (r->cmd == CMD_IMAGE) {
        r->base = GetProcImage(r->pid);
        return 1;
    }
    if (r->cmd == CMD_PE) {
        LoadRawPe((VOID *)(UINTN)r->data, r->len);
        return 1;
    }
    return 1;
}
