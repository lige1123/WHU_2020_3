/*************************************************************************
	> File Name: heart_beat.h
	> Author: 
	> Mail: 
	> Created Time: Wed 22 Jul 2020 11:13:35 AM CST
 ************************************************************************/

#ifndef _HEART_BEAT_H
#define _HEART_BEAT_H

extern struct User *rteam, *bteam;
extern int repollfd, bepollfd;
void heart_beat_team(struct User *team);
void *heart_beat(void *arg);

#endif
