#include "niel_instrumentation.h"

#include "base/mutex.h"
#include "globals.h"
#include "thread.h"

#include <ctime>

namespace art {

const int LOG_INTERVAL_SECONDS = 5;

Mutex instMutex("NielInstrumentationMutex", kLoggingLock);

time_t lastLogTime;

/* Locked with instMutex */
long numThreadLocalAllocs = 0;
long numNormalAllocs = 0;
long numLargeObjectAllocs = 0;

long sizeThreadLocalAllocs = 0;
long sizeNormalAllocs = 0;
long sizeLargeObjectAllocs = 0;
/* End locked with instMutex */

void NiRecordRosAllocThreadLocalAlloc(Thread * self, size_t size) {
    instMutex.ExclusiveLock(self);
    numThreadLocalAllocs++;
    sizeThreadLocalAllocs += size;
    instMutex.ExclusiveUnlock(self);
    maybePrintLog();
}
void NiRecordRosAllocNormalAlloc(Thread * self, size_t size) {
    instMutex.ExclusiveLock(self);
    numNormalAllocs++;
    sizeNormalAllocs += size;
    instMutex.ExclusiveUnlock(self);
    maybePrintLog();
}
void NiRecordLargeObjectAlloc(Thread * self, size_t size) {
    instMutex.ExclusiveLock(self);
    numLargeObjectAllocs++;
    sizeLargeObjectAllocs += size;
    instMutex.ExclusiveUnlock(self);
    maybePrintLog();
}

void maybePrintLog() {
    time_t currentTime = time(NULL);
    if (difftime(currentTime, lastLogTime) > LOG_INTERVAL_SECONDS) {
        LOG(INFO) << "NIEL total thread-local allocs: " << numThreadLocalAllocs
                  << " size: " << sizeThreadLocalAllocs
                  << " total normal allocs: " << numNormalAllocs
                  << " size: " << sizeNormalAllocs
                  << " total large-object allocs: " << numLargeObjectAllocs
                  << " size: " << sizeLargeObjectAllocs
                  << "\n";
        lastLogTime = currentTime;
    }
}

}
