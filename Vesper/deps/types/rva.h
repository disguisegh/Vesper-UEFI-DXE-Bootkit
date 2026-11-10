#ifndef RVA_H_
#define RVA_H_

// https://discord.gg/sourcesheaven

#define RVA_LDR_BLOCK   0x001F1898ull
#define RVA_CTX_SW      0x0002E01Cull

#define RVA_HOOK        0x00731600ull
#define RVA_COPY        0x0030C030ull
#define RVA_MAPIO       0x002E7FA0ull
#define RVA_LOOKUP      0x00625CA0ull
#define RVA_SECBASE     0x0024FB50ull
#define RVA_RANGES      0x007CCC30ull
#define RVA_POOL        0x003C8A20ull

#define MASK_4K         0x000FFFFFFFFFF000ull
#define MASK_2M         0x000FFFFFFFE00000ull
#define MASK_1G         0x000FFFFFC0000000ull
#define OFF_4K          0x0000000000000FFFull
#define OFF_2M          0x00000000001FFFFFull
#define OFF_1G          0x000000003FFFFFFFull

#define COPY_PHYS       0x1u
#define MAP_RW          0x4u

#define COOKIE_LO       0x7A1C4E90u
#define COOKIE_HI       0xD82B6F13u

#define BIND_MAGIC      0x51C8A2E7u
#define BIND_VER        3u

#define MZ_WALK_PAGES   0x2000u
#define MOD_WALK_MAX    512u

#endif
