#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <atomic>
#include <thread>
#include <chrono>

extern std::chrono::milliseconds millisec;
class Spinlock 
{
    private:
        std::atomic_flag spinlock = ATOMIC_FLAG_INIT;

    public:
        Spinlock() = default;
        void Lock();
        void Unlock();
};

#endif