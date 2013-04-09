#include <stdio.h>
#include <string.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE 50
#endif

typedef struct {
    int xsum, ysum, zsum;
    unsigned char aptr;
    short xs[ARRAY_SIZE];
    short ys[ARRAY_SIZE];
    short zs[ARRAY_SIZE];
} Stat;

void stats_init();
void stats_add(short, short, short);

Stat stats;

#define STATS_XAVG (stats.xsum / ARRAY_SIZE)
#define STATS_YAVG (stats.ysum / ARRAY_SIZE)
#define STATS_ZAVG (stats.zsum / ARRAY_SIZE)

void stats_init() {
    stats.xsum = 0;
    stats.ysum = 0;
    stats.zsum = 0;
    stats.aptr = 0;

    memset(stats.xs, 0, sizeof(short) * ARRAY_SIZE);
    memset(stats.ys, 0, sizeof(short) * ARRAY_SIZE);
    memset(stats.zs, 0, sizeof(short) * ARRAY_SIZE);

    return;
}

void stats_add(short x, short y, short z) {
    stats.xsum += x;
    stats.ysum += y;
    stats.zsum += z;

    stats.xsum -= stats.xs[stats.aptr];
    stats.ysum -= stats.ys[stats.aptr];
    stats.zsum -= stats.zs[stats.aptr];

    stats.xs[stats.aptr] = x;
    stats.ys[stats.aptr] = y;
    stats.zs[stats.aptr] = z;

    if (++stats.aptr == ARRAY_SIZE) stats.aptr = 0;

    return;
}
