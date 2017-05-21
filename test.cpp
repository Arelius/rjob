#include "rjob.h"

char jobmem[rjob::GetRequiredMemory()];

int main(int argc, char** argv)
{
    rjob::Initialize(jobmem);
    rjob::Startup(false);
    rjob::Shutdown();
    rjob::Deinitialize();
}
