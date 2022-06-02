/*
   simple driver for a counterfeit texas inst ads1115 adc 
   datasheet is in docs.

   the quickstart guide on p35 describes how the bytes go 
   back and forth:
        - write high byte first.
        - read returns high byte first.

   given the lab setting, we jam everything into one file --- 
   after your code works you should pull the ads1115 code into
   its own file so you can add it easily to other projects.
 */
#include "rpi.h"
#include "i2c.h"
#include "ads1115.h"
#include "libc/bit-support.h"

// for bitfield checking
#include "libc/helper-macros.h"

    // print config
static void ads1115_print(const char *msg, uint32_t c) {
        printk("<%s> config:\n", msg);
        printk("\tOS[15]=%b\n", bit_get(c,15));
        printk("\tMUX[12:14]=%b\n", bits_get(c,12,14));
        printk("\tPGA[9:11]=%b\n", bits_get(c,9,11));
        printk("\tMODE[8]=%b\n", bit_get(c,8));
        printk("\tDR[5:7]=%b\n", bits_get(c,5,7));
        printk("\tCOMP_MODE[4]=%b\n", bit_get(c,4));
        printk("\tCOMP_POL[3]=%b\n", bit_get(c,3));
        printk("\tCOMP_LAT[2]=%b\n", bit_get(c,2));
        printk("\tCOMP_Q[0:1]=%b\n", bits_get(c,0,1));
    }
// p21 states device will reset itself when you do an
// i2c "general call" reset with 6h (6 hex = 0x6)
// we need to do this b/c  ---- and this is an easy 
// mistake to make --- even though the pi reboots,
// the adc (and all sensors) are still up and running,
// so have the previous configuration.
void ads1115_reset(void) {
    uint8_t rst = 0x6;
    i2c_write(0, &rst,1);
}

// write a 16 bit register: 
//  fill in a 3 byte array.
//    1. first byte is register name <reg>
//    2. next two bytes are the value <v> (in what order?)
//  send it:
//    3. call i2c_write with <dev_addr> and the array.
void ads1115_write16(uint8_t dev_addr, uint8_t reg, uint16_t v) {
    uint8_t data [] = {reg, (v >> 8) & 0xFF, v & 0xFF};
    i2c_write(dev_addr, data, 3);
}

// read a 16-bit register
// 1. write the register address (1 byte i2c_write)
// 2. read the two bytes that come back (a 2-byte i2c_read)
// 3. reconstruct the 16-bit value.
uint16_t ads1115_read16(uint8_t dev_addr, uint8_t reg) {
    i2c_write(dev_addr, &reg, 1);
    uint8_t data[2];
    i2c_read(dev_addr, data, 2);
    return ((uint16_t)data[0] << 8) | ((uint16_t)data[1]);
}

// returns the device addr: hard-coded configuration atm.
uint8_t ads1115_config(void) {
    delay_ms(30);   // allow time for device to boot up.
    i2c_init();
    delay_ms(30);   // allow time to settle after init.

    // dev address: p23
    enum { dev_addr = 0b1001000};
    // panic("set dev_addr!\n");

    // p28
    // one way to set fields in a register.
    //   note, these are not clearing them.
#   define PGA(x) ((x)<<(9))
#   define MODE(x) ((x)<<(8))
#   define DR(x) ((x)<<5)
#   define MUX(x) ((x)<<12)

#   define MODE_V(x) bits_get(x,8,8)    // ((x) >> 8)&1)
#   define DR_V(x)   bits_get(x,5,7)  // (((x)>>5)&0b111)
#   define PGA_V(x)  bits_get(x,9,11)  // (((x)>>9)&0b111)
#   define MUX_V(x)  bits_get(x,12,14) // (((x)>>12)&0b111)

    // reset device to known state.
    ads1115_reset();

    // we seem to have to wait for power up.
    delay_us(25);

    // 1. get the initial config
	uint16_t c = ads1115_read16(dev_addr, config_reg);
	printk("initial: config reg = =%x (lo=%b,hi=%b)\n", c,c&0xff,c>>8);

    // 2. sanity check the default default values from p29
    //   mode: 8 is 1
    //   dr: 7:5 = 0b100
    //   pg: 11:9 = 010
    assert(c & (1 << 8));
    assert(c & (0b100 << 5));
    assert(c & (0b010 << 9));

    // 3. set the config to:
    //  - PGA gain to +/-4v 
    //  - MODE to continuous.
    //  - DR to 860sps
    // see page 28.
    c  = (0b100 << 12) | (0b001 << 9) | (0b111 << 5);

	printk("writing config: %x\n", c);
    ads1115_write16(dev_addr, config_reg, c);

    // 4. read back the config and make sure the fields we set
    // are correct.
    uint16_t new_config = ads1115_read16(dev_addr, config_reg);
    assert(new_config & (0b001 << 9));
    assert(new_config & (0b111 << 5));

    return dev_addr;
}
