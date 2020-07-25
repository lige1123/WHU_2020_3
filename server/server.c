/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 一  7/20 20:00:06 2020
 ************************************************************************/

#include "../common/head.h"

int port = 0;
char *conf = "./footballd.conf";
char conf_ans[512];
int repollfd, bepollfd;
struct User *rteam, *bteam;
pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bmutex = PTHREAD_MUTEX_INITIALIZER;

struct Map court;//球场大小，你应该在server.c和client.c中定义该变量，并初始化
WINDOW *Football, *Football_t, *Message, *Help, *Score, *Write;//窗体
struct Bpoint ball;  //球的位置
struct BallStatus ball_status;
struct Score score;
int message_num = 0;
char data_stream[20] = {0};
double last_v = 0;

int main(int argc, char **argv) {
    int opt, listener, epollfd;
    char *temport;
    pthread_t red_t, blue_t, heart_t;

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

    //bzero
    bzero(&court, sizeof(court));
    bzero(&ball, sizeof(ball));
    bzero(&ball_status, sizeof(ball_status));

    //初始化
    court.height = atoi(get_conf_value(conf, "LINES"));
    court.width = atoi(get_conf_value(conf, "COLS"));
    court.start.x = 3;
    court.start.y = 3;
    ball.x = court.width / 2;
    ball.y = court.height / 2;
    //printf("w:%d\nh:%d\n", court.width, court.height);
    setlocale(LC_ALL, "");
#ifndef _D
    initfootball();
#endif
    if (!port) {
        temport = get_conf_value(conf, "PORT");
        if (temport == NULL) {
            perror("get_conf_value()");
            exit(1);
        }
        //printf(GREEN"%s" L_RED"test\n", temport);
        port = atoi(temport);
    }

    if ((listener = socket_create_udp(port)) < 0) {
        perror("socket_create_udp()");
        exit(1);
    }
    DBG(GREEN"INFO " NONE": Server start On port %d.\n", port);

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
    pthread_create(&heart_t, NULL, heart_beat, NULL);
    
    struct epoll_event ev, events[MAX * 2];
    ev.events = EPOLLIN;
    ev.data.fd = listener;
    
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        perror("epoll_ctl");
        exit(1);
    }

    signal(SIGINT, server_exit);

    signal(SIGALRM, re_draw);
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 100000;
    itimer.it_value.tv_sec = 5;
    itimer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itimer, NULL);
    
    //为了防止信号中断epoll_wait
    sigset_t origmask, sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);

    show_message(Message, NULL, "Waiting for Login.", 1);

    while (1) {
        DBG(L_YELLOW"Main Reactor " NONE": Waiting for client.\n");
        pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);
        int nfds = epoll_wait(epollfd, events, MAX * 2, -1);
        pthread_sigmask(SIG_SETMASK, &origmask, NULL);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(1);
        }
        
        for (int i = 0; i < nfds; ++i) {
            struct User user;
            char buff[512] = {0};
            bzero(&user, sizeof(user));
            if (events[i].data.fd == listener) {
                int new_fd = udp_accept(listener, &user);
                if (new_fd > 0) {
                    sprintf(buff, "%s login the Game.", user.name);
                    show_message(Message, &user, buff, 1);
                    show_data_stream('l');
                    add_to_sub_reactor(&user);
                }
            }
        }
    }
    endwin();
    return 0;
}

