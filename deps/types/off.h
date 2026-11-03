#ifndef EI_OFF_H_
#define EI_OFF_H_

#define EI_LDR_BLK      0x001F1898ull
#define EI_LDR_SW       0x0002E01Cull

#define EI_NT_SVC       0x00731600ull
#define EI_NT_COPY      0x0030C030ull
#define EI_NT_MAPIO     0x002E7FA0ull
#define EI_NT_LOOKUP    0x00625CA0ull
#define EI_NT_SECBASE   0x0024FB50ull
#define EI_NT_RANGES    0x007CCC30ull
#define EI_NT_POOL      0x003C8A20ull

#define EI_M4K          0x000FFFFFFFFFF000ull
#define EI_M2M          0x000FFFFFFFE00000ull
#define EI_M1G          0x000FFFFFC0000000ull
#define EI_O4K          0x0000000000000FFFull
#define EI_O2M          0x00000000001FFFFFull
#define EI_O1G          0x000000003FFFFFFFull

#define EI_COPY_PA      0x1u
#define EI_MAP_RW       0x4u

#define EI_CK_LO        0xB3E82D17u
#define EI_CK_HI        0xC4F19A60u

#define EI_NV_TAG       0x3E91C0AAu
#define EI_NV_VER       2u

#define EI_LEAVES       8u
#define EI_MZ_MAX       0x2000u
#define EI_LDR_MAX      512u

#endif
