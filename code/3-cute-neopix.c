/*
 * my cute neopixel array 
 */
#include "rpi.h"
#include "WS2812B.h"
#include "neopixel.h"

#define NPIX 60 

// the pin used to control the light strip.
enum { pix_pin = 21 };

// draw all pixels the same color
void set_all(neo_t h, uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < NPIX; i++) {
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

void hannah(neo_t h, int delay) {
    set_all(h, 87, 131, 158);
    // r: up 71
    // g: down 39
    // b: down 12
    for (int i = 0; i < 12; i++) {
        set_all(h, 87 + 6*i, 131 - 3*i, 158 - i);
        delay_ms(delay);
    }
    for (int i = 0; i < 12; i++) {
        set_all(h, 158 - 6*i, 92 + 3*i, 146 + i);
        delay_ms(delay);
    }
}

void bi(neo_t h, int delay) {
    // r: 255 b: 255 g: 0
    for (int g = 0; g < 256; g++) {
        set_all(h, 0, g, 255 - g);
        delay_ms(delay);
    }
    for (int g = 255; g >= 0; g--) {
        set_all(h, 0, g, 255);
        delay_ms(delay);
    }
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

void notmain(void) {
    enable_cache(); 
    gpio_set_output(pix_pin);

    // make sure when you implement the neopixel 
    // interface works and pushes a pixel around your light
    // array.
    unsigned npixels = NPIX;  // you'll have to figure this out.
    neo_t h = neopix_init(pix_pin, npixels);
    
    while(1){
        //halloween(h, 20);
        //hannah(h, 10);
        //bi(h, 10);
        //set_all(h, 255, 0, 0);
        rainbow(h, 0);
    }
    output("done!\n");
}
