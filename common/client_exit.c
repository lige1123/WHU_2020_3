/*************************************************************************
	> File Name: common/client_exit.c
	> Author: 
	> Mail: 
	> Created Time: Thu 23 Jul 2020 09:21:54 AM CST
 ************************************************************************/

#include "head.h"

extern int sockfd;

void client_exit(int signum) {
    struct FootBallMsg msg;
    msg.type = FT_FIN;
    send(sockfd, (void *)&msg, sizeof(msg), 0);
    endwin();
    exit(0);
}

