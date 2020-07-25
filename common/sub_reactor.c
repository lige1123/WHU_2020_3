/*************************************************************************
	> File Name: sub_reactor.c
	> Author: 
	> Mail: 
	> Created Time: Tue 21 Jul 2020 11:26:28 AM CST
 ************************************************************************/

#include "head.h" 

void *sub_reactor(void *arg) {
    struct task_queue *taskQueue = (struct task_queue *)arg;
    pthread_t *tid = (pthread_t *)calloc(NWORKER, sizeof(pthread_t));
    for (int i = 0; i < NWORKER; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)taskQueue);
    }
    struct epoll_event ev, events[MAX];
    //为了防止信号中断epoll_wait
    sigset_t mask, orig_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    
    while (1) {
        DBG(RED"Sub Reactor " NONE": Epoll Waiting...\n");
        pthread_sigmask(SIG_SETMASK, &mask, &orig_mask); 
        int nfds = epoll_wait(taskQueue->epollfd, events, MAX, -1); 
        pthread_sigmask(SIG_SETMASK, &orig_mask, NULL);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(1);
        } 
        for (int i = 0; i < nfds; i++) {
            struct User *user = (struct User *)events[i].data.ptr;
            DBG(RED"Sub Reactor " NONE": %s Ready\n", user->name);
            if (events[i].events & EPOLLIN) {
                task_queue_push(taskQueue, (struct User *)events[i].data.ptr);
            }
        }
    }
    return NULL;
}

