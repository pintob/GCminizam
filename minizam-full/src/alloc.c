#include <stdlib.h>
#include <stdio.h>
#include "alloc.h"
#include "mlvalues.h"
#include "domain_state.h"


/* ================================================== */

mlvalue* caml_alloc(size_t size) {
#ifndef __GC_SET__
  return aligned_alloc(8,size);
#endif // __GC_SET__
#ifdef __USE_MARK_AND_SWEEP
    // todo triggerGC ???
    return new(size, &Caml_state->gc_data);
#endif

}

void displayStack(){
    mlvalue temp;
    for(int i = 0; i < Caml_state->sp; i++){
        temp = Caml_state->stack[i];
        printf("%d: ", i);
        if(Is_long(temp)){
            printf("%ld\n", Long_val(temp));
        }
        else{
            switch (Tag(temp)){
                case ENV_T: printf("ENV_T %s\n", val_to_str(temp)); break;
                case CLOSURE_T: printf("CLOSURE_T\n");  break;
                case BLOCK_T: printf("BLOCK_T: size: %ld at %p\n", Size(temp), (mlvalue *)temp-1); break;
                                                                                      // temp -1 to access the header
                default:
                    fprintf(stderr, "FATAL ERROR ON SWITCH %s line %d\n", __FILE__, __LINE__);
                    exit(1);
            }
        }

    }
}
