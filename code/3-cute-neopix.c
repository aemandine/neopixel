/*
 * my cute neopixel array 
 */
#include "rpi.h"
#include "WS2812B.h"
#include "neopixel.h"
#include "ads1115.h"

#define LOW_NPIX 0
#define NPIX 60 

// the pin used to control the light strip.
enum { pix_pin = 21 };

// draw all pixels the same color
void set_all(neo_t h, uint8_t r, uint8_t g, uint8_t b) {
    for (int i = LOW_NPIX; i < NPIX; i++) {
        neopix_write(h, i, r, g, b); 
    }
    neopix_flush(h);
}

void set_half(neo_t h, uint8_t r, uint8_t g, uint8_t b, int start) {
    for (int i = start; i < NPIX; i+=2) {
        neopix_write(h, i, r, g, b);
    }
    neopix_flush(h);
}

void set_one(neo_t h, uint8_t r, uint8_t g, uint8_t b, int index) {
    for (int i = 0; i < index; i++) {
        neopix_write(h, i, 0, 0, 0);
    }
    neopix_write(h, index, r, g, b);
    for (int i = index + 1; i < NPIX; i++) {
        neopix_write(h, i, 0, 0, 0);
    }
    neopix_flush(h);
}

void halloween(neo_t h, int delay) {
    for (int g = 0; g < 128; g ++) {
        set_all(h, 2*g, g, 0);
        delay_ms(delay);
    }
    for (int g = 127; g >= 0; g--) {
        set_all(h, 2*g, g, 0);
        delay_ms(delay);
    }
    delay_ms(3*delay);
}

void other(neo_t h, int delay) {
    for (int i = 30; i < NPIX; i++) {
        set_one(h, 255, 255, 255, i);
        delay_ms(delay);
    }
}

void rainbow(neo_t h, int delay) {
    // red to yellow
    for (int g = 0; g < 256; g++) {
        set_all(h, 255, g, 0);
        delay_ms(delay);
    }
    // yellow to green
    for (int r = 255; r >= 0; r--) {
        set_all(h, r, 255, 0);
        delay_ms(delay);
    }
    // green to cyan
    for (int b = 0; b < 256; b++) {
        set_all(h, 0, 255, b);
        delay_ms(delay);
    }
    // cyan to blue
    for (int g = 255; g >= 0; g--) {
        set_all(h, 0, g, 255);
        delay_ms(delay);
    }
    // blue to magenta
    for (int r = 0; r < 256; r++) {
        set_all(h, r, 0, 255);
        delay_ms(delay);
    }
    // magenta to red
    for (int b = 255; b >= 0; b--) {
        set_all(h, 255, 0, b);
        delay_ms(delay);

    }
}
void rainbow_one(neo_t h, int interval) {
    
    // red to yellow
    if (interval < 256) {
        int g = interval;
        set_all(h, 255, g, 0);
    } else if (interval < 512) {
        interval -= 256;
        int r = 256 - interval; 
        set_all(h, 0, 0, 0); 
    }
    // yellow to green
    for (int r = 255; r >= 0; r--) {
        set_one(h, r, 255, 0, LOW_NPIX + r % (NPIX - LOW_NPIX));
    }
    // green to cyan
    for (int b = 0; b < 256; b++) {
        set_one(h, 0, 255, b, LOW_NPIX + b % (NPIX - LOW_NPIX));
    }
    // cyan to blue
    for (int g = 255; g >= 0; g--) {
        set_one(h, 0, g, 255, LOW_NPIX + g % (NPIX - LOW_NPIX));
    }
    // blue to magenta
    for (int r = 0; r < 256; r++) {
        set_one(h, r, 0, 255, LOW_NPIX + r % (NPIX - LOW_NPIX));
    }
    // magenta to red
    for (int b = 255; b >= 0; b--) {
        set_one(h, 255, 0, b, LOW_NPIX + b % (NPIX - LOW_NPIX));

    }
}
#define THRESHOLD 100
#define AVG_NUM 10
void project(neo_t h) {
    unsigned dev_addr = ads1115_config();
    set_all(h, 0, 0, 255);
    short last[AVG_NUM];
    for (int i = 0; i < AVG_NUM; i++){
        last[i] = 0;
    }
    int i = 0;
    while (1) {
        short v = ads1115_read16(dev_addr, conversion_reg);
        last[i % AVG_NUM] = v;
        i++;
        int avg = 0;
        for (int i = 0; i < AVG_NUM; i++) {
            avg += last[i];
        }
        avg /= AVG_NUM;
        if (avg > THRESHOLD) {
            set_all(h, 255, 0, 0);
        } else {
            set_all(h, 0, 0, 255);
        }

        output("%d\n", v);
        delay_ms(50);
    }

}


void notmain(void) {
    enable_cache(); 
    gpio_set_output(pix_pin);

    // make sure when you implement the neopixel 
    // interface works and pushes a pixel around your light
    // array.
    unsigned npixels = NPIX;  // you'll have to figure this out.
    neo_t h = neopix_init(pix_pin, npixels);
    project(h);
    output("done!\n");
}
