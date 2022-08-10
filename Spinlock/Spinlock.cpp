#include "Spinlock.h"

void Spinlock::Lock() 
{
    while (spinlock.test_and_set(std::memory_order_acquire)) 
    {
        std::this_thread::sleep_for(500 * std::chrono::milliseconds(1));
    }
}

void Spinlock::Unlock() 
{
    spinlock.clear(std::memory_order_release);
}