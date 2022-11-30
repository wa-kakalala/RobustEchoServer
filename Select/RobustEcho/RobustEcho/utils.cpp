#include "utils.h"

SOCKET robustecho_soc[ROBUSTECHO_MAX_USR];
// �����±�
int soc_max_index;

int RobustEchoInit() {
	WSADATA wsa_data;
	int ret = WSAStartup(WINSOCK_VERSION, &wsa_data); /* ��ʼ��WinSock��Դ */
	if (ret) {
		perror("[RobustEchoServer]:RobustEchoInit error...");
		exit(1);
	}
	for (int i = 0; i < ROBUSTECHO_MAX_USR; i++) robustecho_soc[i] = INVALID_SOCKET;
	soc_max_index = -1;
	return 0;
}

SOCKET RobustEchoListen(unsigned port) {
	struct sockaddr_in srv_addr;
	int ret = 0;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	SOCKET srv_soc_t = socket(AF_INET, SOCK_STREAM, 0); /* ����TCP socket */
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
	for (int index = 0; index <= soc_max_index; index++) {
		if (robustecho_soc[index] == INVALID_SOCKET) {
			robustecho_soc[index] = acpt_soc;
			break;
		}
	}
	if (index == ROBUSTECHO_MAX_USR) /* �Ѿ����� */
	{
		closesocket(acpt_soc);
		return INVALID_SOCKET;
	}
	if (index > soc_max_index) {
		soc_max_index = index;
		robustecho_soc[soc_max_index] = acpt_soc;

	}
	FD_SET(acpt_soc, soc_set);
	return acpt_soc;
}

int RobustEchoProc(SOCKET clt_soc, char* buf, unsigned int buflen) {
	int ret = recv(clt_soc, buf, buflen, 0); /* ����TCP�����socket */
	ret = send(clt_soc, buf, ret, 0); /* ����TCP�ķ����� */
	return ret;
}

int RobustEchoDel(int index) {
	robustecho_soc[index] = robustecho_soc[soc_max_index];
	robustecho_soc[soc_max_index--] = INVALID_SOCKET;
	return 0;
}




