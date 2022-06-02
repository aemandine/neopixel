#ifndef __VECTOR_BASE_SET_H__
#define __VECTOR_BASE_SET_H__
#include "libc/bit-support.h"
#include "asm-helpers.h"

/*
 * vector base address register:
 *   3-121 --- let's us control where the exception jump table is!
 *
 * defines: 
 *  - vector_base_set  
 *  - vector_base_get
 */

static inline void *vector_base_get(void) {
    unsigned result;
    asm volatile ( "MRC p15, 0, %0, c12, c0, 0" : "=r" (result) );
    return (void *) result;
}

// set the vector register to point to <vector_base>.
// must: 
//    - check that it satisfies the alignment restriction.
static inline void vector_base_set(void *vector_base) {
    assert((unsigned) vector_base % 32 == 0); // make sure the last five bits are zero
    asm volatile ( "MCR p15, 0, %0, c12, c0, 0" : : "r" ((unsigned) vector_base) );
    assert(vector_base_get() == vector_base);
}
#endif
