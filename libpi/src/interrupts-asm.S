/*
 * interrupt-asm.s
 *
 * Code for interrupt handling.  Refer to armisa.pdf in docs/ for what
 * the opcodes mean.
 */
#include "rpi-asm.h"



/*
 * Enable/disable interrupts.
 *
 * CPSR = current program status register
 *        upper bits are different carry flags.
 *        lower 8:
 *           7 6 5 4 3 2 1 0
 *          +-+-+-+---------+
 *          |I|F|T|   Mode  |
 *          +-+-+-+---------+
 *
 *  I : disables IRQ when = 1.
 *  F : disables FIQ when = 1.
 *  T : = 0 indicates ARM execution, = 1 is thumb execution.
 *      Mode = current mode.
 */
.globl system_enable_interrupts
system_enable_interrupts:
    mrs r0,cpsr         @ move process status register (PSR) to r0
    bic r0,r0,#(1<<7)	@ clear 7th bit.
    msr cpsr_c,r0		@ move r0 back to PSR
    bx lr		        @ return.

.globl system_disable_interrupts
system_disable_interrupts:
    mrs r0,cpsr		       
    orr r0,r0,#(1<<7)	       @ set 7th bit: or in 0b100 0000
    msr cpsr_c,r0
    bx lr
.align 5
.globl _interrupt_table
.globl _interrupt_table_end
_interrupt_table:
  ldr pc, _reset_asm
  ldr pc, _undefined_instruction_asm
  ldr pc, _software_interrupt_asm
  ldr pc, _prefetch_abort_asm
  ldr pc, _data_abort_asm
  ldr pc, _reset_asm
  ldr pc, _interrupt_asm
fast_interrupt_asm:
  sub   lr, lr, #4 @First instr of FIQ handler
  push  {lr}
  push  {r0-r12}
  mov   r0, lr              @ Pass old pc
  bl    fast_interrupt_vector    @ C function
  pop   {r0-r12}
  ldm   sp!, {pc}^

_reset_asm:                   .word reset_asm
_undefined_instruction_asm:   .word undefined_instruction_asm
_software_interrupt_asm:      .word software_interrupt_asm
_prefetch_abort_asm:          .word prefetch_abort_asm
_data_abort_asm:              .word data_abort_asm
_interrupt_asm:               .word interrupt_asm
_interrupt_table_end:

@ only handler that should run since we only enable general interrupts
interrupt_asm:
  mov sp, #INT_STACK_ADDR   @ i believe we have 512mb - 16mb, so this should be safe
  sub   lr, lr, #4

  push  {r0-r12,lr}         @ XXX: pushing too many registers: only need caller
  @ vpush {s0-s15}	        @ uncomment if want to save caller-saved fp regs

  mov   r0, lr              @ Pass old pc
  bl    interrupt_vector    @ C function

  @ vpop {s0-s15}           @ pop caller saved fp regs
  pop   {r0-r12,lr} 	    @ pop integer registers

  @ return from interrupt handler: will re-enable general ints.
  movs    pc, lr        @ moves the link register into the pc and implicitly
                        @ loads the PC with the result, then copies the 
                        @ SPSR to the CPSR.

@ change this to call your system call handler.
@ generally you only need to save the caller-saved registers (callee saved will be
@ saved by the syscall_vector anyways)
@ however, in this case, the caller has already saved all the caller-saved
@ registers and the callee has saved all the callee-saved registers, so all we
@ need to save is lr.
software_interrupt_asm:
    @ runs at system level (p.a2-5): assumes we have a sp
    @ implement: 
    @   - save regs
    @   - need to figure out what lr points to,
    @   - mov the original r0 into r1
    @   - mov the pointer to the syscall inst into r0
    @   - call
    @   - restore regs
    @   - return from the exception.
    push {lr}
    mov r1, r0
    sub r0, lr, #4
    bl syscall_vector
    pop {lr}
    movs pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ we don't generate any of these, will just panic and halt.
reset_asm:
  sub   lr, lr, #4
  bl    reset_vector
undefined_instruction_asm:
  sub   lr, lr, #4
  bl    undefined_instruction_vector

@ We're saving the caller saved registers because the callee saved registers
@ will be saved by the function handler. However there's no function that
@ triggers the interrupt, so the caller-saved ones aren't saved anymore.
prefetch_abort_asm:
  mov   sp, #INT_STACK_ADDR
  sub   lr, lr, #4
  push  {r0-r3,lr} 
  mov   r0, lr
  bl    prefetch_abort_vector
  pop   {r0-r3,lr}
  movs  pc, lr

@ Data abort needs us to do RA+8 (subtract 8 from lr), as described
@ in Chapter 13 (Debug) on page 13-36.
data_abort_asm:
  mov   sp, #INT_STACK_ADDR
  sub   lr, lr, #8
  push  {r0-r3,lr} 
  mov   r0, lr
  bl    data_abort_vector
  pop   {r0-r3,lr}
  movs  pc, lr
