/**
 * DESCRIPTION: 
 *    Visualization of a loudness level for the 
 *    Adafruit Circuit Playground in different modes. 
 *    
 *    
 * AUTHOR: Denis Zygann
 */
#include <Adafruit_CircuitPlayground.h>

#define SAMPLE_WINDOW   50  // Sample window for average level
#define INPUT_FLOOR     45  // Lower range of mic sensitivity in dB SPL
#define INPUT_CEILING   100  // Upper range of mic sensitivity in db SPL
// #define INPUT_DELTA     55

const static long PIXEL_COLOR_GREEN = 0x00FF00;
const static long PIXEL_COLOR_YELLOW = 0x808000;
const static long PIXEL_COLOR_RED = 0xFF0000;

const static int OFF = 0;
const static int INPUT_DELTA = INPUT_CEILING - INPUT_FLOOR;

static int mode = 0;
static int numberOfModes = 4;

int oldPosition = 0;


void setup() {
  Serial.begin(115200);
  CircuitPlayground.begin();

}


/**
 * This loop iterates over the different modes of the Adafruit.
 * The first few lines determine the sound pressure to decide 
 * which color should light up.  
 * 
 * The next lines are determine which mode is is used. 
 * The last mode turns off all pixels.
 * 
 * The last method checks if the user pressed the button to 
 * change the mode or to turn off the pixels. 
 */
void loop() {

  float peakToPeak = 0;   // peak-to-peak level
  //get peak sound pressure level over the sample window
  peakToPeak = CircuitPlayground.mic.soundPressureLevel(SAMPLE_WINDOW);


  peakToPeak = max(INPUT_FLOOR, peakToPeak);


  if (mode == 0) {
    if (peakToPeak < 60) {
      setAllPixels(PIXEL_COLOR_GREEN);
    } else if (peakToPeak < 85) {
      setAllPixels(PIXEL_COLOR_YELLOW);
    } else {
      setAllPixels(PIXEL_COLOR_RED);
    }
  } else if (mode == 1) {
    setPixels(peakToPeak);
  } else if (mode == 2){
    setPixelsWithThreeRounds(peakToPeak);
  } else {
    setAllPixels(OFF);
  }

  updateMode();

}


/**
 * This algorithm divides the pixels three sections. 
 * The first 4 pixels are the first section which are light up in green.
 * The next 3 pixels are the second section which are light up in yellow
 * and the last section are the remaining pixels which are light up in red.
 * 
 * The peakToPeak parameter determines how many pixels
 * are activated. 
 */
void setPixels(float peakToPeak) {

  int pixelStep = INPUT_DELTA / CircuitPlayground.strip.numPixels();

  for (int i = 0; i < CircuitPlayground.strip.numPixels(); i++) {

    if ((pixelStep * i) + INPUT_FLOOR < peakToPeak) {
      if (i < 4) {
        CircuitPlayground.setPixelColor(i, PIXEL_COLOR_GREEN);
      } else if (i < 7) {
        CircuitPlayground.setPixelColor(i, PIXEL_COLOR_YELLOW);
      } else {
        CircuitPlayground.setPixelColor(i, PIXEL_COLOR_RED);
      }
    } else {
      CircuitPlayground.setPixelColor(i, OFF);
    }
    delay(10);

  }

}


/**
 * Depend on the peakToPeak parameter this algorithm can make 
 * three rounds. In the first round the pixels are actived one 
 * after another until they reach the last pixel. The next pixel 
 * will light up in yellow and it goes round until the last pixel 
 * is yellow. The last round lets the pixels light up in red. 
 * 
 * In the next loop when this method is called there is a 
 * comparision. It checks if the new peakToPeak parameter is
 * higer, lower or the same as the one before. 
 * 
 * The peakToPeak value is mapped to a position on the Adafruit.
 * The higest position is 30 (3 * 10 pixels).  
 * 
 * If the position is the same as the position before the lights
 * will stay the same for 100 ms. 
 * 
 * If the current position is higer the next pixels are light up
 * in the respectively color. 
 * 
 * If the current position is lower one pixel after another is set 
 * off until it's reached the end and it changes all pixels to the
 * colors of the lower level. If its already the lowest level the pixels 
 * will stay off.
 */
void setPixelsWithThreeRounds(float peakToPeak) {

  int maxPosition = 30;
  int pixelStep = 2; // 1 is more sensible
  int numPixels = CircuitPlayground.strip.numPixels();
  float result = peakToPeak - INPUT_FLOOR;
  int currentPosition = result / pixelStep;

  if (oldPosition == currentPosition) {
    delay(100);
    return;
  }

  if (oldPosition < currentPosition) {

    for (int i = oldPosition; i < maxPosition; i++) {

      if (i <= currentPosition) {

        if (i < 10) {
          CircuitPlayground.setPixelColor(i % numPixels, PIXEL_COLOR_GREEN);
          delay(100);
        } else if (i > 9 && i < 20 ) {
          CircuitPlayground.setPixelColor(i % numPixels, PIXEL_COLOR_YELLOW);
          delay(100);
        } else {
          CircuitPlayground.setPixelColor(i % numPixels, PIXEL_COLOR_RED);
          delay(100);
        }

      }

    }

  }


  if (oldPosition > currentPosition) {

    for (int i = maxPosition-1; i >= currentPosition; i--) {
      if (oldPosition < i) {
        // doNothing
      } else if (i > 19) {
        // turn off
        CircuitPlayground.setPixelColor(i % numPixels, PIXEL_COLOR_YELLOW);

        delay(100);

        if (i == 20) {
          setAllPixels(PIXEL_COLOR_YELLOW);
        }
      } else if (i > 9 && i < 20 ) {
        CircuitPlayground.setPixelColor(i % numPixels, PIXEL_COLOR_GREEN);

        delay(100);

        if (i == 10) {
          setAllPixels(PIXEL_COLOR_GREEN);
        }
      } else if (i < 10) {

        CircuitPlayground.setPixelColor(i % numPixels, OFF);
        delay(100);
      }

    }


  }


  oldPosition = currentPosition;

}


/**
 * This method gets a color and lights them all up. 
 */
void setAllPixels(long color) {

  for (int i = 0; i < CircuitPlayground.strip.numPixels(); i++) {
    CircuitPlayground.setPixelColor(i, color);
  }
  delay(200);
}


/**
 * This method checks if the left button is pressed and
 * changes the mode.
 */
void updateMode() {
  if (CircuitPlayground.leftButton()) {
    mode = ++mode % numberOfModes;
    setAllPixels(OFF);
    Serial.print("Mode: ");
    Serial.println(mode);
  }
}
