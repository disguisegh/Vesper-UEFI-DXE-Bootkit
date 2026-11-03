#include "ei.h"

STATIC VOID EFIAPI ei_on_va(EFI_EVENT ev, VOID *ctx)
{
	(VOID)ev;
	(VOID)ctx;
	if (!ei.st || !ei.st->RuntimeServices)
		return;
	ei.st->RuntimeServices->ConvertPointer(0, (VOID **)&ei.isr);
	ei.st->RuntimeServices->ConvertPointer(0, (VOID **)&ei.site);
	ei_bind(ei.site, ei.isr);
}

VOID ei_hook_va(VOID)
{
	if (!ei.st || !ei.st->BootServices)
		return;
	ei.st->BootServices->CreateEvent(
		EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE,
		TPL_NOTIFY,
		ei_on_va,
		NULL,
		&ei.va);
}
