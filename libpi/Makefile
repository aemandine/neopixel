# simple definitions for making libpi.a
#  - you should only ever have to modify SRC or STAFF_OBJS

# copy any file you want to add to libpi.a to the libpi/src
# directory.  and then add it to this variable.  
#
# for example if you want to add your gpio.c, 
#   1. copy gpio.c to src/gpio.c
#   2. define SRC as:
#           SRC := ./src/gpio.c
#
# tip: if you want to automatically add all .S and .c files 
# in src/ do:
#   SRC := $(wildcard ./src/*.[Sc])
#
# currently not defined to hold anything
SRC := $(wildcard ./src/*.[Sc])

# if you want to use our gpio.o or other .o (e.g., if yours
# is acting weird):
#   1. remove the file from SRC and 
#   2. add it to STAFF_OBJS 
#
#
# currently: use our uart.o (for printk) and gpio.o
STAFF_OBJS  :=  ./staff-objs/gpio.o     \
                ./staff-objs/uart.o     \
                ./staff-objs/gpio-pud.o     \
                ./staff-objs/gpio-int.o 

STAFF_OBJS += staff-objs/interrupts-asm.o 
STAFF_OBJS += staff-objs/interrupts-vec-asm.o

# automatically add all staff src from:  libc, 
# staff-src and ./staff-start.S
#
# you should not have to modify this variable.  
SRC :=  $(SRC)                              \
        $(wildcard ./libc/*.[Sc])           \
        $(wildcard ./staff-src/*.[Sc])      \
        staff-start.S

DEPS += ./Makefile ./manifest.mk

include ./manifest.mk
