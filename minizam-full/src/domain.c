#include <stdlib.h>

#include "domain_state.h"
#include "config.h"
#include "mlvalues.h"


caml_domain_state* Caml_state; // <- this thing it GLOBAL

void caml_init_domain() {

  Caml_state = malloc(sizeof(caml_domain_state));
  Caml_state->sp = 0;

  Caml_state->stack = malloc(Stack_size);

  init_gc_data(&(Caml_state->gc_data));

}
