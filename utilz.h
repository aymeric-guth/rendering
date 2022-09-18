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

static int clamp(int hi, int lo, int val)
{
    if (val > hi)
        return hi;
    else if (val < lo)
        return lo;
    else
        return val;
}

static int msleep(long msec)
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

static float linear_interpolation(
    float src_min,
    float src_max,
    float dst_min,
    float dst_max,
    float x
)
{
    return (x - src_min) * (dst_max - dst_min) / (src_max - src_min) + dst_min;
}

static float lin_int(float xa, float ya, float xb, float yb, float y)
{
    return xa + (xb - xa) * (y - ya) / (yb - ya);
}
