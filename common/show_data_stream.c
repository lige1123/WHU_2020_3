/*************************************************************************
	> File Name: show_data_stream.c
	> Author: 
	> Mail: 
	> Created Time: Thu 23 Jul 2020 12:03:50 AM CST
 ************************************************************************/

#include "head.h"

extern char data_stream[20];//外部变量，分别再client.c,server.c中定义
extern WINDOW *Help;
extern struct Map court;//该变量用来在函数中定位要输出的位置
//type： 'l','c','k','s','n','e'
//分别表示：login， carry， kick， stop， normal， exit
void show_data_stream(int type) {
    //data_stream数组后移一位，将type加到第一位
    //根据type不同，使用wattron设置Help窗口的颜色
    //在合适位置打印一个空格
    
    for(int i = 17; i >= 1; i--){
        data_stream[i + 1] = data_stream[i];
    }
    data_stream[1] = type;
    for (int i = 1; i <= 18; i++) {
        if (data_stream[i] == 'l') 
            wattron(Help, COLOR_PAIR(10));
        else if (data_stream[i] == 'c') 
            wattron(Help, COLOR_PAIR(9));
        else if (data_stream[i] == 'k') 
            wattron(Help, COLOR_PAIR(12));
        else if (data_stream[i] == 's') 
            wattron(Help, COLOR_PAIR(11));
        else if (data_stream[i] == 'n') 
            wattron(Help, COLOR_PAIR(8));
        else if (data_stream[i] == 'e') 
            wattron(Help, COLOR_PAIR(10));
        else
            wattron(Help, COLOR_PAIR(3));
        w_gotoxy_putc(Help, i, court.height, ' ');
    }
}
