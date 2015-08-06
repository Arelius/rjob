#include "rjob.h"

namespace
{
    const uint32 workerThreadCount = 8;
    const uint32 maxFibers = 512;
    const uint32 maxJobsPerQueue = 2048;
    const uint32 jobStackSize = 32 * 1024;
    const uint32 schedulerStackSize = 1 * 1024;

    struct Job
    {
        Fiber* fiber;
        JobEntry entryFn;
        void* user;
    };

    struct RunQueue
    {
        Job jobs[maxJobsPerQueue];
    };

    struct WaitingJob
    {
        Job job;
        JobPriority priority;
        JobCounter waitCounter;
    };

    struct WaitQueue
    {
        WaitingJob jobs[maxJobsPerQueue * JobPriority::PriorityCount];
    };

    struct JobFiber
    {
        Fiber fiber;
        Job runningJob;
    }

    struct JobSystem
    {
        RunQueue runQueues[JobPriority::PriorityCount];
        WaitQueue waitQueue;

        Thread workerThreads[workerThreadCount];
        Fiber schedulerFibers[workerThreadCount];
        JobFiber fiberPool[maxFibers];
        

        volatile bool running;
        volatile uint32 runningWorkers;
    };

    JobSystem* jobSystem;

    // Always access via GetCurrentThreadIdx because compilers can optimize TLS access in a thread.
    thread_local uint32 gCurrentThreadIdx;

    void FiberThread(uint64 fiberIdx)
    {
        while(true)
        {
            JobFiber* fiber = jobSystem->fiberPool[fiberIdx];
            Job* job = fiber->runningJob;
            job->entryFn(job->user);
            Platform::SwitchToFiber(jobSystem->schedulerFibers[rjob::GetCurrentThreadIndex()]);
        }
    }

    // We do scheduling in it's own fiber so that in theory, jobs will be able to pick their stack size.

    void ScheduleWork(uint64 threadIdx)
    {
        while(jobSystem->running)
        {
            // Deque Best Job
            Job job;
            JobFiber* fiber;
            if(job->fiber == nullptr)
            {
                // Find free fiber.
            }
            fiber->runningJob = job;// Redundant copy?
            Platform::SwitchToFiber(fiber)
        }
    }

    void WorkerStartup(uint64 threadIdx)
    {
        gCurrentThreadIdx = gCurrentThreadIdx;
        Platform::AtomicIncrement32(jobSystem->runningWorkers);
        jobSystem->schedulerFibers[threadIdx] = Platform::BeginFiber(ScheduleWork, threadIdx);
        Platform::AtomicDecrement32(jobSystem->runningWorkers);
    }

    uint32 rjob::GetRequiredMemory()
    {
        return sizeof(JobSystem);
    }

    void rjob::Initialize(void* memory)
    {
        jobSystem = (JobSystem*)memory;
        for(uint32 i = 0; i < maxFibers; i++)
            jobSystem->fiber[i].fiber = Platform::CreateFiber(FiberThread, i, jobStackSize);
    }

    void rjob::Deinitialize()
    {
        while(jobSystem->runningWorkers)
            Platform::Yield();

        for(uint32 i = 0; i < maxFibers; i++)
            Platform::DestroyFiber(jobSystem->fiber[i].fiber)
    }

    void rjob::Startup(bool consumeCurrentThread)
    {
        jobSystem->running = true;
        uint i = 0;
        if(consumeCurrentThread)
        {
            i++;
        }
        for(; i < workerThreadCount; i++)
        {
            uint32 coreMask = 0x01 << i;
            jobSystem->workerThreads[i] = Platform::LaunchThread(WorkerStartup, i, schedulerStackSize, coreMask);
        }
        if(consumeCurrentThread)
        {
            jobSystem->workerThreads[0] = Platform::CurrentThread();
            Platform::SetThreadAffinity(jobSystem->workerThreads[0], 0x01);
            WorkerStartup(0);
        }
    }

    void rjob::Shutdown()
    {
        jobSystem->running = false;
    }

    uint32 rjob::GetCurrentThreadIndex()
    {
        return gCurrentThreadIdx;
    }
}
