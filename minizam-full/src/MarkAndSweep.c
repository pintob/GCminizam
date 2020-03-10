//
// Created by bruno on 28/02/2020.
//

#include "MarkAndSweep.h"
#include "domain_state.h"
#include <assert.h>
#include <stdio.h>


/* @STATIC FUNCTION DECLARATION*/

static mlvalue* big_elem_new(size_t size, GC_global_data* data);
static void display_big_elem(GC_global_data* data);
static void purge_mem(int nothing, void* _data);
static void iterate_and_mark(mlvalue v);
static void set_color(mlvalue v, color_t col);
static void remove_elem_big_list(Elem_list* list);
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

    assert(size >= 8);

    data->current_memory_size += size;
    if(NULL != data->free_list){
        ; // todo extraction with a first (next?) fit strategy

        // UPGRADE : use few free list and use a best fit strategy ???
    }
    // else { <- when the free_list will be implemented

    return big_elem_new(size, data);
}

mlvalue* big_elem_new(size_t size, GC_global_data* data){

    assert(size >= 8); // greater than empty block

    Elem_list elem = malloc(size + sizeof(Elem_list));

    elem->next = data->big_obj_list; // insertion in head
    data->big_obj_list = elem;
    #ifdef VERBOSE
        printf("\033[1;31mBig alloc of %lu octet at %p\n", size + sizeof(Elem_list), elem);
        printf("Current mem size: %u, last gc mem: %u\n\033[0m", data->current_memory_size, data->last_gc_size);
    #endif // VERBOSE

    return (elem->body);
}

void display_big_elem(GC_global_data* data){
    for(Elem_list e = data->big_obj_list; e != NULL; e = e->next){
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

    // iterate on accessible object
    iterate_and_mark(Ptr_val(Caml_state->env));

    for(int i = 0; i < Caml_state->sp; i++){
        iterate_and_mark(Caml_state->stack[i]);
    }
    for(int i = 0; i < Size(Caml_state->env); i++){
        iterate_and_mark((Ptr_val(Caml_state->env)[i]));
    }
    iterate_and_mark(Ptr_val(Caml_state->accu));


    /* extraction in list  */

    remove_elem_big_list(&(data->big_obj_list));

    // iterate on page and at to the free_list the unlink object
    // data->last_gc_size = data->current_memory_size;
}

void iterate_and_mark(mlvalue v){
    if(Is_long(v) || Color(v) != WHITE){
        return;
    }

    #ifdef VERBOSE
        printf("\033[1;34mMark %p\033[0m, %ld\n", (void *)v, Tag(v));
    #endif // VERBOSE
    set_color(v, BLACK);

    for(int i = 0; i < Size(v); i++){
        iterate_and_mark(((mlvalue*)v)[i]);
    }
}

void set_color(mlvalue v, color_t col){
    assert(WHITE == col ||
            BLACK == col);

    assert(Is_block(v));

    mlvalue hd = Hd_val(v);

    hd &=  ~(3<<7);
    hd |= (col << 8);

    ((mlvalue *)v)[-1] = hd;

    assert(Color(v) == col);
}

void remove_elem_big_list(Elem_list* lst){
    assert(lst != NULL);

    if(NULL == (*lst))
        return;

    assert(Color_hd((*lst)->body[0]) == WHITE ||
        Color_hd((*lst)->body[0]) == BLACK);

    assert(Is_block(&(*lst)->body[1]));
    Elem_list temp;

    if(WHITE == Color_hd(((*lst)->body[0]))){
        temp = *lst;
        *lst = temp->next;
        free(temp);
        remove_elem_big_list(lst);
        #ifdef VERBOSE
            printf("\033[1;32mSweep of %p\033[0m\n", temp);
        #endif // VERBOSE
    }
    else{
        #ifdef VERBOSE
            printf("\033[1;32mUnmark of %p\033[0m\n", *lst);
        #endif // VERBOSE
        set_color((mlvalue )&((*lst)->body[1]), WHITE);
        remove_elem_big_list(&((*lst)->next));
    }
}

void purge_mem(int nothing, void* _data){
    GC_global_data* data = (GC_global_data*)_data;
    Elem_list tmp;
    for(Elem_list e = data->big_obj_list; e != NULL; e = tmp){
        tmp = e->next;
        free(e);
        #ifdef VERBOSE
                printf("\033[1;31mFinal purge of %p\033[0m\n", e);
        #endif // VERBOSE
    }
    free(Caml_state->stack);
    free(Caml_state);

}

#undef ISCONSISTENTSTATE