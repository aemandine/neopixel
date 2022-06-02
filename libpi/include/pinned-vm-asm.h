#ifndef __PINNED_VM_ASM_H__
#define __PINNED_VM_ASM_H__

// define these routines: i put them in pinned-vm.c


// arm1176.pdf: 3-149
//
// you can generate the _get and _set methods.
// for these inline assembly routines using the
// <cp_asm> macro in libpi/include/asm-helpers.h
// included:

// override return type
#undef ASM_RET_TYPE
#define ASM_RET_TYPE
#include "asm-helpers.h"

// arm1176.pdf: 3-149
static inline void lockdown_index_set(uint32_t x);
static inline uint32_t lockdown_index_get(void);

static inline void lockdown_va_set(uint32_t x);
static inline uint32_t lockdown_va_get(void);

static inline void lockdown_pa_set(uint32_t x);
static inline uint32_t lockdown_pa_get(void);

static inline void lockdown_attr_set(uint32_t x);
static inline uint32_t lockdown_attr_get(void);

static inline void xlate_pa_set(uint32_t x);

// routines to manually check that a translation
// can succeed.  we use these to check that 
// pinned translations are in the TLB.
// see:
//    p 3-80---3-82 in arm1176.pdf

// translate for a privileged read access
static inline void xlate_kern_rd_set(uint32_t x);

// translate for a priviledged write access
static inline void xlate_kern_wr_set(uint32_t x);

// get physical address after manual translation
static inline uint32_t xlate_pa_get(void);

// clean up the return type
#undef ASM_RET_TYPE

#endif
