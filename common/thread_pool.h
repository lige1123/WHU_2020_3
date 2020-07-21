/*************************************************************************
	> File Name: thread_pool.h
	> Author: 
	> Mail: 
	> Created Time: Tue 21 Jul 2020 09:55:41 AM CST
 ************************************************************************/

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

struct task_queue {
    int size;
    int total;
    int epollfd;
    struct User **team;
    int head;
    int tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void task_queue_init(struct task_queue *taskQueue, int size, int epollfd);
void task_queue_push(struct task_queue *taskQueue,struct User *user);
struct User *task_queue_pop(struct task_queue *taskQueue);
void *thread_run(void *arg);

#endif
