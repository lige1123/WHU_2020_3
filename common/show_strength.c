/*************************************************************************
	> File Name: show_strength.c
	> Author: 
	> Mail: 
	> Created Time: Thu 23 Jul 2020 12:08:04 AM CST
 ************************************************************************/

#include "head.h"

//客户端中，按下空格键，调用此函数
extern WINDOW *Write, *Message;
extern int sockfd;

void show_strength() {
    //在Write窗口中，显示踢球力度条，光标在进度条上快速移动
    //设置0文件为非阻塞IO
    //while 等待空格或者'k'键的按下，如果按下退出，取得当前的strength
    //通过sockfd向服务端发送控制信息，踢球
    int maxx, maxy, strength;
    getmaxyx(Write, maxy, maxx);
    struct FootBallMsg msg;
    bzero(&msg, sizeof(msg));
    for (int i = 2; i < maxx - 2; i++) {
        if (i < maxx / 5 || i > (maxx -  maxx / 5)){
            wattron(Write, COLOR_PAIR(8));
        } else if (i < 2 *maxx / 5 || i > 3 *maxx / 5) {
            wattron(Write, COLOR_PAIR(9));
        } else {
            wattron(Write, COLOR_PAIR(10));
        }
        mvwaddch(Write, 2, i, ' ');
    }
    wattron(Write, COLOR_PAIR(3));
    int tmp[5] = {1, 2, 3, 2, 1};
    int offset = 1;
    int tmp_x = 2;
    make_non_block(0);
    while (1) {
        int c = getchar();
        if (c != -1) {
            if (c == ' ' || c =='k') {
                mvwaddch(Write, 1, tmp_x, ' ');
                mvwaddch(Write, 3, tmp_x, ' ');
                make_block(0);
                break;
            }
        }
        usleep(5000);
        mvwaddch(Write, 1, tmp_x, ' ');
        mvwaddch(Write, 3, tmp_x, ' ');
        tmp_x += offset;
        mvwaddch(Write, 1, tmp_x, '|');
        mvwaddch(Write, 3, tmp_x, '|');
        mvwaddch(Write, 4, maxx, ' ');
        if (tmp_x >= maxx - 2) offset = -1;
        if (tmp_x <= 2) offset = 1;
        wrefresh(Write);
    }		
    char info[512] = {0};
    sprintf(info, "strength = %d", tmp[tmp_x / (maxx / 5)]);
    show_message(Message, NULL, info, 1);
    msg.type = FT_CTL;
    msg.ctl.action = ACTION_KICK;
    msg.ctl.strength = tmp[tmp_x / (maxx / 5)];
    send(sockfd, (void *)&msg, sizeof(msg), 0);
}


