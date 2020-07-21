/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 一  7/20 20:00:06 2020
 ************************************************************************/

#include "../common/head.h"

int port;
char *conf = "./footballd.conf";
char conf_ans[512];
int repollfd, bepollfd;
struct User *rteam, *bteam;
pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bmutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv) {
    int opt, listener, epollfd;
    char *temport;
    pthread_t red_t, blue_t;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s -p port\n", argv[0]);
                exit(1);
        }
    }
    if (!port) {
        temport = get_conf_value(conf, "PORT");
        if (temport == NULL) {
            perror("get_conf_value()");
            exit(1);
        }
        //printf("%s\n", temport);
        port = atoi(temport);
    }

    if ((listener = socket_create_udp(port)) < 0) {
        perror("socket_create_udp()");
        exit(1);
    }

    epollfd = epoll_create(1);
    repollfd = epoll_create(1);
    bepollfd = epoll_create(1);
    if (epollfd < 0 || repollfd < 0 || bepollfd < 0) {
        perror("epoll_create()");
        exit(1);
    }

    rteam = (struct User *)calloc(MAX, sizeof(struct User));
    bteam = (struct User *)calloc(MAX, sizeof(struct User));

    struct task_queue redQueue;
    struct task_queue blueQueue;

    task_queue_init(&redQueue, MAX, repollfd);
    task_queue_init(&blueQueue, MAX, bepollfd);

    pthread_create(&red_t, NULL, sub_reactor, (void *)&redQueue);
    pthread_create(&blue_t, NULL, sub_reactor, (void *)&blueQueue);
    
    struct epoll_event ev, events[MAX];
    ev.events = EPOLLIN;
    ev.data.fd = listener;
    
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        perror("epoll_ctl");
        exit(1);
    }

    while (1) {
        int nfds = epoll_wait(epollfd, events, MAX, -1);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(1);
        }
        
        for (int i = 0; i < nfds; i++) {
            struct User user;
            bzero(&user, sizeof(user));
            if (events[i].data.fd == listener) {
                int new_fd = udp_accept(listener, &user);
                if (new_fd > 0) {
                    printf("New Connection!\n");
                }
            }
        }
    }
    return 0;
}

