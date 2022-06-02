#ifndef __ARMV6_DEBUG_IMPL_H__
#define __ARMV6_DEBUG_IMPL_H__
#include "armv6-debug.h"

// all your code should go here.  implementation of the debug interface.

// co-processor 14
coproc_mk(dscr, p14, 0, c0, c1, 0)
coproc_mk(wcr0, p14, 0, c0, c0, 7)
coproc_mk(bcr0, p14, 0, c0, c0, 5)
coproc_mk(wvr0, p14, 0, c0, c0, 6)
coproc_mk(wfar, p14, 0, c0, c6, 0)
coproc_mk(bvr0, p14, 0, c0, c0, 4)
// co-processor 15
coproc_mk(dfsr, p15, 0, c5, c0, 0)
coproc_mk(ifsr, p15, 0, c5, c0, 1)
coproc_mk(ifar, p15, 0, c6, c0, 2)

// Functions made using coproc_mk
static inline uint32_t cp15_dfsr_get(void);
static inline uint32_t cp15_ifar_get(void);
static inline uint32_t cp15_ifsr_get(void);
static inline uint32_t cp14_dscr_get(void);
static inline uint32_t cp14_bcr0_get(void);
static inline uint32_t cp14_wvr0_get(void);
static inline uint32_t cp14_wfar_get(void);
static inline void cp14_bcr0_set(uint32_t r);
static inline void cp14_wcr0_set(uint32_t r);
static inline void cp14_wvr0_set(uint32_t r);
static inline void cp14_bvr0_set(uint32_t r);

// return 1 if enabled, 0 otherwise.  
//    - we wind up reading the status register a bunch:
//      could return its value instead of 1 (since is 
//      non-zero).
// P. 13-9 says if bit 15 is 1, monitor debug-mode is enabled. If bit 14 is 0,
// monitor debug-mode is selected.
static inline int cp14_is_enabled(void) {
    uint32_t dscr = cp14_dscr_get();
    return bit_is_on(dscr, 15) && bit_is_off(dscr, 14);
}

// enable debug coprocessor 
static inline void cp14_enable(void) {
    // if it's already enabled, just return?
    if(cp14_is_enabled())
        panic("already enabled\n");

    // for the core to take a debug exception, monitor debug mode has to be both 
    // selected and enabled --- bit 14 clear and bit 15 set.
    uint32_t dscr = cp14_dscr_get();
    dscr = bit_set(dscr, 15);
    dscr = bit_clr(dscr, 14);
    cp14_dscr_set(dscr);

    assert(cp14_is_enabled());
}

// disable debug coprocessor
static inline void cp14_disable(void) {
    if(!cp14_is_enabled())
        return;

    uint32_t dscr = cp14_dscr_get();
    dscr = bit_clr(dscr, 15);
    dscr = bit_set(dscr, 14);
    cp14_dscr_set(dscr);
    assert(!cp14_is_enabled());
}

// These use the helper functions below.
static inline int cp14_bcr0_is_enabled(void) {
    return bit_is_on(cp14_bcr0_get(), 0);
}

/*
  3. set a simple breakpoint.  from 13-45:
         1. read the bcr
         2. clear enable bit: write back
         3. write the imva to BVR
         4. write BCR: BCR[22:21] = 00 or 1
            BCR[20] = 0 no linking
            BCR[15:14]: secure access as required
            BCR[8:5] base addres as required
            BCR[2:1]: super visor access as required
            BCR[0] = 1
        prefetch flush.
 */


static inline void cp14_bcr0_enable(void) {
    uint32_t bcr0 = cp14_bcr0_get();
    // Match
    bcr0 = bits_clr(bcr0, 21, 22);
    // Disable linking
    bcr0 = bit_clr(bcr0, 20);
    // Match in secure and non-secure world
    bcr0 = bits_clr(bcr0, 14, 15);
    // Match load of any location in word
    bcr0 = bits_set(bcr0, 5, 8, 0b1111);
    // Supervisor or not
    bcr0 = bits_set(bcr0, 1, 2, 0b11);
    // Enabled
    bcr0 = bit_set(bcr0, 0);
    cp14_bcr0_set(bcr0);
}
static inline void cp14_bcr0_disable(void) {
    uint32_t bcr0 = bit_clr(cp14_bcr0_get(), 0); // Clear bit 0 to disable breakpoint
    cp14_bcr0_set(bcr0);
}

// was this a brkpt fault?
static inline int was_brkpt_fault(void) {
    // IFSR was debug, 3-67
    uint32_t ifsr = bits_get(cp15_ifsr_get(), 0, 3);
    if (ifsr != 0b0010)
        return 0;
    // DSCR was breakpoint
    uint32_t dscr = bits_get(cp14_dscr_get(), 2, 5);
    if (dscr != 0b0001)
        return 0;
    return 1;
}

// was watchpoint debug fault caused by a load?
static inline int datafault_from_ld(void) {
    return bit_isset(cp15_dfsr_get(), 11) == 0;
}
// ...  by a store?
static inline int datafault_from_st(void) {
    return !datafault_from_ld();
}


// 13-33: tabl 13-23
static inline int was_watchpt_fault(void) {
    // DFSR
    uint32_t dfsr = bits_get(cp15_dfsr_get(), 0, 3);
    if (dfsr != 0b0010)
        return 0;
    // DSCR was watchpoint
    uint32_t dscr = bits_get(cp14_dscr_get(), 2, 5);
    if (dscr != 0b0010)
        return 0;
    return 1;
}

static inline int cp14_wcr0_is_enabled(void) {
    return bit_is_on(cp14_wcr0_get(), 0);
}
// 13-22 has instructions for this
static inline void cp14_wcr0_enable(void) {
    uint32_t wcr0 = cp14_wcr0_get();
    // Turn off linking
    wcr0 = bit_clr(wcr0, 20);
    // Watchpoints both in secure and non-secure
    wcr0 = bits_clr(wcr0, 14, 15);
    // For both loads and stores
    wcr0 = bits_set(wcr0, 3, 4, 0b11);
    // For both privileged and user
    wcr0 = bits_set(wcr0, 1, 2, 0b11);
    // Enable
    wcr0 = bit_set(wcr0, 0);
    // Byte address select for all accesses (0x0, 0x1, 0x2, 0x3)
    //wcr0 = bits_set(wcr0, 5, 8, 0b1111);
    // Put back into wcr0
    cp14_wcr0_set(wcr0);
}
static inline void cp14_wcr0_disable(void) {
    uint32_t wcr0 = bit_clr(cp14_wcr0_get(), 0);
    cp14_wcr0_set(wcr0);
}

// Get watchpoint fault using WFAR
static inline uint32_t watchpt_fault_pc(void) {
    return cp14_wfar_get() - 8;
}

// Get the address that the load/store was on
static inline uint32_t watchpt_fault_addr(void) {
    uint32_t fault_addr = cp14_wvr0_get() & ~0b11; // drop the last two bits
    uint32_t bitmask = bits_get(cp14_wcr0_get(), 5, 8);
    for (int i = 0; i < 4; i++) {
        if (bitmask>>i & 1) {
            return fault_addr | i;
        }
    }
    panic("Didn't find the fault addr!");
}
    
#endif
