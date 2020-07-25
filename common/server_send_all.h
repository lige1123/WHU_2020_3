/*************************************************************************
	> File Name: server_send_all.h
	> Author: 
	> Mail: 
	> Created Time: Thu 23 Jul 2020 09:27:32 AM CST
 ************************************************************************/

#ifndef _SERVER_SEND_ALL_H
#define _SERVER_SEND_ALL_H

void send_team(struct User *team, struct FootBallMsg *msg);
void send_all(struct FootBallMsg *msg);

#endif
