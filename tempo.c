#include "tempo.h"
#include <time.h>

double now_seconds(void) {
#if defined(CLOCK_MONOTONIC)
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t) == 0) {
        return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
    }
#endif
    return (double)clock() / (double)CLOCKS_PER_SEC;
}
