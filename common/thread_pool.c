/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 
	> Created Time: Tue 21 Jul 2020 09:55:58 AM CST
 ************************************************************************/

#include "head.h"

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
    recv(user->fd, (void *)&msg, sizeof(msg), 0);
    if (msg.type & FT_FIN) {

    } else if (msg.type & FT_MSG) {

    } else if (msg.type & FT_WALL) {
        
    } else if (msg.type & FT_ACK) {

    }
}

void *thread_run(void *arg) {
    pthread_detach(pthread_self());
    struct task_queue *taskQueue = (struct task_queue *)arg;
    while (1) {
        struct User *user = task_queue_pop(taskQueue);
        do_with(user);
    }
}

