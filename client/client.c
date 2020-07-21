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

int main(int argc, char **argv) {
    int opt;
    struct LogRequest request;
    struct LogResponse response;

    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));
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
    

    printf("port: %d\n", server_port);
    printf("team: %d\n", request.team);
    printf("ip: %s\n", server_ip);
    printf("name: %s\n", request.name);
    printf("msg: %s\n", request.msg);

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
        perror("select");
        exit(1);
    } else if (retval){
        int ret = recvfrom(sockfd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&server, &len);
        if (ret != sizeof(response) || response.type) {
            printf("The Game Server refused your login.\n\tThis May be helpful:%s\n", response.msg);
             exit(1);                
        }                
    } else {
        printf("The Game Server is out of service!.\n");
        exit(1);
    }

    printf("Server : %s\n", response.msg);
    connect(sockfd, (struct sockaddr *)&server, len);
    //是否需要更多的逻辑或者数据传输来测试，请自己决定，如果需要，在服务端，应该用多线程
    return 0;
}




