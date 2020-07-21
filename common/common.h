/*************************************************************************
	> File Name: common.h
	> Author: 
	> Mail: 
	> Created Time: 一  7/20 19:58:45 2020
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

extern char conf_ans[512];
char *get_conf_value(const char *path, const char *key);
int socket_create_udp(int port);
int socket_udp();

#endif
