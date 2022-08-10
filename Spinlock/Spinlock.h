#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <atomic>
#include <thread>
#include <chrono>

class Spinlock 
{
    private:
        std::atomic_flag spinlock;

    public:
        Spinlock() : spinlock(ATOMIC_FLAG_INIT) {}
        void Lock();
        void Unlock();
};

#endif