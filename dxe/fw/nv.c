#include "ei.h"
#include <Protocol/SmmCommunication.h>

STATIC CONST EFI_GUID bind_guid = {
	0x5C9F3D82, 0x0B27, 0x4E4E,
	{ 0xC8, 0xD1, 0x2F, 0x6B, 0x9E, 0x04, 0xA1, 0x35 }
};

typedef struct {
	UINT32 tag;
	UINT32 ver;
	UINT64 nt;
	UINT64 site;
	UINT64 isr;
	UINT8  pad[92];
} ei_bind;

STATIC EFI_SMM_COMMUNICATION_PROTOCOL *smm;

STATIC VOID ei_fill(ei_bind *o)
{
	SetMem(o, sizeof *o, 0);
	o->tag = EI_NV_TAG;
	o->ver = EI_NV_VER;
	o->nt = ei.nt;
	o->site = ei.site;
	o->isr = ei.isr;
}

EFI_STATUS ei_nv_put(VOID)
{
	ei_bind b;
	UINT64 placed;
	UINT32 attr;

	if (!ei.st)
		return EFI_NOT_READY;
	ei_fill(&b);
	attr = EFI_VARIABLE_NON_VOLATILE |
	       EFI_VARIABLE_BOOTSERVICE_ACCESS |
	       EFI_VARIABLE_RUNTIME_ACCESS;
	gRT->SetVariable(L"Vesper", (EFI_GUID *)&bind_guid, attr, sizeof b, &b);
	placed = 0;
	ei_put((CONST UINT8 *)&b, sizeof b, &placed);
	return EFI_SUCCESS;
}

STATIC EFI_STATUS ei_smm_find(VOID)
{
	EFI_STATUS st;

	if (!ei.st)
		return EFI_NOT_READY;
	if (ei.smm_ok)
		return EFI_SUCCESS;
	smm = NULL;
	st = ei.st->BootServices->LocateProtocol(
		&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **)&smm);
	if (EFI_ERROR(st))
		return st;
	ei.smm_n = 256;
	st = ei.st->BootServices->AllocatePool(EfiBootServicesData, ei.smm_n, &ei.smm);
	if (EFI_ERROR(st)) {
		smm = NULL;
		ei.smm = NULL;
		return st;
	}
	ei.smm_ok = TRUE;
	return EFI_SUCCESS;
}

EFI_STATUS ei_smi(VOID)
{
	EFI_SMM_COMMUNICATION_HEADER *h;
	UINTN n;

	if (!ei.smm_ok || !ei.smm || !smm)
		return EFI_NOT_READY;
	h = (EFI_SMM_COMMUNICATION_HEADER *)ei.smm;
	CopyMem(&h->HeaderGuid, &bind_guid, sizeof(EFI_GUID));
	h->MessageLength = 1;
	h->Data[0] = 0xA7;
	n = ei.smm_n;
	return smm->Communicate(smm, ei.smm, &n);
}

EFI_STATUS ei_nv_open(VOID)
{
	if (ei.nv_ok)
		return EFI_SUCCESS;
	ei_nv_put();
	ei_smm_find();
	ei.nv_ok = TRUE;
	return EFI_SUCCESS;
}
