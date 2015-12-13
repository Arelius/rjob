#ifndef _RJOB_PLATFORM_H_
#define _RJOB_PLATFORM_H_

#define RJOB_THREADLOCAL __declspec(thread)

namespace rjob
{
    namespace Platform
    {

        typedef void (*LaunchFn)(uintptr user);

        struct Thread
        {
        };

        struct Fiber
        {
        };

        Thread& LaunchThread(LaunchFn, uintptr user, uint32 stackSize, uint32 coreMask);
        Thread& CurrentThread();
        void SetThreadAffinity(Thread&, uint32);

        // Create a fiber that will begin with LaunchFn.
        Fiber& CreateFiber(LaunchFn, uintptr user, uint32 stackSize);
        // Convert the current thread into a Fiber, calling immediatlly into LaunchFn.
        Fiber& BeginFiber(LaunchFn, uintptr user);
        void DestroyFiber(Fiber&);
        void SwitchToFiber(Fiber&);
        void Yield();

        uint32 AtomicIncrement32(volatile uint32*);
        uint32 AtomicDecrement32(volatile uint32*);
    }
}

#endif //_RJOB_PLATFORM_H_
