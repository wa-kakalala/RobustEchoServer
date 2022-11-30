#include "utils.h"
#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */

#define ROBUST_BUF_SIZE 4096
char recv_buf[ROBUST_BUF_SIZE];


int main()
{
    fd_set read_all, read_set;
    int ready_cnt, i, ret;
    SOCKET client_soc;
    RobustEchoInit();
    SOCKET srv_soc_t = RobustEchoListen(ROBUSTECHO_PORT);
    if (srv_soc_t == INVALID_SOCKET) {
        printf("socket error \r\n");
    }
    
    FD_ZERO(&read_all);
    FD_SET(srv_soc_t, &read_all);

    while (1) {
        read_set = read_all;
        ready_cnt = select(0, &read_set, NULL, NULL, NULL);
        if (ready_cnt == SOCKET_ERROR)
        {
            perror("[RobustEchoServer] Select error ...");
            printf("error code : %d\r\n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(srv_soc_t, &read_set)) /* 检查TCP的侦听socket */
        {
            printf("new client\r\n");
            RobustEchoNewSoc(srv_soc_t, &read_all);
            if (--ready_cnt <= 0)
                continue;
        }

        for (i = 0; (i <= soc_max_index) && (ready_cnt > 0); i++)
        {
            if (robustecho_soc[i] == INVALID_SOCKET) continue;
            client_soc = robustecho_soc[i];
            if (!FD_ISSET(client_soc, &read_set))
                continue;
            // 处理数据
            ret = RobustEchoProc(client_soc, recv_buf, ROBUST_BUF_SIZE);
            --ready_cnt;
        }
    }
    for (i = 0; i <= soc_max_index; i++) {
        if (robustecho_soc[i] == INVALID_SOCKET) continue;
        closesocket(robustecho_soc[i]);
    }
    closesocket(srv_soc_t);
    WSACleanup();
}

