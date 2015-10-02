/**
 * eyeblink-demo.ino
 *
 * Populate a WS2812B LED strip with pairs of blinking eyes.
 * 
 * Code for Particle Core/Photon, using the NeoPixel library.
 * 
 * @author Dougal Campbell <dougal@gunters.org>
 * @see https://community.particle.io/t/halloween-blinky-eyes/16065
 */

// Serial debugging on/off 
#define DBG 1

/*
 * I'm not going to rely on WiFi or cloud...
 * comment this out if you want to be able to do OTA updates!
 */
//SYSTEM_MODE(SEMI_AUTOMATIC);

#include "neopixel/neopixel.h"
#include "eyeblinks.h"

// Data Out pin from Core/Photon to LED strip DIN
#define DOUT A5

// Some good colors for Halloween
#define RED    0x00ff0000
#define GREEN  0x0000ff00
#define ORANGE 0x00ff7f00
#define ORANGE2 0x00ff5f00
#define ORANGE3 0x00ff3f00
#define PURPLE 0x007f00ff
#define PURPLE2 0x005f00ff
#define PURPLE3 0x009f00ff

/**
 * arrayCount
 * 
 * Returns the count of an array of objects. (thanks, StackOverflow!)
 */
template<typename T, size_t N>
size_t arrayCount( T(&)[N] )
{
  return(N);
}

uint8_t arrSize;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(120, DOUT, WS2812B);

// An array of eyeblink instances. {strip, position, eye separation, color}
Eyeblink blinkers[ ] = {
        {&strip, 0, 2, RED},
        //{&strip, 15, 3, GREEN},
        {&strip, 35, 2, PURPLE},
        //{&strip, 47, 1, ORANGE},
        {&strip, 51, 1, ORANGE2},
        //{&strip, 56, 2, ORANGE3},
        {&strip, 60, 2, PURPLE2},
        //{&strip, 67, 2, ORANGE2},
        {&strip, 72, 1, PURPLE3},
        //{&strip, 75, 1, ORANGE},
        {&strip, 79, 1, ORANGE3},
        //{&strip, 85, 2, GREEN},
        {&strip, 100, 2, RED},
        {&strip, 115, 2, PURPLE}
    };



void setup() {
    ///< Get the size of the blinkers array
    arrSize = arrayCount(blinkers);
    
    // Clear the lights before we start.
    strip.begin();
    strip.clear();
    strip.setBrightness(64);
    strip.show(); // set leds to black 

    // 15 second window for programming before we hog the CPU
    while (millis() < 15000) {
        Spark.process();
    }
    
    if (DBG) {
        Serial.begin(19200);
        Serial.println("Starting up...");
    
        // Let's enable serial log for one of the instances...
        blinkers[0].debuglevel = LOG_INFO;
    }
    
    RGB.control(true); // turn off the status light

    for (uint8_t i = 0; i < arrSize; i++) {
        blinkers[i].reset();
    }
}

void loop() {
    /**
     * Loop through the eyeblink instances, and update them.
     */
    for (uint8_t i = 0; i < arrSize; i++) {
        blinkers[i].step();
        blinkers[i].draw();
    }

    strip.show(); // send updates!
}

// - fin -
