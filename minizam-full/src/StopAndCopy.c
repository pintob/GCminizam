//
// Created by bruno on 28/02/2020.
//
#include <stdlib.h>
#include <stddef.h>
#include "StopAndCopy.h"
#include "domain_state.h"
#include <assert.h>
#include <stdio.h>
#ifdef __USE_STOP_AND_COPY__

void init_gc_data(GC_global_data* data){
        data =malloc(sizeof(GC_global_data));
        data->fromspace = malloc(32*KB);
        data->tospace = malloc(32*KB);
        data->alloc_pointer = 0 ;
        data->alloc_taille = (32*KB)/sizeof(mlvalue);
        /*data->Map_Pointer_Mlvalue =malloc(sizeof(Pair_Mlvalue)*data->stack_taille);*/

}

mlvalue* new(size_t size, GC_global_data* data){
    /*commence par verifier s'il y a besoin de faire un gc
     * puis allou la variable a partir du pointeur de tat*/
    gc_if_necessary(data);
    int temp =data->alloc_pointer;
    /*allou dans le tableau un objet */
    data->alloc_pointer +=(size) ;
    return (data->tospace+temp);
}
void gc_if_necessary(GC_global_data* data){
    /*verifie s'il reste de la place dans le tat courent si oui la fonction ternmine.
     * sinon on fait un appel au GC*/
    if(data->alloc_pointer == data->alloc_taille) {
        gc(data);
    }
}
/*regard la stack courent
 * puis copy les bloc vivant dans le tospace
 * mais a jours les pairs pointeur réel de l'objet et pointeur virtuel
 * */
int need_copy(mlvalue v){
        if(Tag(*Ptr_val(v))!=FWD_PTR_T){
            return 0;
        }
        else{
            return 1;
        }
}
/* copie les cases mémoire pointer par la mlvalue dans le fromspace et remplace
 * dans tospace l'anciennevaleur par le pointeur ver la nouvelle case memoire.
 * Mais a jour le pointeur d'allocation de fromspace*/
void copy(mlvalue v,GC_global_data* data,int* apf){
    size_t size = Size(*Ptr_val(v));
    for(int i = -1 ; i <size;i++) {
        data->fromspace[(*apf + i)] =*(Ptr_val(v)+i);
    }
    Hd_val(v)=Make_header(Size(v),Color(v),FWD_PTR_T);
    *Ptr_val(v)=(*apf+data->fromspace+1);
    v=(*apf+data->fromspace+1);
    *apf += size;
}

void gc(GC_global_data* data) {
    int alloc_pointer_from = 0;
    int* apf= &alloc_pointer_from;
    for(mlvalue i = 0; i < Caml_state->sp; i++){
        if(Is_block(Caml_state->stack[i])) {
            if (need_copy(Caml_state->stack[i])) {
                copy((Caml_state->stack[i]), data, apf);
            } else {
                Caml_state->stack[i] = *(Ptr_val(Caml_state->stack[i]));
            }
        }
    }
    for (uint64_t i = 0; i < Size(Caml_state->env); i++){
       if(Is_block((Ptr_val(Caml_state->env)[i])))
       {
           if (need_copy(Ptr_val(Caml_state->env)[i])){
            copy(Ptr_val(Caml_state->env)[i],data,apf);
        }
        else{
            (Ptr_val(Caml_state->env)[i])=(Ptr_val(Caml_state->stack)[i]);
        }
    }
    }


    mlvalue n = (Caml_state->accu);
    if(Is_block(n)){
        if (need_copy(n)){
            copy((n),data,apf);
        }

        else{
            n=*Ptr_val(n);
        }
    }
    mlvalue n2 = (Caml_state->accu2);
    if(Is_block(n2)) {
        if (need_copy(n2)) {

        } else {
            n2 = *Ptr_val(n2);
        }
    }
    mlvalue* temp =data->fromspace;
    data->fromspace=data->tospace;
    data->tospace=temp;
}
#endif