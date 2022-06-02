#include "rpi.h"
#include "rpi-interrupts.h"
#include "vector-base.h"

void int_init_reg(void *int_vector_addr) {
    PUT32(Disable_IRQs_1, 0xffffffff);
    PUT32(Disable_IRQs_2, 0xffffffff);
    dev_barrier();
    extern unsigned _interrupt_table;
    vector_base_set(&_interrupt_table);
}
