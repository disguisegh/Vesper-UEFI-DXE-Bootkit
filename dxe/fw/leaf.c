#include "ei.h"

EFI_STATUS ei_leaf_mk(UINT64 src, UINT64 *dst)
{
	VOID *p;
	EFI_STATUS st;
	ei_leaf *s;

	if (!ei.st || !dst)
		return EFI_INVALID_PARAMETER;
	if (ei.nleaf >= EI_LEAVES)
		return EFI_OUT_OF_RESOURCES;
	p = NULL;
	st = ei.st->BootServices->AllocatePool(EfiBootServicesData, 0x1000, &p);
	if (EFI_ERROR(st) || !p)
		return st;
	CopyMem(p, (VOID *)(UINTN)src, 0x1000);
	s = &ei.leaf[ei.nleaf];
	s->src = src;
	s->dst = (UINT64)(UINTN)p;
	s->on = TRUE;
	ei.nleaf++;
	*dst = s->dst;
	return EFI_SUCCESS;
}
