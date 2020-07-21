/*************************************************************************
	> File Name: common.c
	> Author: 
	> Mail: 
	> Created Time: 一  7/20 19:58:22 2020
 ************************************************************************/

#include "head.h"

char *get_conf_value(const char *path, const char *key) {
    if ((access(path, F_OK)) < 0) {
        printf("File does not exist!\n");
        return NULL;
    }
    if (key == NULL) {
        return NULL;
    }

    FILE *fp = NULL;
    fp = fopen(path, "r");
    char *line = NULL;
    size_t size = 0;
    bzero(conf_ans, sizeof(conf_ans));
    while (getline(&line, &size, fp) != -1) {
        if (strstr(line, key) != NULL) {
            if (*(line + strlen(key)) == '=') {
                strncpy(conf_ans, (line + strlen(key) + 1), strlen(line + strlen(key) + 1) - 1);
            }
        } 
    }
    
    free(line);
    fclose(fp);
    return conf_ans;
}

void make_non_block(int fd) {
    unsigned long u1 = 1;
    ioctl(fd, FIONBIO, &u1);
}

void make_block(int fd) {
    unsigned long u1 = 0;
    ioctl(fd, FIONBIO, &u1);
}

int socket_create_udp(int port) {
    int listener;
    if ((listener = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    
    //设置地址重用
    unsigned long opt = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //设置为非阻塞套接字
    make_non_block(listener);

    if (bind(listener, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }

    return listener;
} 

int socket_udp() {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }
    return sockfd;
}


/*
char conf_ans[512];
int main() {
    char *name = get_conf_value("../client/football.conf", "NAME");
    printf("%s\n%ld\n", name, strlen(name));
}
*/


