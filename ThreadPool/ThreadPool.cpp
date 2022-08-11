#include "ThreadPool.h"

sig_atomic_t flagShutdown = 0;

void ThreadPool::idleThread(ThreadPool *threadPool) 
{
    while (1) 
    {
        std::unique_lock uniqueLock(threadPool->mutexThreadPool);
        threadPool->condThreadPool.wait_for(uniqueLock, 2000 * millisec, [&]{return threadPool->canTakeTask || flagShutdown;});

        if (!threadPool->IsEmptyQueueTasks()) 
        {
            threadPool->canTakeTask = false;
            threadPool->queueTasks.front().func(threadPool->queueTasks.front().arg);
            threadPool->queueTasks.pop();
            std::cout << "requset: " << threadPool->requestCounter << std::endl;
        }

        uniqueLock.unlock();

        if (flagShutdown) 
        {
            break;
        }
    }
}

void ThreadPool::Initialize() 
{
    std::cout << "ThreadPool: Initialize" << std::endl;
    for (int i = 0; i < numThreads; ++i) 
    {
        threads.emplace_back(idleThread, this);
    }
}

void ThreadPool::AddTask(TaskWrapper &&taskWrapper) 
{
    spinlockThreadPool.Lock();
    queueTasks.push(taskWrapper);
    spinlockThreadPool.Unlock();
}

void ThreadPool::PopTask() 
{
    if (!mutexThreadPool.try_lock()) 
    {
        return;
    }
    canTakeTask = true;
    ++requestCounter;
    condThreadPool.notify_one();
    mutexThreadPool.unlock();
}

bool ThreadPool::IsEmptyQueueTasks() 
{
    return queueTasks.empty();
}

void ThreadPool::Shutdown() 
{
    for (int i = 0; i < numThreads; ++i) 
    {
        threads[i].join();
    }
}