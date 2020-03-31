//
// Created by bruno on 28/02/2020.
//
#include "config.h"
#include "mlvalues.h"

#define __USE_STOP_AND_COPY__ // <- set with -D compilation option
#ifdef __USE_STOP_AND_COPY__

#ifndef MINIZAM_ETU_STOPANDCOPY_H
#define MINIZAM_ETU_STOPANDCOPY_H

#ifdef __GC_SET__
#error "Couldn't use two gc"
#endif // __GC_SET__
#define __GC_SET__
#define VERBOSE
typedef enum {STACK1,STACK2}Flag;

/*typedef struct{
    mlvalue* inpointer;
    mlvalue* topointer;
}Pair_Mlvalue;
*/
typedef struct{
    mlvalue* fromspace;
    mlvalue* tospace;
    int alloc_pointer;
    int alloc_taille;
}GC_global_data;
/*initalise un GC*/
void init_gc_data(GC_global_data* data);

/*Ajoute une variable a l'environemt et renvoye un pointeur sur la case dans la stack*/
mlvalue* new(size_t size, GC_global_data* data);

/*test si il faut declancher le GC*/
void gc_if_necessary(GC_global_data* data,size_t size);

/*code du GC appel par gc_if_nes*/
void gc(GC_global_data* data);


#endif //MINIZAM_ETU_STOPANDCOPY_H

#endif //__USE_STOP_AND_STOP__