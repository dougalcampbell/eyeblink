/**
 * eyeblink.h
 * 
 * Create a pair of blinking eyes that fade in, blink a few times, then
 * fade away. Header.
 *
 * Suggestions and GitHub pull requests are welcome.
 * 
 * @author Dougal Campbell <dougal@gunters.org>
 * @see https://community.particle.io/t/halloween-blinky-eyes/16065
 * @see http://dougal.gunters.org/
 */
#include "application.h"
#include "neopixel/neopixel.h"

#define EYBLINK_VERSION "1.0.0"
/**
 * The various states for our state machnine, which is handled in our `step()`
 * method. See the diagram in `eyeblink.cpp`.
 */
enum States {
	WAITING,       // Dark. Waiting to appear...
	FADEIN,        // Going from dark to on...
	ON,            // Fully on. Maybe blinking.
	OFF,           // The off part of a blink.
	FADEOUT        // Going back to dark. And waiting.
};

/**
 * If Serial is connected, you can set the `loglevel` property of this
 * instance, to get some info. I suggest LOG_INFO. 
 */
enum LogLevel {
	LOG_OFF,       // Shut up.
	LOG_ERROR,     // Just tell me about critical errors
	LOG_WARN,      // Warning! Warning! Danger, Will Robinson!
	LOG_INFO,      // Here's some info about what I'm doing
	LOG_DEBUG,     // Here's some low-level details
	LOG_TRACE      // FIREHOSE!
};

class Eyeblink {
	public:
		// constructor
		Eyeblink(Adafruit_NeoPixel* s, uint16_t start, uint8_t sep, uint32_t col);
		Eyeblink();
		~Eyeblink();
		
		// getters/setters
		uint32_t getColor();
		uint32_t getColorCurrent();
		uint16_t getFadeInTime();
		uint16_t getFadeOutTime();
		uint8_t	getBlinksMin();
		uint8_t getBlinksMax();
		uint8_t getBlinkCount();
		uint16_t getStartPos();
		uint8_t getEyeSep();
		uint32_t getStartEvent();
		uint32_t getNextEvent();
		uint8_t getState();
		Adafruit_NeoPixel* getStrip();

		void setColor(uint32_t);
		void setColorCurrent(uint32_t);
		void setFadeInTime(uint32_t);
		void setFadeOutTime(uint32_t);
		void setBlinksMin(uint8_t);
		void setBlinksMax(uint8_t);
		void setBlinkCount(uint8_t);
		void setStartPos(uint16_t);
		void setEyeSep(uint8_t);
		void setStartEvent(uint32_t);
		void setNextEvent(uint32_t);
		void setState(uint8_t);
		void setStrip(Adafruit_NeoPixel*);

        // primary work functions
		void step(void);
		void draw(void);
		void reset(void);
		uint32_t scaleColor(uint32_t color, uint32_t scale, uint32_t min, uint32_t max);

        ///< set debug level for serial logging. Assumes Serial.begin() was called elsewhere.
		uint8_t debuglevel = LOG_OFF;

	private:
		uint8_t state = WAITING;

        Adafruit_NeoPixel* strip;

		uint32_t color;
		uint32_t colorCurrent;
		uint16_t fadeInTime = 20000;
		uint16_t fadeOutTime;
		uint8_t	blinksMin;
		uint8_t blinksMax;
		uint16_t startPos;
		uint8_t eyeSep;
		uint8_t blinkCount = blinksMax;
		uint32_t startEvent, nextEvent;
		
};

// - fin - 
