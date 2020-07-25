/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 一  7/20 19:56:05 2020
 ************************************************************************/

#include "../common/head.h"

int server_port = 0;
char server_ip[20] = {0};
int team = -1;
char *conf = "./football.conf";
int sockfd = -1;
char conf_ans[512] = {0};

struct Map court;//球场大小，你应该在server.c和client.c中定义该变量，并初始化
WINDOW *Football, *Football_t, *Message, *Help, *Score, *Write;//窗体
struct Bpoint ball;  //球的位置
struct BallStatus ball_status;

struct FootBallMsg chat_msg;
struct FootBallMsg ctl_msg;
int show_name = 0;
char data_stream[20] = {0};
int message_num = 0;

void logout(int signum) {
    struct FootBallMsg msg;
    msg.type = FT_FIN;
    send(sockfd, (void *)&msg, sizeof(msg), 0);
    close(sockfd);
    DBG(RED"Bye!\n"NONE);
    endwin();
    exit(0);
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "");
    int opt;
    struct LogRequest request;
    struct LogResponse response;
    pthread_t recv_t, draw_t;
    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));
    bzero(&chat_msg, sizeof(struct FootBallMsg));
    bzero(&ctl_msg, sizeof(struct FootBallMsg));
    chat_msg.type = FT_WALL;
    ctl_msg.type = FT_CTL;

    bzero(&court, sizeof(court));
    bzero(&ball, sizeof(ball));
    bzero(&ball_status, sizeof(ball_status));

    while ((opt = getopt(argc, argv, "h:p:t:m:n:")) != -1) {
        switch (opt) {
            case 't':
                request.team = atoi(optarg);
                break;
            case 'h':
                strcpy(server_ip, optarg);
                break;
            case 'p':
                server_port = atoi(optarg);
                break;
            case 'm':
                strcpy(request.msg, optarg);
                break;
            case 'n':
                strcpy(request.name, optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s [-h] ip [-p] port [-t] team [-n] name [-m] msg", argv[0]);
                exit(1);
        }
    }
    if (!server_port) server_port = atoi(get_conf_value(conf, "SERVERPORT"));
    if (!request.team) request.team = atoi(get_conf_value(conf, "TEAM"));
    if (!strlen(server_ip)) strcpy(server_ip, get_conf_value(conf, "SERVERIP"));
    if (!strlen(request.name)) strcpy(request.name, get_conf_value(conf, "NAME"));
    if (!strlen(request.msg)) strcpy(request.msg, get_conf_value(conf, "LOGMSG"));
    

    DBG("<"GREEN"Conf Show"NONE"> : server_ip = %s, port = %d, team = %s, name = %s\n%s\n",\
        server_ip, server_port, request.team ? "BLUE" : "RED", request.name, request.msg);


    signal(SIGINT, logout);

    
    //初始化
    court.height = atoi(get_conf_value(conf, "LINES"));
    court.width = atoi(get_conf_value(conf, "COLS"));
    court.start.x = 3;
    court.start.y = 3;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip);

    socklen_t len = sizeof(server);
    if ((sockfd = socket_udp()) < 0) {
        perror("socket_udp()");
        exit(1);
    }

    sendto(sockfd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&server, len);
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sockfd, &set);

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    
    int retval = select(sockfd + 1, &set, NULL, NULL, &tv);
    if (retval < 0) {
        perror("select()");
        exit(1);
    } else if (retval){
        int ret = recvfrom(sockfd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&server, &len);
        if (ret != sizeof(response) || response.type) {
            DBG(RED"Error"NONE": The Game Server refused your login.\n\tThis May be helpful:%s\n", response.msg);
            exit(1);                
        }                
    } else {
        DBG(RED"Error"NONE": The Game Server is out of service!.\n");
        endwin();
        exit(1);
    }

    DBG(GREEN"Server"NONE" : %s\n", response.msg);
    connect(sockfd, (struct sockaddr *)&server, len);

#ifndef _D
    initfootball();
#endif
    pthread_create(&recv_t, NULL, client_recv, NULL);
    signal(14, send_ctl);
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 100000;
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 100000;
    setitimer(ITIMER_REAL, &itimer, NULL);
    

    while (1) {
        int c = getchar();
        switch (c) {
            case 'a':
                ctl_msg.ctl.dirx -= 1;
                break;
            case 'd':
                ctl_msg.ctl.dirx += 1;
                break;
            case 'w':
                ctl_msg.ctl.diry -= 1;
                break;
            case 's':
                ctl_msg.ctl.diry += 1;
                break;
            case 13:
                send_chat();
                break;
            case ' ':{
                show_strength();
                break;
            }
            case 'k':{
                show_data_stream('k');
                struct FootBallMsg msg;
                bzero(&msg, sizeof(msg));
                msg.type = FT_CTL;
                msg.ctl.action = ACTION_KICK;
                msg.ctl.strength = 1;
                send(sockfd, (void *)&msg, sizeof(msg), 0);
                break;
            }
            case 'j':{
                show_data_stream('s');
                struct FootBallMsg msg;
                bzero(&msg, sizeof(msg));
                msg.type = FT_CTL;
                msg.ctl.action = ACTION_STOP;
                send(sockfd, (void *)&msg, sizeof(msg), 0);
                break;
            }
            case 'l':{
                show_data_stream('c');
                struct FootBallMsg msg;
                bzero(&msg, sizeof(msg));
                msg.type = FT_CTL;
                msg.ctl.action = ACTION_CARRY;
                send(sockfd, (void *)&msg, sizeof(msg), 0);
                break;
            }
            case 'n':{
                show_name = show_name ? 0 : 1;
                break;
            }
            default:
                break;
        }
        box(Write, 0, 0);
        wrefresh(Write);
    }
    
    return 0;
}




