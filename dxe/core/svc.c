#include "ei.h"

STATIC UINT64 ei_ck(VOID)
{
	return ((UINT64)EI_CK_HI << 32) | (UINT64)EI_CK_LO;
}

VOID ei_bind(UINT64 site, UINT64 isr)
{
	UINT8 *p;

	if (!site || !isr)
		return;
	p = (UINT8 *)(UINTN)site;
	p[0] = 0xFF;
	p[1] = 0x25;
	p[2] = 0x00;
	p[3] = 0x00;
	p[4] = 0x00;
	p[5] = 0x00;
	CopyMem(p + 6, &isr, sizeof isr);
}

INT32 ei_svc(INT64 slot, VOID *u, INT32 k)
{
	ei_req *r;

	(VOID)u;
	(VOID)k;
	if (!slot)
		return 1;
	r = (ei_req *)(UINTN)slot;
	if (r->ck != ei_ck())
		return 1;
	switch (r->op) {
	case EI_OP_OUT:
		ei_wr(r->at, r->n, r->buf);
		break;
	case EI_OP_IN:
		ei_rd((VOID *)(UINTN)r->buf, r->at, r->n);
		break;
	case EI_OP_DTB:
		r->dtb = ei_dtb(r->pid);
		break;
	case EI_OP_BASE:
		r->img = ei_img(r->pid);
		break;
	case EI_OP_PE:
		ei_pe((VOID *)(UINTN)r->buf, r->n);
		break;
	default:
		break;
	}
	return 1;
}
