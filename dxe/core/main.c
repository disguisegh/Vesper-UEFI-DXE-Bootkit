#include "ei.h"

ei_state ei;

EFI_STATUS EFIAPI ei_entry(EFI_HANDLE img, EFI_SYSTEM_TABLE *st)
{
	(VOID)img;
	SetMem(&ei, sizeof ei, 0);
	ei.st = st;
	ei_nv_open();
	ei_hook_exit();
	ei_hook_va();
	return EFI_SUCCESS;
}
