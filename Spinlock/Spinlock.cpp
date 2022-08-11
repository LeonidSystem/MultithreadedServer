#include "Spinlock.h"

std::chrono::milliseconds millisec = std::chrono::milliseconds(1);

void Spinlock::Lock() 
{
    while (spinlock.test_and_set(std::memory_order_acquire)) 
    {
        std::this_thread::sleep_for(250 * millisec);    //delay, so that some of the requests are not lost
    }
}

void Spinlock::Unlock() 
{
    spinlock.clear(std::memory_order_release);
}