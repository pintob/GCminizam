//
// Created by bruno on 28/02/2020.
//

#include "MarkAndSweep.h"
#include "domain_state.h"
#include <assert.h>
#include <stdio.h>

#ifdef VERBOSE
// my code
#endif

/* @STATIC FUNCTION DECLARATION*/

static mlvalue* big_elem_new(size_t size, GC_global_data* data);
static void display_big_elem(GC_global_data* data);
static void purge_mem(int nothing, void* _data);
static void iterate_and_mark(mlvalue v);
/*STATIC FUNCTION DECLARATION @*/

#define ISCONSISTENTSTATE \
    assert(data->last_gc_size*2 >= data->current_memory_size); // <- the gc will be triggered before

void init_gc_data(GC_global_data* data){
    #ifdef VERBOSE
        printf("\033[1;31mInit %s init with:\n", __FILE__);
        printf("\t last_gc_size = %d\n\033[0m", FIRSTGCAT);
    #endif
    data->last_gc_size = FIRSTGCAT;
    data->current_memory_size = 0;
    data->big_obj_list = NULL;
    data->free_list = NULL;

    on_exit(purge_mem, data);

}

mlvalue* new(size_t size, GC_global_data* data){
//    ISCONSISTENTSTATE

    size = AJUST(size);

    data->current_memory_size += size;
    if(NULL != data->free_list){
        ; // todo extraction with a first (next?) fit strategy

        // UPGRADE : use few free list and use a best fit strategy ???
    }
    // else { <- when the free_list will be implemented

    return big_elem_new(size, data);
}

mlvalue* big_elem_new(size_t size, GC_global_data* data){

    mlvalue* elem = malloc(size + sizeof(mlvalue));

    elem[0] = (mlvalue) data->big_obj_list; // insertion in head
    data->big_obj_list = elem;
    #ifdef VERBOSE
    printf("\033[1;31mBig alloc of %lu octet at %p\n", size, elem+1);
    printf("Current mem size: %u, last gc mem: %u\n\033[0m", data->current_memory_size, data->last_gc_size);
    #endif // VERBOSE

    return elem+1;
}

void display_big_elem(GC_global_data* data){
    for(mlvalue* e = data->big_obj_list; e != NULL; e = (mlvalue *)e[0]){
        printf("%p\n", e+1);
    }
}


void gc_if_necessary(GC_global_data* data){
    if (data->last_gc_size * 1.5 <= data->current_memory_size)
        gc(data);
}

void gc(GC_global_data* data){
    #ifdef VERBOSE
        printf("\033[1;31mGC triggered with current_memory = %u and last_gc_size = %u\033[0m\n",
                data->current_memory_size, data->last_gc_size);
    #endif // VERBOSE
    // todo
    // iterate on Caml_state->stack and mark the object

    // iterate on big object and free the unlink object

    // iterate on page and at to the free_list the unlink object
//    data->last_gc_size = data->current_memory_size;
}

void purge_mem(int nothing, void* _data){
    GC_global_data* data = (GC_global_data*)_data;
    mlvalue* tmp;
    for(mlvalue* e = data->big_obj_list; e != NULL; e = tmp){
        tmp = (mlvalue *)e[0];
        free((void*)e);
#ifdef VERBOSE
        printf("\033[1;31mFinal purge of %p\033[0m\n", e);
#endif // VERBOSE
    }
    free(Caml_state->stack);
    free(Caml_state);

}

void iterate_and_mark(mlvalue v){
    if(Is_long(v) || Color(v) != WHITE){
        return;
    }
    for(int i = 0; i < Size(v); i++){
        #ifdef VERBOSE
            printf("\033[1;34mMark %p\033[0m\n", (mlvalue *)v);
        #endif // VERBOSE

        // mark v
        iterate_and_mark(v + i * sizeof(mlvalue));
    }
}

#undef ISCONSISTENTSTATE