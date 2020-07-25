/*************************************************************************
	> File Name: common/server_exit.c
	> Author: 
	> Mail: 
	> Created Time: Thu 23 Jul 2020 09:21:54 AM CST
 ************************************************************************/

#include "head.h"

extern struct User *rteam, *bteam;

void server_exit(int signum) {
    struct FootBallMsg msg;
    bzero(&msg, sizeof(msg));
    DBG(RED"Server is going to stop!\n"NONE);
    msg.type = FT_FIN;
    for (int i = 0; i < MAX; i++) {
        if (rteam[i].online) send(rteam[i].fd, (void *)&msg, sizeof(msg), 0);
        if (bteam[i].online) send(bteam[i].fd, (void *)&msg, sizeof(msg), 0);
    }
    endwin();
    DBG(RED"Server stopped!\n"NONE);
    exit(0);
}

