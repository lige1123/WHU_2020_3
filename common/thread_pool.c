/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 
	> Created Time: Tue 21 Jul 2020 09:55:58 AM CST
 ************************************************************************/

#include "head.h"
extern WINDOW *Message, *Football_t;
extern int bepollfd, repollfd;
extern struct Map court;
extern struct BallStatus ball_status;
extern struct Bpoint ball;

void task_queue_init(struct task_queue *taskQueue, int size, int epollfd) {
    taskQueue->size = size;
    taskQueue->total = 0;
    taskQueue->epollfd = epollfd;
    taskQueue->team = calloc(size, sizeof(void *));
    taskQueue->head = taskQueue->tail = 0;
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
}

void task_queue_push(struct task_queue *taskQueue,struct User *user) {
    pthread_mutex_lock(&taskQueue->mutex);
    if (taskQueue->total == taskQueue->size) {
        pthread_mutex_unlock(&taskQueue->mutex);
        return ;
    }
    taskQueue->team[taskQueue->tail] = user;
    taskQueue->total++;
    DBG(YELLOW"Thread Poll " NONE": Task Push %s\n", user->name);
    if(++taskQueue->tail == taskQueue->size) {
        taskQueue->tail = 0;
    }
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

struct User *task_queue_pop(struct task_queue *taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);
    while (taskQueue->tail == taskQueue->head) {
        DBG(YELLOW"Thread Poll " NONE": Task Queue Empty, Waiting For Task.\n");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
    }
    struct User *user = taskQueue->team[taskQueue->head];
    DBG(YELLOW"Thread Poll " NONE": Task Pop %s\n", user->name);
    if (++taskQueue->head == taskQueue->size) {
        taskQueue->head = 0;
    }
    pthread_mutex_unlock(&taskQueue->mutex);
    return user;
}

void do_with(struct User *user) {
    struct FootBallMsg msg;
    bzero(&msg, sizeof(msg));
    char tmp[512] = {0};
    int size = recv(user->fd, (void *)&msg, sizeof(msg), 0);
    user->flag = 10;
    if (msg.type & FT_ACK) {
        show_data_stream('n');
        if (user->team)
            DBG(L_BLUE" %s "NONE"♥\n", user->name);
        else
            DBG(L_RED" %s "NONE"♥\n", user->name);
    } else if (msg.type & (FT_WALL | FT_MSG)) {
        if (user->team)
            DBG(L_BLUE" %s : %s\n "NONE, user->name, msg.msg);
        else
            DBG(L_RED" %s : %s\n"NONE, user->name, msg.msg);
        strcpy(msg.name, user->name);
        msg.team = user->team;
        show_message(Message, user, msg.msg, 0);
        send_all(&msg);
    } else if (msg.type & FT_FIN) {
        show_data_stream('n');
        DBG(RED"%s logout.\n", user->name);
        sprintf(tmp, "%s Logout.", user->name);
        show_message(Message, NULL, tmp, 1);
        user->online = 0;
        //w_gotoxy_putc(Football_t,user->loc.x, user->loc.y, ' ');
        int epollfd_tmp = (user->team ? bepollfd : repollfd);
        del_event(epollfd_tmp, user->fd);
    } else if (msg.type & FT_CTL) {
        sprintf(tmp, "Ctrl Message kick = %d", msg.ctl.strength);
        show_message(Message, user, tmp, 0);
        if (msg.ctl.action & ACTION_DFT) {
        //if (msg.ctl.dirx || msg.ctl.diry) {
            show_data_stream('n');
            user->loc.x += msg.ctl.dirx;
            user->loc.y += msg.ctl.diry;
            if (user->loc.x <= 1) user->loc.x = 1;
            if (user->loc.x >= court.width + 2) user->loc.x = court.width + 2;
            if (user->loc.y <= 0) user->loc.y = 0;
            if (user->loc.y >= court.height + 1) user->loc.y = court.height + 1;
        } else if(msg.ctl.action & ACTION_KICK) {
            show_data_stream('k');
            sprintf(tmp, "bx = %lf, by = %lf, px = %d, py = %d", ball.x, ball.y, user->loc.x, user->loc.y);
            show_message(Message, user, tmp, 0);
            if (can_kick(&user->loc, msg.ctl.strength)) {
                ball_status.by_team = user->team;
                strcpy(ball_status.name, user->name);
                sprintf(tmp, "vx = %f, vy = %f, ax = %f, ay = %f", ball_status.v.x, ball_status.v.y, ball_status.a.x, ball_status.a.y);
                show_message(Message, user, tmp, 0);
            }
        } else if (msg.ctl.action & ACTION_STOP) {
            show_data_stream('s');
            if (can_acess(&user->loc)) {
                bzero(&ball_status.v, sizeof(ball_status.v));
                bzero(&ball_status.a, sizeof(ball_status.a));
                sprintf(tmp, "Stop the ball");
                show_message(Message, user, tmp, 0);
            }
        } else if (msg.ctl.action & ACTION_CARRY) {
            show_data_stream('c');
            sprintf(tmp, "Try to carry the ball");
            show_message(Message, user, tmp, 0);
        }
    } 
        
}

void *thread_run(void *arg) {
    pthread_detach(pthread_self());
    struct task_queue *taskQueue = (struct task_queue *)arg;
    while (1) {
        struct User *user = task_queue_pop(taskQueue);
        DBG(L_RED"Before do_with()"NONE": %s",user->name);
        do_with(user);
    }
}

