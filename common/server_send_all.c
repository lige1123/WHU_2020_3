/*************************************************************************
	> File Name: server_send_all.c
	> Author: 
	> Mail: 
	> Created Time: Thu 23 Jul 2020 09:29:06 AM CST
 ************************************************************************/

#include "head.h"

void send_team(struct User *team, struct FootBallMsg *msg) {
    for (int i = 0; i < MAX; i++) {
        if (team[i].online) send(team[i].fd, (void *)msg, sizeof(struct FootBallMsg), 0);
    }
}



void send_all(struct FootBallMsg *msg) {
    send_team(rteam, msg);
    send_team(bteam, msg);
}


