//
// Created by bruno on 28/02/2020.
//


//#define __USE_MARK_AND_SWEEP
#ifdef __USE_MARK_AND_SWEEP

#ifndef MINIZAM_ETU_MARKANDSWEEP_H
#define MINIZAM_ETU_MARKANDSWEEP_H

#ifdef __GC_SET__
#error "Couldn't use two gc"
#endif // __GC_SET__
#define __GC_SET__

typedef struct{
    int foo;
    char* bar;
}GC_global_data;

void init_gc_data(GC_global_data* data);

#endif //MINIZAM_ETU_MARKANDSWEEP_H
#endif //__USE_MARK_AND_SWEEP