//
// Created by bruno on 28/02/2020.
//




#define __USE_MARK_AND_SWEEP
#ifdef __USE_MARK_AND_SWEEP

#ifndef MINIZAM_ETU_MARKANDSWEEP_H
#define MINIZAM_ETU_MARKANDSWEEP_H

#ifdef __GC_SET__
#error "Couldn't use two gc"
#endif // __GC_SET__
#define __GC_SET__

#include <stdlib.h>
#include "mlvalues.h"
#include "config.h"
#define VERBOSE

typedef struct{
    mlvalue *big_obj_list;
    mlvalue *free_list;
    unsigned int current_memory_size;
    unsigned int last_gc_size;
}GC_global_data;

typedef mlvalue Page[KB * 64 / sizeof(mlvalue)];

void init_gc_data(GC_global_data* data);

mlvalue* new(size_t size, GC_global_data* data);
void gc_if_necessary(GC_global_data* data);
void gc(GC_global_data* data);

#endif //MINIZAM_ETU_MARKANDSWEEP_H
#endif //__USE_MARK_AND_SWEEP