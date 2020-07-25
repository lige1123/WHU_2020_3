/*************************************************************************
	> File Name: server_re_draw.c
	> Author: 
	> Mail: 
	> Created Time: Wed 22 Jul 2020 11:23:38 AM CST
 ************************************************************************/

#include "head.h"

extern struct Map court;
extern struct User *rteam, *bteam;
extern WINDOW *Football, *Football_t, *Message;
extern struct BallStatus ball_status;
extern struct Bpoint ball;
extern struct Score score;
//根据ball_status里记录的加速度，和上次re_drew时的速度，算出本次球应该移动的时间
//加速度保持不变，速度更新
//需要注意的是，当判断到速度减为0，ball_status里的速度和加速度都清空
//同样需要注意的时，球如果超过球场边界，则判定为出界，球停止到边界即可
void re_draw_ball() {
    if (ball_status.v.x || ball_status.v.y) {
        char tmp[512] = {0};
        double v = sqrt(pow(ball_status.v.x, 2) + pow(ball_status.v.y, 2));
        double a = sqrt(pow(ball_status.a.x, 2) + pow(ball_status.a.y, 2));
        sprintf(tmp, "time out = %lf, ax = %lf, ay = %lf", v / a, ball_status.a.x, ball_status.a.y);
        double t = 100000.0 / 1000000.0;
        show_message(Message, NULL, tmp, 1);
        if (t >= v / a) {
            show_message(Message, NULL, tmp, 1);
            show_message(Message, NULL, "time out!", 1);
            bzero(&ball_status.v, sizeof(ball_status.v));
            bzero(&ball_status.a, sizeof(ball_status.a));
        } else {
            ball.x += (ball_status.v.x * t + ball_status.a.x * pow(t, 2) / 2);
            ball_status.v.x += ball_status.a.x * t;
            ball.y += (ball_status.v.y * t + ball_status.a.y * pow(t, 2) / 2);
            ball_status.v.y += ball_status.a.y * t;
            if (ball.x >= court.width || ball.x <= 0 || ball.y >= court.height || ball.y <= 0) {
                if (ball.x >= court.width - 1) {
                    if (ball.x >= court.width - 1) {
                        if (ball.y >= court.height / 2 - 3 && ball.y <= court.height / 2 + 3) {
                            score.red++;
                            ball.x = court.width - 3;
                            ball. y = court.height / 2;
                            struct FootBallMsg msg;
                            msg.type = FT_WALL;
                            sprintf(msg.msg, "%s of %s team, get 1 score", ball_status.name, ball_status.by_team ? "blue" : "red");
                            send_all(&msg);
                        }
                        ball.x = court.width - 1;
                    }
                    if (ball.x <= 0) {
                        if (ball.y >= court.height / 2 - 3 && ball.y <= court.height / 2 + 3) {
                            score.blue++;
                            ball.x = 2;
                            ball.y = court.height / 2;
                            struct FootBallMsg msg;
                            msg.type = FT_WALL;
                            sprintf(msg.msg, "%s of %s team, get 1 score", ball_status.name, ball_status.by_team ? "blue" : "red");
                            send_all(&msg);
                        }
                        ball.x = 0;
                    }
                    if (ball.y >= court.height) ball.y = court.height - 1;
                    if (ball.y <= 0) ball.y = 0;
                    bzero(&ball_status.v, sizeof(ball_status.v));
                    bzero(&ball_status.a, sizeof(ball_status.a));
                }
            }
        }
    }
    w_gotoxy_putc(Football, (int)ball.x, (int)ball.y, 'o');
    if (ball_status.by_team) {
        wattron(Football, COLOR_PAIR(6));
    } else {
        wattron(Football, COLOR_PAIR(2));
    }
    w_gotoxy_putc(Football, (int)ball.x + 1, (int)ball.y, '`');
    wattron(Football, COLOR_PAIR(3));
}

//根据team，切换颜色
//在loc位置打印player，并显示姓名
void re_draw_player(int team, char *name,  struct Point *loc) {
    if (team) wattron(Football_t, COLOR_PAIR(6));
    else wattron(Football_t, COLOR_PAIR(2));

    w_gotoxy_putc(Football_t, loc->x, loc->y, 'K');
    w_gotoxy_puts(Football_t, loc->x + 1, loc->y - 1, name);
    wattron(Football_t, COLOR_PAIR(3));
}


void re_draw_team(struct User *team) {
    for (int i = 0; i < MAX; i++) {
        if (team[i].online) re_draw_player(team[i].team, team[i].name, &team[i].loc);
    }
}

void re_drew_gate() {
    for (int i = court.height / 2 + 1 -4; i <= court.height / 2 + 1 + 4; i++) {
        mvwprintw(Football_t, i, 1, "x");
        mvwprintw(Football_t, i, court.width + 2, "x");
    }
}

void re_draw(int signum) {
    //调用re_draw_team和re_draw_ball
    //wrefresh(Football);
    char tmp[512] = {0};
    //sprintf(tmp, "create cjson len");
    //show_message(Message, NULL, tmp, 1);
    werase(Football_t);
    box(Football, 0, 0);
    box(Football_t, 0, 0);
    re_draw_ball();
    re_draw_team(rteam);
    re_draw_team(bteam);
    re_drew_gate();
    wrefresh(Football_t);
    struct FootBallMsg msg;
    bzero(&msg, sizeof(msg));
    //strcpy(msg.msg, create_spirit());
    //msg.type = FT_MAP;
    //sprintf(tmp, "create cjson len %ld", sizeof(msg));
    //send_all(&msg);
}

