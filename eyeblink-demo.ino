/**
 * eyeblink-demo.ino
 *
 * Populate a WS2812B LED strip with pairs of blinking eyes.
 * 
 * Code for Particle Core/Photon, using the NeoPixel library.
 * 
 * Suggestions and GitHub pull requests are welcome.
 * 
 * @author Dougal Campbell <dougal@gunters.org>
 * @see https://community.particle.io/t/halloween-blinky-eyes/16065
 * @see http://dougal.gunters.org/
 */

// Serial debugging on/off 
#define DBG 0

/*
 * Start up without WiFi and Cloud. Give the MODE button a quick
 * press to turn Cloud connection on/off!
 */
SYSTEM_MODE(MANUAL);

#include "neopixel/neopixel.h"
#include "eyeblinks.h"

// Data Out pin from Core/Photon to LED strip DIN
#define DOUT A5

// How many pixels are we driving?
#define NUM_PIXELS 120

// Some good colors for Halloween
#define BLACK     0x00000000
#define WHITE     0x00ffffff
#define RED       0x00ff0000
#define GREEN     0x0000ff00
#define ORANGE    0x00ff7f00
#define ORANGE2   0x00ff5f00
#define ORANGE3   0x00ff3f00
#define PURPLE    0x007f00ff
#define PURPLE2   0x005f00ff
#define PURPLE3   0x009f00ff

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

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, DOUT, WS2812B);

// An array of eyeblink instances: {&strip, position, eye separation, color}
Eyeblink blinkers[ ] = {
        {&strip,   0, 2, GREEN},
        //{&strip,  15, 3, RED},
        {&strip,  35, 2, PURPLE},
        //{&strip,  47, 1, GREEN},
        {&strip,  51, 1, ORANGE},
        //{&strip,  56, 2, ORANGE3},
        {&strip,  60, 2, PURPLE2},
        //{&strip,  67, 2, ORANGE2},
        {&strip,  72, 1, PURPLE3},
        //{&strip,  75, 1, ORANGE},
        {&strip,  79, 1, ORANGE2},
        //{&strip,  85, 2, GREEN},
        {&strip, 100, 2, ORANGE3},
        {&strip, 117, 2, RED}
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
        /**
         * If you are using firmware version 0.4.6 or higher, you can use
         * Particle.process(), because Spark.process is being deprecated.
         */
        Spark.process();
        // Particle.process();
    }
    
    if (DBG) {
        Serial.begin(19200);
        Serial.println("Starting up...");
    
        // Let's enable serial log for one of the instances...
        blinkers[0].debuglevel = LOG_INFO;
    }
    
    RGB.control(true); // turn off the status light

    for (uint8_t i = 0; i < arrSize; i++) {
        blinkers[i].setNextEvent( millis() + random(15000) );
    }
}

void loop() {
    /**
     * On MODE button click, Particle Cloud connect/disconnect
     */
    if (HAL_Core_Mode_Button_Pressed(100)) {    // 100ms press
        // Turn cloud connection on/off
        toggleCloud();
        
        /**
         * debounce 1/2 sec before resetting button state to
         * check for system mode change
         */
        delay (500); 
        HAL_Core_Mode_Button_Reset();
    }
    
    /**
     * Loop through the eyeblink instances, and update them.
     */
    for (uint8_t i = 0; i < arrSize; i++) {
        blinkers[i].step();
        blinkers[i].draw();
    }

    strip.show(); // send updates!
}

/**
 * Toggle Cloud connection.
 * 
 * If we are not connected to the Particle Cloud, then try to connect.
 * 
 * If we are already connected, then disconnect.
 * 
 * Onboard RGB LED will be off when not connected.
 */
void toggleCloud() {
    
    if (WiFi.ready()) {
        // Already connected to WiFi... Check cloud?
        if (Spark.connected()) {
            // Connected to cloud. We must be wanting to disconnect?
            Spark.disconnect();
            WiFi.disconnect();
            RGB.control(true);
        } else {
            RGB.control(false);
            Spark.connect();
        }
    } else {
        RGB.control(false);
        Spark.connect();
        
    }
}

// - fin -
