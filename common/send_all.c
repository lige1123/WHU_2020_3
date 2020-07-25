/*************************************************************************
	> File Name: ../common/send_all.c
	> Author: 
	> Mail: 
	> Created Time: Fri 24 Jul 2020 03:53:49 PM CST
 ************************************************************************/

#include "head.h"

extern struct User *rteam, *bteam;
void send_team(struct User *team, struct FootBallMsg *msg){
    for (int i = 0; i < MAX; i++) {
        if (team[i].online) send(team[i].fd, (void *)msg, sizeof(struct FootBallMsg), 0);    
    }
}
void send_all(struct FootBallMsg *msg) {
    send_team(rteam, msg);
    send_team(bteam, msg);
}
