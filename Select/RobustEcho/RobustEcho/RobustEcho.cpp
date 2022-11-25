#include "utils.h"

#define ROBUST_BUF_SIZE 4096
char recv_buf[ROBUST_BUF_SIZE];

int main()
{
    fd_set read_all, read_set;
    int ready_cnt, i, ret;
    SOCKET client_soc;
    RobustEchoInit();
    SOCKET srv_soc_t = RobustEchoListen(ROBUSTECHO_PORT);
    
    FD_ZERO(&read_all);
    FD_SET(srv_soc_t, &read_all);

    while (1) {
        read_set = read_all;
        ready_cnt = select(0, &read_set, NULL, NULL, NULL);
        if (ready_cnt == SOCKET_ERROR)
        {
            perror("[RobustEchoServer] Select error...");
            break;
        }

        if (FD_ISSET(srv_soc_t, &read_set)) /* 检查TCP的侦听socket */
        {
            RobustEchoNewSoc(srv_soc_t, &read_all);
        }

        for (i = 0; (i <= ROBUSTECHO_MAX_USR) && (ready_cnt > 0); i++)
        {
            if (robustecho_soc[i] == INVALID_SOCKET) continue;
            client_soc = robustecho_soc[i];
            if (!FD_ISSET(client_soc, &read_set))
                continue;
            // 处理数据
            ret = RobustEchoProc(client_soc, recv_buf, ROBUST_BUF_SIZE);
            
            --ready_cnt;
        }
        for (i = 0; i <= ROBUSTECHO_MAX_USR; i++) {
            if (robustecho_soc[i] == INVALID_SOCKET) continue;
            closesocket(robustecho_soc[i]);
        }  
        closesocket(srv_soc_t);
    }
    WSACleanup();
}

