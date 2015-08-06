#ifndef _RJOB_H_
#define _RJOB_H_

typedef signed char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

namespace rjob
{

    typedef volatile uint32* JobCounter;

    enum struct JobPriority
    {
        PriorityHigh = 0,
        PriorityNormal,
        PriorityLow,
        PriorityCount
    };

    typedef void (JobEntry*)(void* user);

    struct JobDesc
    {
        JobEntry entry;
        void* user;
        JobPriority priority;

        JobDesc(JobEntry _entry, void* _user) : entry(_entry), user(_user), priority(JobPriority::PriorityNormal)
        {}

        JobDesc(JobEntry _entry, void* _user, JobPriority _priority) : entry(_entry), user(_user), priority(_priority)
        {}
    };

    uint32 GetRequiredMemory();

    // Initial Job system initialization, needs to be called before anything else.
    void Initialize(void* memory);

    // Called at final shutdown to clean up any resources. Needs to be called after Shutdown,
    // on a non-worker thread.
    void Deinitialize();

    // Startup worker threads and begins execution. With consumeCurrentThread true it
    // will turn the current thread into a worker.
    void Startup(bool consumeCurrentThread = true);

    // Begins worker thread shutdown, may be called from inside a worker thread.
    void Shutdown();

    uint32 GetCurrentThreadIndex();

    void RunJobs(JobDesc* jobs, uint32 count = 1, JobCounter finishCounter = nullptr);

    JobCounter AllocCounter();
    void AcquireCounter(JobCounter counter);
    void FreeCounter(JobCounter counter);
    void WaitForCounter(JobCounter counter, uint32 value = 0);
    void WaitForCounterAndFree(JobCounter counter, uint32 value = 0);

}

#endif //_RJOB_H_
