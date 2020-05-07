#include "rjob.h"

#include <catch2/catch.hpp>

TEST_CASE("rjob startup and shutdown", "[rjob]") {
    char jobmem[rjob::GetRequiredMemory()];
    SECTION("job system initialization, and deinitialization", "[rjob]") {
        rjob::Initialize(jobmem);
        SECTION("job system startup and shutdown", "[rjob]") {
            rjob::Startup(false);
            rjob::Shutdown();
        }
        rjob::Deinitialize();
    }
}
