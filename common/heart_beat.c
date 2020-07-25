/*************************************************************************
	> File Name: heart_beat.c
	> Author: 
	> Mail: 
	> Created Time: Wed 22 Jul 2020 11:13:57 AM CST
 ************************************************************************/

#include "head.h"

extern struct User *rteam, *bteam;
extern int repollfd, bepollfd;
extern WINDOW *Message;
//to do
//遍历team数组，判断在线，则发送FT_HEART心跳包，flag--
//判断palyer的flag是否减为0，减为0则判断为下线
//数组中标记为offline-->online = 0
//在从反应堆中注销IO  //del_event

void heart_beat_team(struct User *team) {
    struct FootBallMsg msg;
    bzero(&msg, sizeof(msg));
    msg.type = FT_HEART;
    for (int i = 0; i < MAX; i++) {
        if (team[i].online) {
            if(!team[i].flag) {
                team[i].online = 0;
            }    
            DBG(RED"send ♥ ...\n"NONE);
            send(team[i].fd, (void *)&msg, sizeof(msg), 0);
            team[i].flag--;
            if (team[i].flag <= 0) {
                char tmp[512] = {0};
                sprintf(tmp, "%s is removed from list.", team[i].name);
                DBG(RED"%s\n"NONE, tmp);
                show_message(Message, NULL, tmp, 1);
                team[i].online = 0;
                int epollfd_tmp = (team[i].team ? bepollfd : repollfd);
                del_event(epollfd_tmp, team[i].fd);
            }
        }
    }
}


void *heart_beat(void *arg) {
    /*
    signal(SIGALRM, (void *)heart_beat_team(team));
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 100000;
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_sec = 200000;
    setitimer(ITIMER_REAL, &itimer, NULL);
    */
    while(1) {
        sleep(10);
        heart_beat_team(rteam);
        heart_beat_team(bteam);
    }    
}


