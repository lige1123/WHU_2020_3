/*************************************************************************
	> File Name: send_ctl.c
	> Author: 
	> Mail: 
	> Created Time: Thu 23 Jul 2020 12:17:08 AM CST
 ************************************************************************/

#include "head.h"

//此函数应该是一个周期性被调用的函数，也就是说，是由时钟信号触发的
extern int sockfd;
extern struct FootBallMsg ctl_msg; //也是个全局变量

void send_ctl() {
    if (ctl_msg.ctl.dirx || ctl_msg.ctl.diry){
        ctl_msg.ctl.action = ACTION_DFT;
        send(sockfd, (void *)&ctl_msg, sizeof(ctl_msg), 0);
        ctl_msg.ctl.dirx = ctl_msg.ctl.diry = 0;
        show_data_stream('n');
    } 
}

