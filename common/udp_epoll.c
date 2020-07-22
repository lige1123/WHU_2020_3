/*************************************************************************
	> File Name: udp_epoll.c
	> Author: 
	> Mail: 
	> Created Time: 一  7/20 19:59:18 2020
 ************************************************************************/

#include "head.h"

extern int port;
extern struct User *rteam, *bteam;
extern pthread_mutex_t bmutex, rmutex;
extern int repollfd, bepollfd;

void add_event_ptr(int epollfd, int fd, int events, struct User *user) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr =(void *)user;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD,fd, &ev) < 0) {
        perror("epoll_ctl()");
        exit(1);
    }
    return ;
}

void del_event(int epollfd, int fd) {
    if(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
        perror("epoll_ctl()");
        exit(1);
    }
    return ;
}

int udp_connect(struct sockaddr_in *client) {
    int sockfd;
    if ((sockfd = socket_create_udp(port)) < 0) {
        perror("socket_create_udp()");
        exit(1);
    }
    if ((connect(sockfd, (struct sockaddr *)client, sizeof(struct sockaddr))) < 0) {
        perror("connect()");
        exit(1);
    }
    return sockfd;
}

int udp_accept(int fd, struct User *user) {
    int new_fd, ret;
    struct sockaddr_in client;
    struct LogRequest request;
    struct LogResponse response;
    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));
    socklen_t len = sizeof(client);
    ret = recvfrom(fd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&client, &len);
    if (ret != sizeof(request)) {
        response.type = 1;
        strcpy(response.msg, "Login failed with Data errors!");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }

    if (check_online(&request)) {
        response.type = 1;
        strcpy(response.msg, "You have already login!");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }
    

    if ((new_fd = udp_connect(&client)) < 0) {
        return -1;
    }
    strcpy(user->name, request.name);
    user->team = request.team;
    user->fd = new_fd;
    char buff[512];
    sprintf(buff, "Hello, %s. Login sucess, enjoy youself!\n", user->name);
    response.type = 0;
    strcpy(response.msg, buff);
    send(new_fd, (void *)&response, sizeof(response), 0);
    return new_fd;
}

int find_sub(struct User *team) {
    for (int i = 0; i< MAX; i++) {
        if (!team[i].online) return i; 
    }
    return -1;
}

void add_to_sub_reactor(struct User *user) {
    struct User *team = (user->team ? bteam : rteam);
    if (user->team) pthread_mutex_lock(&bmutex);
    else pthread_mutex_lock(&rmutex);

    int sub = find_sub(team);
    if (sub < 0) return ;
    team[sub] = *user;
    team[sub].online = 1;
    team[sub].flag = 10;
    if (user->team) pthread_mutex_unlock(&bmutex);
    else pthread_mutex_unlock(&rmutex);

    DBG("Add to sub reactor %s\n", team[sub].name);
    
    if (user->team) {
        add_event_ptr(bepollfd, team[sub].fd, EPOLLIN | EPOLLET, &team[sub]);
    } else {
        add_event_ptr(repollfd, team[sub].fd, EPOLLIN | EPOLLET, &team[sub]);
    }
}

int check_online(struct LogRequest *request) {
    for (int i = 0; i < MAX; i++) {
        if (rteam[i].online == 1 && !strcmp(rteam[i].name, request->name)) return 1;
        if (bteam[i].online == 1 && !strcmp(bteam[i].name, request->name)) return 1;
    }
    return 0;
}

