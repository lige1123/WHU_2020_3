/*************************************************************************
	> File Name: ball_status.c
	> Author: 
	> Mail: 
	> Created Time: Wed 22 Jul 2020 11:46:46 PM CST
 ************************************************************************/

#include "head.h"
#define PI 3.1415926 

extern WINDOW *Message;
extern struct Bpoint ball;
extern struct BallStatus ball_status;

//palyer和ball坐标对齐
//判断palyer和ball的坐标在上下左右2个单位距离内，则可踢球
//根据player和ball的相对位置，计算球的运动方向，加速度方向，假设球只能在palyer和ball的延长线上运动
//假设player踢球的接触时间为0.2秒，默认加速度为40，力度增加，加速度也增加
//可踢返回1，否则返回0

int can_kick(struct Point *loc, int strength) {
    char tmp[512] = {0};
    int px = loc->x - 2;
    int py = loc->y;
    double bx = ball.x;
    double by = ball.y;
    sprintf(tmp, "loc->x = %d, (int)ball.x = %d, loc->y = %d, (int)ball.y = %d", px, (int)bx, py, (int)by);
    show_message(Message, NULL, tmp, 1);
    if (abs(px - (int)bx) <= 2 && abs(-py + 1 +(int)by) <= 2) {
        if (px == (int)bx && py - 1 == (int)by) return 0;
        double v_tmp = (40.0 *strength) * 0.2;
        sprintf(tmp, "v_tmp = %lf", v_tmp);
        show_message(Message, NULL, tmp, 1);
        double a;
        if (px == (int)bx) 
            a = PI / 2;
        else
            a = atan(fabs(-py + 1 + by) / abs(px - bx));
        if (px > bx) {
            ball_status.v.x = -cos(a) * v_tmp;
            ball_status.a.x = cos(a) * 3;
        } else {
            ball_status.v.x = cos(a) * v_tmp;
            ball_status.a.x = -cos(a) * 3;
        }
        if (-py + 1 > -by) {
            ball_status.v.y = +sin(a) * v_tmp;
            ball_status.a.y = -sin(a) * 3;
        } else {
            ball_status.v.y = -sin(a) * v_tmp;
            ball_status.a.y = +sin(a) * 3;
        }
        return 1;
    } else 
        return 0;
}

int can_acess(struct Point *loc) {
    char tmp[512] = {0};
    sprintf(tmp, "Palyer loc: <x = %d, y = %d>, Ball loc: <x = %lf, y = %lf>", loc->x - 2, loc->y - 1, ball.x, ball.y);
    show_message(Message, NULL, tmp, 1);
    if (abs(loc->x -2 - (int)ball.x) <= 2 && abs(loc->y - 1 - (int)ball.y) <= 2)
        return 1;
    return 0;
}




    
