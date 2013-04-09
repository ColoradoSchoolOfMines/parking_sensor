#include "u3DStat.c"
#include <stdlib.h>

#define ARANDN  ((rand() % 2048) - 1024)

extern Stat stats;

int main(int argc, char** argv) {
    stats_init();

    int i = 0;
    while (++i < 5000) {
        stats_add(ARANDN, ARANDN, ARANDN);

        if (i % 10 == 0) {
            printf("xsum: %d\nysum: %d\nzsum: %d\nptr: %d\n\n", stats.xsum, stats.ysum, stats.zsum, stats.aptr);
        }
    }

    return 0;
}

