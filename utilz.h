#include <time.h>
#include <errno.h>

#define MAX(a,b) \
    ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })
#define MIN(a,b) \
    ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a < _b ? _a : _b; })

// int clamp(int hi, int lo, int val)
// {
//     return MAX(MIN(hi, val), lo);
// }
int clamp(int hi, int lo, int val)
{
    if (val > hi)
        return hi;
    else if (val < lo)
        return lo;
    else
        return val;
}

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}
