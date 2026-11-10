#ifndef PACKET_H_
#define PACKET_H_

#include <Uefi.h>

// https://discord.gg/sourcesheaven

enum {
    CMD_READ  = 1,
    CMD_WRITE = 2,
    CMD_CR3   = 4,
    CMD_IMAGE = 5,
    CMD_PE    = 7
};

typedef struct {
    UINT64 key;
    UINT32 cmd;
    UINT32 len;
    UINT64 phys;
    UINT64 data;
    UINT64 pid;
    UINT64 base;
    UINT64 cr3;
    UINT64 pad;
} req_t;

#endif
