#include "breakpoint.h"
#include "rpi.h"
#include "debug-fault.h"
// this will mismatch on the first instruction at user level.
void brkpt_mismatch_start(void) {
    if (!cp14_is_enabled()) {
        cp14_enable();
    }
    unsigned value = cp14_bcr0_get();
    value = bit_set(value, 0); // enable cp14
    value = bits_set(value, 1, 2, 0b11); // supervisor or not
    value = bits_set(value, 5, 8, 0b1111); // match any location in word
    value = bits_clr(value, 14, 15); // match in secure and non-secure world
    value = bit_clr(value, 20); // disable linking
    value = bits_set(value, 21, 22, 0b10); // mismatch
    cp14_bcr0_set(value);
}

// stop mismatching.
void brkpt_mismatch_stop(void) {
    if (!cp14_is_enabled()) {
        cp14_enable();
    }
    cp14_bcr0_disable();
}

// set a mismatch on <addr> --- call the prefetch abort handler on mismatch.
//  - you cannot get mismatches in "privileged" modes (all modes other than
//    USER_MODE)
//  - once you are in USER_MODE you cannot switch modes on your own since the 
//    the required "msr" instruction will be ignored.  if you do want to 
//    return from user space you'd have to do a system call ("swi") that switches.
void brkpt_mismatch_set(uint32_t addr) {
    brkpt_mismatch_stop();
    cp14_bvr0_set(addr);
    brkpt_mismatch_start();
}

