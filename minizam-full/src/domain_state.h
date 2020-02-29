#ifndef _DOMAIN_STATE_H
#define _DOMAIN_STATE_H

#include "mlvalues.h"
#include "GC.h"

typedef struct _caml_domain_state {
  /* Stack */
  mlvalue* stack;
  unsigned int sp;

  GC_global_data gc_data;
  // TODO add globale variable for the gc
} caml_domain_state;

/* The global state */
extern caml_domain_state* Caml_state;  // use this glob' variable for the GC !

/* Initialisation function for |Caml_state| */
void caml_init_domain();

#endif
