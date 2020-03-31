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

#include <stdlib.h>
#include "mlvalues.h"
#include "config.h"
//#define VERBOSE
#define NDEBUG
typedef struct _f{
    struct _f *next;
    mlvalue header;
    mlvalue body[];
}Cell_list, *Elem_list;

typedef struct _g{
    struct _g *next;
    Cell_list cell;
}*Page;

typedef struct{
    Elem_list big_obj_list;
    Elem_list free_list;
    Page page;
    unsigned int current_memory_size;
    unsigned int last_gc_size;
}GC_global_data;

void init_gc_data(GC_global_data* data);

mlvalue* new(size_t size, GC_global_data* data);
void gc_if_necessary(GC_global_data* data);
void gc(GC_global_data* data);

#endif //MINIZAM_ETU_MARKANDSWEEP_H
#endif //__USE_MARK_AND_SWEEP