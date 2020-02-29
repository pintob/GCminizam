#include <stdlib.h>
#include <stdio.h>
#include "alloc.h"
#include "mlvalues.h"
#include "domain_state.h"

static void displayStack();

/* ================================================== */

mlvalue* caml_alloc(size_t size) {
//  displayStack();
  return aligned_alloc(8,size);
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
                case ENV_T: printf("ENV_T\n"); break;
                case CLOSURE_T: printf("CLOSURE_T\n");  break;
                case BLOCK_T: printf("BLOCK_T: size: %ld", Size_hd(temp)); break;
                default:
                    fprintf(stderr, "FATAL ERROR ON SWITCH %s line %d\n", __FILE__, __LINE__);
                    exit(1);
            }
        }

    }
}
