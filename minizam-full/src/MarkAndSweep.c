//
// Created by bruno on 28/02/2020.
//

#include "MarkAndSweep.h"
#include <assert.h>

void init_gc_data(GC_global_data* data){
    data->last_gc_size = data->current_memory_size = 0;
    data->big_obj_list = NULL;
}

void bootstrap(GC_global_data* data){
    assert(data->last_gc_size == 0);
    assert(data->current_memory_size == 0);
    assert(data->big_obj_list == NULL);

    /*create a page*/
}