//
// Created by bruno on 28/02/2020.
//

#include "MarkAndSweep.h"
#include "domain_state.h"
#include <assert.h>
#include <stdio.h>


/* @STATIC FUNCTION DECLARATION*/

static mlvalue* big_elem_new(size_t size, GC_global_data* data);
static mlvalue* free_list_new(size_t size, GC_global_data* data);
static void display_big_elem(GC_global_data* data);
static void display_free_list(GC_global_data* data);
static void purge_mem(int nothing, void* _data);
static void iterate_and_mark(mlvalue v);
static void set_color(mlvalue v, color_t col);
static void remove_elem_big_list(Elem_list* list);
static void remove_page_elem(Page list, Elem_list* free_list);
static void set_size(Elem_list elem, int size);
static Elem_list first_fit_get_elem(Elem_list* free_list, int size);
static void ajust_cell(Elem_list* free_list, Elem_list* elem_to_ajust, int size);
static void new_page(GC_global_data* data);
/*STATIC FUNCTION DECLARATION @*/

void init_gc_data(GC_global_data* data){
    #ifdef VERBOSE
        printf("\033[1;31mInit %s init with:\n", __FILE__);
        printf("\t last_gc_size = %d\n\033[0m", FIRSTGCAT);
    #endif
    data->last_gc_size = FIRSTGCAT;
    data->current_memory_size = 0;
    data->big_obj_list = NULL;
    data->free_list = NULL;
    data->page = NULL;

    on_exit(purge_mem, data);
}

mlvalue* new(size_t size, GC_global_data* data){

    assert(size >= 8);

    data->current_memory_size += size;

//    if(size >= 32 * KB)
        return big_elem_new(size, data);

    return free_list_new(size, data);
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

    return &(elem->header);
}

mlvalue* free_list_new(size_t size, GC_global_data* data){
    Elem_list ptr;

    ptr = first_fit_get_elem(&(data->free_list), size);

    if(NULL == ptr) {
        new_page(data);
        ptr = first_fit_get_elem(&(data->free_list), size);
    }

    ajust_cell(&(data->free_list), &ptr, size);

    #ifdef VERBOSE
        printf("\033[1;31mFirst fit get %p %p->body with %ld octet\n\033[0m", ptr, ptr->body, Size_hd(ptr->header));
    #endif // VERBOSE

    return ptr->body;
}

void new_page(GC_global_data* data){
    Page elem = malloc(sizeof(Elem_list) + sizeof(mlvalue) +sizeof(Page) + 64 * KB);
    set_size(&elem->cell, 64 * KB);

    #ifdef VERBOSE
        printf("\033[1;31mNew page at %p with %ld bytes\n\033[0m", elem, Size_hd(elem->cell.header));
    #endif // VERBOSE
    elem->next = data->page;
    data->page = elem;

    elem->cell.next = data->free_list; // insertion in head
    data->free_list = &(elem->cell);
    #ifdef VERBOSE
        printf("\033[1;31mNew free elem at %p\n\033[0m", data->free_list);
    #endif // VERBOSE

}

void display_big_elem(GC_global_data* data){
    for(Elem_list e = data->big_obj_list; e != NULL; e = e->next){
        printf("%p\n", e+1);
    }
}
void display_free_list(GC_global_data* data){
    for(Elem_list e = data->free_list; e != NULL; e = e->next){
        printf("%p: %ld\n", e, Color_hd(*e->body));
    }
}

void gc_if_necessary(GC_global_data* data){
    if (data->last_gc_size * 1.5 <= data->current_memory_size)
        gc(data);
}

void gc(GC_global_data* data){
    static int i = 0;

#ifdef VERBOSE
        printf("\033[1;31mGC triggered with current_memory = %u and last_gc_size = %u\033[0m\n",
                data->current_memory_size, data->last_gc_size);
    #endif // VERBOSE

    // iterate on accessible object
    iterate_and_mark(Caml_state->env);

    for(int i = 0; i < Caml_state->sp; i++){
        iterate_and_mark(Caml_state->stack[i]);
    }
    if(i++) {
        for (int i = 0; i < Size(Caml_state->env); i++) {
            iterate_and_mark((Ptr_val(Caml_state->env)[i]));
        }
    }
    iterate_and_mark(Caml_state->accu);
    iterate_and_mark(Caml_state->accu2);

    /* extraction in list  */

    remove_elem_big_list(&(data->big_obj_list));
    remove_page_elem((data)->page, &(data->free_list));
    // iterate on page and add to the free_list the unlink object
     data->last_gc_size = data->current_memory_size;
}

void iterate_and_mark(mlvalue v){
    if(v == 0 || Is_long(v) || Color(v) != WHITE){
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

    assert(Color_hd((*lst)->header) == WHITE ||
        Color_hd((*lst)->header) == BLACK);

    assert(Is_block((*lst)->body));
    Elem_list temp;

    if(WHITE == Color_hd(((*lst)->header))){
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
        set_color((mlvalue )((*lst)->body), WHITE);
        remove_elem_big_list(&((*lst)->next));
    }
}

void set_size(Elem_list elem, int size){
    elem->header &= 0x3FF;
    elem->header |= size<<10;

    assert(Size_hd(elem->header) == size);
}

Elem_list first_fit_get_elem(Elem_list* free_list, int size){
    if(NULL == (*free_list)){
        return NULL;
    }
    Elem_list temp;
    if(Size_hd((*free_list)->header) > size) {
        temp = *free_list; // copy
        *free_list = temp->next;
        #ifdef VERBOSE
            printf("\033[1;31mExtract %p of freelist\033[0m\n", free_list);
        #endif // VERBOSE
        return temp;
    }
    return first_fit_get_elem(&((*free_list)->next), size);
}

void ajust_cell(Elem_list* free_list, Elem_list* elem_to_ajust, int size){

    Elem_list elem = (Elem_list )(((uint64_t) ((*elem_to_ajust)->body)) + size);
    set_size(elem, Size_hd((*elem_to_ajust)->header) - size);
    set_size(*elem_to_ajust, size);
    (*elem->body) &= ~(3 << 7);
    (*elem->body) |= (WHITE << 8);
    #ifdef VERBOSE
        printf("\033[1;32mAjust Ptr %p -> %d to %p:%ld\n\033[0m", *elem_to_ajust, size, elem, Size_hd((elem)->header));
    #endif // VERBOSE
    elem->next = *free_list; // insertion in head
    *free_list = elem;
}

void remove_page_elem(Page list, Elem_list* free_list){
    int i;
    Elem_list e;

//    display_free_list(&Caml_state->gc_data);

    *free_list = NULL;

    for(Page p = list; p != NULL; p = p->next){
//        printf("Page: %p:\n\t", p);
        e = &(p->cell); i = 0;
        while(i < 64 * KB) {

            Elem_list temp = (Elem_list) ((uint64_t) e + 16 + Size_hd(e->header));
            if((Color_hd(*temp->body == WHITE) && Color_hd(*e->body) == WHITE)
                    && i + Size_hd(e->header) <= 64 * KB){
//                printf(" [ Merging of %p and %p :", e, temp);
                set_size(e, 16 + Size_hd(e->header) + Size_hd(temp->header));
//                printf("%ld octet] ", Size_hd(e->header));
//                printf("%p %ld %ld ", e, Color_hd(*e->body), Size_hd(e->header));
            }
            else {
//                printf("%p %ld %ld ", e, Color_hd(*e->body), Size_hd(e->header));
                if(Color_hd(*e->body) == WHITE){
                    e->next = *free_list; // insertion in head
                    *free_list = e;
//                    printf("(Add %p to the free list)", e);
                }
                i += Size_hd(e->header) + 16;
                e = temp;
                (*e->body) &= ~(3 << 7);
                (*e->body) |= (WHITE << 8);
            }

        }
//        printf("\n");
    }
}

void purge_mem(int nothing, void* _data){
    GC_global_data* data = (GC_global_data*)_data;
    Elem_list tmp;
    for(Elem_list e = data->big_obj_list; e != NULL; e = tmp){
        tmp = e->next;
        free(e);
        #ifdef VERBOSE
                printf("\033[1;31mFinal purge of big alloc%p\033[0m\n", e);
        #endif // VERBOSE
    }
    Page tmp2;
    for(Page e = data->page; e != NULL; e = tmp2){
        tmp2 = e->next;
        free(e);
        #ifdef VERBOSE
                printf("\033[1;31mFinal purge of page %p\033[0m\n", e);
        #endif // VERBOSE
    }

    free(Caml_state->stack);
    free(Caml_state);

}

#undef ISCONSISTENTSTATE