#include "utils.h"
#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */


SOCKET robustecho_soc[ROBUSTECHO_MAX_USR];

int RobustEchoInit() {
	WSADATA wsa_data;
	int ret = WSAStartup(WINSOCK_VERSION, &wsa_data); /* 初始化WinSock资源 */
	if (ret) {
		perror("[RobustEchoServer]:RobustEchoInit error...");
		exit(1);
	}
	for (int i = 0; i < ROBUSTECHO_MAX_USR; i++) robustecho_soc[i] = INVALID_SOCKET;
	return 0;
}

SOCKET RobustEchoListen(unsigned port) {
	struct sockaddr_in srv_addr;
	int ret = 0;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	SOCKET srv_soc_t = socket(AF_INET, SOCK_STREAM, 0); /* 基于TCP socket */
	if (srv_soc_t == INVALID_SOCKET) {
		perror("[RobustEchoServer]:Create Server Socket error...");
		exit(1);
	}
	ret = bind(srv_soc_t, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
	if (ret == SOCKET_ERROR)
	{
		printf("[RobustEchoServer] Bind error...");
		closesocket(srv_soc_t);
		return -1;
	}
	listen(srv_soc_t, SOMAXCONN);
	return srv_soc_t;
}

SOCKET RobustEchoNewSoc(SOCKET lstn_soc, fd_set* soc_set) {
	int index = 0;
	SOCKET acpt_soc = accept(lstn_soc, NULL, 0);
	if (acpt_soc == INVALID_SOCKET)
		return INVALID_SOCKET;
	for (int index = 0; index < ROBUSTECHO_MAX_USR; index++) {
		if (robustecho_soc[index] == INVALID_SOCKET) {
			robustecho_soc[index] = acpt_soc;
			break;
		}
	}
	if (index == ROBUSTECHO_MAX_USR) /* 已经满了 */
	{
		closesocket(acpt_soc);
		return INVALID_SOCKET;
	}
	FD_SET(acpt_soc, soc_set);
	return acpt_soc;
}

int RobustEchoProc(SOCKET clt_soc, char* buf, unsigned int buflen) {
	int ret = recv(clt_soc, buf, buflen, 0); /* 基于TCP服务的socket */
	ret = send(clt_soc, buf, buflen, 0); /* 基于TCP的服务器 */
	return ret;
}




