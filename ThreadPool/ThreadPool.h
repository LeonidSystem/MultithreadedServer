#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <iostream>
#include <queue>
#include <vector>
#include <condition_variable>
#include <signal.h>

#include "../Spinlock/Spinlock.h"

extern sig_atomic_t flagShutdown;

struct TaskWrapper 
{
    void*(*func)(void*);
    void *arg;

    TaskWrapper(void*(*_func)(void*), void *_arg) : func(_func), arg(_arg) {}
};

class ThreadPool 
{
    private:
        std::vector<std::thread> threads;
        std::queue<TaskWrapper> queueTasks;
        Spinlock spinlockThreadPool;
        std::mutex mutexThreadPool;
        std::condition_variable condThreadPool;

        int numThreads = 2;//std::thread::hardware_concurrency();
        int requestCounter = 0;
        bool canTakeTask = false;

        static void idleThread(ThreadPool *threadPool);

    public:
        ThreadPool() = default;
        void Initialize();
        void AddTask(TaskWrapper &&taskWrapper);
        void PopTask();
        bool IsEmptyQueueTasks();
        void Shutdown();
};

#endif