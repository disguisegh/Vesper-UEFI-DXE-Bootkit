# 🛠️ Vesper VGK/EAC DXE Driver Source

This is a UEFI DXE runtime driver.

It hooks EBS, finds ntos off winload, then on VA change it smash-hooks one ntos function with a 14-byte abs jmp (FF 25) to HandleRequest in the DXE. That's it. No cave, no stolen bytes, no calling original. The DXE stays mapped after Windows is up so the handler lives in firmware runtime, not ntos padding.

Read goes through MmCopyMemory PHYSICAL. Write is MmMapIoSpaceEx + memcpy. Packet ops are physical. If you want virtual, get the image / CR3 first and walk it yourself (TranslateVa is in here).

RVAs are in deps/types/rva.h and they are for one ntos/winload pair. If your build is different the hook lands in garbage and nothing works. Retune them.

Detection rates for this bootkit are currently exceptionally low across anti-cheat systems such as EAC, VGK, FACEIT.

However, please keep in mind that this is a publicly available source, meaning it can be detected or flagged at any time. Detection rates are never guaranteed.

Join our community https://discord.gg/sourcesheaven for more. 
