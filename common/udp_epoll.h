/*************************************************************************
	> File Name: udp_epoll.h
	> Author: 
	> Mail: 
	> Created Time: 一  7/20 19:59:27 2020
 ************************************************************************/

#ifndef _UDP_EPOLL_H
#define _UDP_EPOLL_H

extern int port;
void add_event_ptr(int epollfd, int fd, int events, struct User *user);
void del_event(int epollfd, int fd);
int udp_connect(struct sockaddr_in *client);
int udp_accept(int fd, struct User *user);
void add_to_sub_reactor(struct User *user);


#endif
