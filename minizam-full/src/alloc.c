#include <stdlib.h>
#include <stdio.h>
#include "alloc.h"
#include "mlvalues.h"
#include "domain_state.h"


/* ================================================== */

mlvalue* caml_alloc(size_t size) {
#ifdef VERBOSE
    printf("\033[1;31mRequest of %ld octets\n", size);
#endif // VERBOSE
#ifndef __GC_SET__
  return aligned_alloc(8,size);
#endif // __GC_SET__
#ifdef __USE_MARK_AND_SWEEP
    gc_if_necessary(&Caml_state->gc_data);
    return new(size, &Caml_state->gc_data);
#endif

}

void displayStack(){
    mlvalue temp;
    for(mlvalue i = 0; i < Caml_state->sp; i++){
        temp = Caml_state->stack[i];
        printf("%d: ", i);
        if(Is_long(temp)){
            printf("%ld\n", Long_val(temp));
        }
        else{
            switch (Tag(temp)){
                case ENV_T: printf("ENV_T: size: %ld at %p | color:%ld\n",
                        Size(temp), (mlvalue *)temp, Color(temp)); break;
                case CLOSURE_T: printf("CLOSURE_T: size: %ld at %p | color:%ld\n",
                        Size(temp), (mlvalue *)temp, Color(temp)); break;
                case BLOCK_T: printf("BLOCK_T: size: %ld at %p | color:%ld\n",
                        Size(temp), (mlvalue *)temp, Color(temp)); break;
                default:
                    fprintf(stderr, "FATAL ERROR ON SWITCH %s line %d with %p\n", __FILE__, __LINE__, (mlvalue *)temp-2);
                    exit(1);
            }
        }

    }
}
