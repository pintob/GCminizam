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

typedef struct _ml_list{
    struct _big_list* next;
    mlvalue elem;
}Mlvalue_list;

typedef struct{
    Mlvalue_list *big_obj_list;
    unsigned int current_memory_size;
    unsigned int last_gc_size;
}GC_global_data;

typedef mlvalue Page[KB * 64 / sizeof(mlvalue)];

void init_gc_data(GC_global_data* data);

void bootstrap(GC_global_data* data);
#endif //MINIZAM_ETU_MARKANDSWEEP_H
#endif //__USE_MARK_AND_SWEEP