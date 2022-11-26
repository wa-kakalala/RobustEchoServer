#pragma once
#include <winsock2.h>
#include <stdio.h>
#include "types.h"

#define ROBUSTECHO_PORT      6666
#define ROBUSTECHO_MAX_USR   64

extern SOCKET robustecho_soc[ROBUSTECHO_MAX_USR];
extern unsigned short soc_max_index;

int RobustEchoInit();
SOCKET RobustEchoListen(unsigned port);
SOCKET RobustEchoNewSoc(SOCKET lstn_soc, fd_set* soc_set);
int RobustEchoProc(SOCKET clt_soc, char* buf, unsigned int buflen);