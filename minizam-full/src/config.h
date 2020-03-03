#ifndef _CONFIG_H
#define _CONFIG_H

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)

/* The size of the stack. */
/* Note that it doesn't hold 8MB mlvalues, but rather 8MB of bytes. */
/* No boundary-checks are done: stack overflow will silently corrupt
   the heap; probably causing something to go wrong somewhere. */
/* TODO: auto-growing stack, or throw stack overflow when needed. */
/* Gaël fprintf(stderr ...) et exit(1) sont assez partique au pire */

#define Stack_size (8 * MB)

#ifdef __USE_MARK_AND_SWEEP
#define FIRSTGCAT (1 * KB)
#define AJUST(size) ((size & ~0x7) + 8) // maybe change to ~0xf + 16
#endif // __USE_MARK_AND_SWEEP

#endif
