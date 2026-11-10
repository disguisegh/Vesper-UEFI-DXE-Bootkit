This is a UEFI DXE runtime driver.

It hooks EBS, finds ntos off winload, then on VA change it smash-hooks one ntos function with a 14-byte abs jmp (FF 25) to HandleRequest in the DXE.

Read goes through MmCopyMemory. Write is MmMapIoSpaceEx + memcpy. Packet ops are physical.

RVAs are in deps/types/rva.h and they are for one ntos/winload pair. If your build is different the hook lands in garbage and nothing works. Retune them.

Developed for https://scuba.fan (not used anymore)
