/**
 * eyeblink.cpp
 * 
 * Create a pair of blinking eyes that fade in, blink a few times, then
 * fade away.
 * 
 * The Eyeblink animation is designed so that many instances can run simultaneously
 * on a single LED strip. And if other strip animation code also uses millis() 
 * scheduling and doesn't fiddle with the Neopixel pixels buffer, they could probably
 * run alongside this code, as well.
 * 
 * Suggestions and GitHub pull requests are welcome.
 * 
 * @author Dougal Campbell <dougal@gunters.org>
 * @see https://community.particle.io/t/halloween-blinky-eyes/16065
 * @see http://dougal.gunters.org/
 */

#include "eyeblinks.h"

// Constructor 
Eyeblink::Eyeblink(Adafruit_NeoPixel* s, uint16_t start, uint8_t sep, uint32_t col) 
{
    setStrip(s);
    startPos = start;
    eyeSep = sep;
    color = col;
    
    state = WAITING;
    colorCurrent = 0x00000000;
    fadeInTime = random(0, 4000) + 1000;
    fadeOutTime = random(500, 4000);
    blinksMin = 2;
    blinksMax = 6;
    blinkCount = random(blinksMin, blinksMax);
    startEvent = millis();
    nextEvent = startEvent + random(15000);
    if (debuglevel >= LOG_INFO) {
        Serial.println("Eyeblink init");
    }
}

/**
 * Empty constructor
 * 
 * Sets a few sane defaults, but gives you complete freedom to
 * override them, of course. Dont forget to call setStrip(),
 * setStartEvent(), and setNextEvent() in your main code.
 */
Eyeblink::Eyeblink()  
{
    state = WAITING;
    startPos = 0;
    eyeSep = 2;
    color = 0x00ffffff;
    colorCurrent = 0x00000000;
    fadeInTime = 1000;
    fadeOutTime = 1000;
    blinksMin = 0;
    blinksMax = 5;
    blinkCount= random(blinksMin, blinksMax + 1);
    startEvent = 0;
    nextEvent = 0;
    if (debuglevel >= LOG_INFO) {
        Serial.println("Eyeblink init");
    }
}

Eyeblink::~Eyeblink()  
{
    // Go dark on destruction
    color = 0x00000000;
    colorCurrent = color;
    draw();
}

/**
 * State machine. Wait for the nextEvent time, and change states.
 * 
 *                     (blinkCount)
 *                       +-----+
 *                       v     |
 * WAITING -> FADEIN -> ON -> OFF
 *    ^                  |
 *    |                  v
 *    +---------------FADEOUT
 * 
 * @TODO * Put the more of the wait times into variables that can be configured
 *         at run-time. E.g., blink on/off time.
 * 
 *       * Smart automatic randomization/distribution of eyes along the strip.
 * 
 *       * Matrix support?
 */
void Eyeblink::step() {
    uint32_t currentTime = millis();
    
    switch (state) {
        // "Baby, what time is it?"
        case WAITING:
            if (currentTime > nextEvent) {
                // transition WAITING -> FADEIN
                state = FADEIN;
                startEvent = currentTime;
                nextEvent = startEvent + fadeInTime;
                if (debuglevel >= LOG_INFO) {
                    Serial.print("WAITING complete. Going to FADEIN in ");
                    Serial.print(nextEvent - startEvent);
                    Serial.println("ms...");
                }
            }
            break;
        case FADEIN:
            if (currentTime > nextEvent) {
                // transition FADEIN -> ON
                state = ON;
                colorCurrent = color;
                startEvent = currentTime;
                nextEvent = startEvent + random(1000, 4000);
                if (debuglevel >= LOG_INFO) {
                    Serial.print("FADEIN complete. Going to ON in ");
                    Serial.print(nextEvent - startEvent);
                    Serial.println("ms...");
                }
            } else {
                // We're in mid-fade:
                // scale color according to current time, in relation to start/end time
                colorCurrent = scaleColor(color, currentTime, startEvent, nextEvent);
            }
            break;
        case ON:
            if (currentTime > nextEvent) {
                if (blinkCount >= 0) {
                    // blinking... transition ON -> OFF
                    state = OFF;
                    colorCurrent = 0x00000000; // black
                    startEvent = currentTime;
                    nextEvent = startEvent + random(50, 150);
                    if (debuglevel >= LOG_INFO) {
                        Serial.print("ON complete. Going to OFF in ");
                        Serial.print(nextEvent - startEvent);
                        Serial.println("ms...");
                    }
                } else {
                    // fade out... transition ON -> FADEOUT
                    state = FADEOUT;
                    startEvent = currentTime;
                    nextEvent = startEvent + fadeOutTime;
                }
            }
            break;
        case OFF:
            if (currentTime > nextEvent) {
                // blinking... transition OFF -> ON
                state = ON;
                colorCurrent = color;
                startEvent = currentTime;
                nextEvent = startEvent + random(100, 1500);
                --blinkCount; // reduce number of remaining blinks
                if (debuglevel >= LOG_TRACE) {
                    Serial.print("OFF complete. Back to ON in ");
                    Serial.print(nextEvent - startEvent);
                    Serial.println("ms...");
                }
            }
            break;
        case FADEOUT:
            if (currentTime > nextEvent) {
                // transition FADEOUT -> WAITING
                state = WAITING;
                colorCurrent = 0x00000000; // black
                nextEvent = currentTime + 10000 + random(50000) - random(4000) - random(500) - random(500);
                startEvent = currentTime;
                blinkCount = random(blinksMin, blinksMax+1);
                if (debuglevel >= LOG_INFO) {
                    Serial.print("FADEOUT complete. Back to WAITING in ");
                    Serial.print(nextEvent - startEvent);
                    Serial.println("ms...");
                }
            } else {
                // scale color down according to current time, in relation to start/end time
                colorCurrent = scaleColor(color, currentTime, nextEvent, startEvent);
            }
            break;
        default:
            // THIS SHOULD NEVER HAPPEN! but reset, if it does...
            reset();

            if (debuglevel >= LOG_ERROR) {
                Serial.println("Where am I?!? Unknown state encountered.");
            }
    }

}

void Eyeblink::draw() {
    // Set our two eyes, according to the position, separation, and current color
    strip->setPixelColor(startPos, colorCurrent);
    strip->setPixelColor(startPos + eyeSep, colorCurrent);

    // NOISY!
    if (debuglevel >= LOG_TRACE) {
        Serial.print("startPos: ");
        Serial.print(startPos);
        Serial.print(", eyeSep: ");
        Serial.print(eyeSep);
        Serial.print(", color: ");
        Serial.println(color, HEX);
    }
}

uint32_t Eyeblink::scaleColor(uint32_t color, uint32_t scale, uint32_t min, uint32_t max) {
    /**
     * Cheap way to adjust brightness of a color, based on a sliding value between
     * a min and max.
     */
     
    // bust out the color components
    uint8_t r = color >> 16 & 0xff;
    uint8_t g = color >> 8 & 0xff;
    uint8_t b = color & 0xff;
    
    uint8_t factor = map(scale, min, max, 0, 255);

    // Scale the values 
    r = ((uint16_t) r * factor) / 255;
    g = ((uint16_t) g * factor) / 255;
    b = ((uint16_t) b * factor) / 255;

    uint32_t clr = ((uint32_t) r << 16 | (uint32_t) g << 8 | (uint32_t) b);

    // NOISY!
    if (debuglevel >= LOG_TRACE) {
        Serial.print("  scale: ");
        Serial.print(scale);
        Serial.print("  min: ");
        Serial.print(min);
        Serial.print("  max: ");
        Serial.print(max);
        Serial.print("  factor: ");
        Serial.println(factor);
        Serial.print("  original color: ");
        Serial.println(color, HEX);
        Serial.print("  scale: ");
        Serial.println(factor);
        Serial.print("  new color:      ");
        Serial.println(clr, HEX);
        Serial.print("  rgb: ");
        Serial.print(r, HEX);
        Serial.print(", ");
        Serial.print(g, HEX);
        Serial.print(", ");
        Serial.println(b, HEX);
    }

    return clr;
}

/**
 * reset()
 * 
 * Reset an instance
 */
void Eyeblink::reset() {
    startEvent = millis();
    nextEvent = startEvent + fadeInTime;
    colorCurrent = color;
    state = WAITING;
    if (debuglevel >= LOG_INFO) {
        Serial.print("RESET. Back to WAITING. Waiting for ");
        Serial.print(fadeInTime);
        Serial.println("ms...");
    }
}

/**
 * Getters
 */
uint32_t Eyeblink::getColor() {
     return color;
}

// getColorCurrent is probably of limited usefulness. I'm just including
// it for the sake of completeness. And who knows...?
uint32_t Eyeblink::getColorCurrent() {
     return colorCurrent;
}

uint16_t Eyeblink::getFadeInTime() {
     return fadeInTime;
}

uint16_t Eyeblink::getFadeOutTime() {
     return fadeOutTime;
}

uint8_t Eyeblink::getBlinksMin() {
     return blinksMin;
}

uint8_t Eyeblink::getBlinksMax() {
     return blinksMax;
}

uint8_t Eyeblink::getBlinkCount() {
     return blinkCount;
}

uint16_t Eyeblink::getStartPos() {
     return startPos;
}

uint8_t Eyeblink::getEyeSep() {
     return eyeSep;
}

uint32_t Eyeblink::getStartEvent() {
     return startEvent;
}

uint32_t Eyeblink::getNextEvent() {
     return nextEvent;
}

uint8_t Eyeblink::getState() {
    return state;
}

Adafruit_NeoPixel* Eyeblink::getStrip() {
    return (Adafruit_NeoPixel *) strip;
}

/**
 * Setters
 */
void Eyeblink::setColor(uint32_t newColor) {
    color = newColor;
}

// setColorCurrent is probably of limited usefulness. I'm just including
// it for the sake of completeness. And who knows...?
void Eyeblink::setColorCurrent(uint32_t newColorCurrent) {
    colorCurrent = newColorCurrent;
}

void Eyeblink::setFadeInTime(uint32_t newFadeInTime) {
    fadeInTime = newFadeInTime;
}

void Eyeblink::setFadeOutTime(uint32_t newFadeOutTime) {
    fadeOutTime = newFadeOutTime;
}

void Eyeblink::setBlinksMin(uint8_t newBlinksMin) {
    blinksMin = newBlinksMin;
}

void Eyeblink::setBlinksMax(uint8_t newBlinksMax) {
    blinksMax = newBlinksMax;
}

void Eyeblink::setBlinkCount(uint8_t newBlinkCount) {
    blinkCount = newBlinkCount;
}

void Eyeblink::setStartPos(uint16_t newStartPos) {
    startPos = newStartPos;
}

void Eyeblink::setEyeSep(uint8_t newEyeSep) {
    eyeSep = newEyeSep;
}

void Eyeblink::setStartEvent(uint32_t newStartEvent) {
    startEvent = newStartEvent;
}

void Eyeblink::setNextEvent(uint32_t newNextEvent) {
    nextEvent = newNextEvent;
}

void Eyeblink::setState(uint8_t newState) {
    state = newState;
}

void Eyeblink::setStrip(Adafruit_NeoPixel* newStrip) {
    // Keep a reference to a NeoPixel strip
    strip = newStrip;
}

// - fin -
