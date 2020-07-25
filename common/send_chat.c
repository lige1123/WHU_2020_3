/*************************************************************************
	> File Name: send_chat.c
	> Author: 
	> Mail: 
	> Created Time: Thu 23 Jul 2020 12:10:02 AM CST
 ************************************************************************/

#include "head.h"

//此函数在按下回车键后调用
extern int sockfd;
extern WINDOW *Write;
extern struct FootBallMsg chat_msg; //外部全局变量
//先发FT_WALL类型的聊天信息，用于测试，后续再加队伍私聊功能
void send_chat() {
    //打开echo回显
    //打开行缓冲
    //在Write窗口中输入数据并读入
    //判断读入信息非空，发送
    //重绘Write  
    //关闭echo                        
    //关闭行缓冲
    echo();
    nocbreak();
    bzero(chat_msg.msg, sizeof(chat_msg.msg));
    chat_msg.type = FT_WALL;
    w_gotoxy_puts(Write, 1, 1, "Input Message : ");
    mvwscanw(Write, 2, 1, "%[^\n]s", chat_msg.msg);
    if (strlen(chat_msg.msg))
        send(sockfd, (void *)&chat_msg, sizeof(chat_msg), 0);
    wclear(Write);
    box(Write, 0, 0);
    wrefresh(Write);
    noecho();
    cbreak();
    
}
