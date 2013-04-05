#ifndef Statistic_h
#define Statistic_h
//
//    FILE: Statistic.h
//  AUTHOR: Rob dot Tillaart at gmail dot com  
//          modified at 0.3 by Gil Ross at physics dot org
// PURPOSE: Recursive Statistical library for Arduino
// HISTORY: See Statistic.cpp
//
// Released to the public domain
//

// the standard deviation increases the lib (<100 bytes)
// it can be in/excluded by un/commenting next line
#define STAT_USE_STDEV

#include <math.h>

#define STATISTIC_LIB_VERSION "0.3.1"

class Statistic
{
        public:
        Statistic();
        void clear();
        void add(float);
        long count();
        float sum();
        float average();
        float minimum();
        float maximum();

#ifdef STAT_USE_STDEV
        float pop_stdev();          // population stdev
        float unbiased_stdev();
#endif

#ifdef STAT_CIRCULAR

#ifndef STAT_CIRC_SIZE
#define STAT_CIRC_SIZE 300
#endif

#if STAT_CIRC_SIZE > 65536
#define STAT_CIRC_SIZE 65536
#endif

        float _vals[STAT_BUF_SIZE];
        unsigned short _vptr;
#endif

protected:
        long _cnt;
        float _store;           // store to minimise computation
        float _sum;
        float _min;
        float _max;
#ifdef STAT_USE_STDEV
        float _ssqdif;              // sum of squares difference
#endif
};

#endif
// END OF FILE
