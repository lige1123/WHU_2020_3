/*************************************************************************
	> File Name: ../common/client_recv.c
	> Author: 
	> Mail: 
	> Created Time: Fri 24 Jul 2020 05:53:56 PM CST
 ************************************************************************/

#include "head.h"

extern int sockfd;
extern WINDOW *Message;

void *client_recv(void *arg) {
    while (1) {
        struct FootBallMsg msg;
        struct User user;
        bzero(&msg, sizeof(msg));
        recv(sockfd, (void *)&msg, sizeof(msg), 0);
        strcpy(user.name, msg.name);
        user.team = msg.team;
        
        
        if (msg.type & FT_HEART) {
            DBG(RED"HeartBeat from Server ♥\n"NONE);
            msg.type = FT_ACK;
            send(sockfd, (void *)&msg, sizeof(msg), 0);
            DBG(GREEN"Send ACK to Server ♥\n"NONE);
        } else if (msg.type & FT_MSG) {
            DBG(GREEN"Server Msg "NONE": %s\n", msg.msg);
//            show_message(Message, &user, msg.msg, 0);
        } else if (msg.type & FT_WALL) {
            DBG(GREEN"Msg to WALL "NONE" %s\n", msg.msg);
            show_message(Message, NULL, msg.msg, 1);
        } else if (msg.type & FT_FIN) {
            DBG(GREEN"Server is going to stop!\n"NONE);
//            close(sockfd);
            endwin();
            exit(0);
        } else if (msg.type & FT_MAP) {
//            show_message(Message, NULL, "FootBall Game refresh", 1);
            //parse_spirit(msg.msg, msg.size);
        } else {
            DBG(GREEN"Msg Unsupport\n"NONE);
        }
       
        //msg.type = FT_ACK;
        //send(sockfd, (void *)&msg, sizeof(msg), 0);
    }
}


