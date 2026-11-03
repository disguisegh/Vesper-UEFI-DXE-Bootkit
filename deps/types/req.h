#ifndef EI_REQ_H_
#define EI_REQ_H_

#include <Uefi.h>

enum {
	EI_OP_IN   = 1,
	EI_OP_OUT  = 2,
	EI_OP_DTB  = 4,
	EI_OP_BASE = 5,
	EI_OP_PE   = 7
};

typedef struct {
	UINT64 ck;
	UINT32 op;
	UINT32 n;
	UINT64 at;
	UINT64 buf;
	UINT64 pid;
	UINT64 img;
	UINT64 dtb;
	UINT64 extra;
} ei_req;

#endif
