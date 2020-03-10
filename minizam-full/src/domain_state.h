#ifndef _DOMAIN_STATE_H
#define _DOMAIN_STATE_H

#include "mlvalues.h"
#include "GC.h"

typedef struct _caml_domain_state {
  /* Stack */
  mlvalue* stack;
  mlvalue accu;
  mlvalue env;

    //not the most effective option but the simplest
  unsigned int sp;

  GC_global_data gc_data;
} caml_domain_state;

/* The global state */
extern caml_domain_state* Caml_state;  // use this glob' variable for the GC !

/* Initialisation function for |Caml_state| */
void caml_init_domain();

#endif
