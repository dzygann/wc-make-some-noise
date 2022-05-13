#include <Adafruit_CircuitPlayground.h>

#define SAMPLE_WINDOW   50  // Sample window for average level
#define INPUT_FLOOR     45  // Lower range of mic sensitivity in dB SPL
#define INPUT_CEILING   100  // Upper range of mic sensitivity in db SPL

const static long PIXEL_COLOR_GREEN = 0x00FF00;
const static long PIXEL_COLOR_YELLOW = 0x808000;
const static long PIXEL_COLOR_RED = 0xFF0000;

const static int OFF = 0;
const static int INPUT_DELTA = INPUT_CEILING - INPUT_FLOOR;

static int mode = 0;

int oldPosition = 0;


void setup() {
  Serial.begin(115200);
  CircuitPlayground.begin();

}

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
  } else if (mode == 2) {
    setPixelsWithThreeRounds(peakToPeak);
  } else {
    setPixelsWithThreeRoundsMoreFancy(peakToPeak);
  }

  updateMode();

}

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

void setPixelsWithThreeRounds(float peakToPeak) {

  int pixelStep = INPUT_DELTA / CircuitPlayground.strip.numPixels();

  if (peakToPeak < 60) {
    for (int i = 0; i < CircuitPlayground.strip.numPixels(); i++) {
      if ((pixelStep * i) + INPUT_FLOOR <= peakToPeak) {
        CircuitPlayground.setPixelColor(i, PIXEL_COLOR_GREEN);
      }
      delay(10);
    }
  }

  else if (peakToPeak < 85) {
    for (int i = 0; i < CircuitPlayground.strip.numPixels(); i++) {
      if ((pixelStep * i) + INPUT_FLOOR <= peakToPeak) {
        CircuitPlayground.setPixelColor(i, PIXEL_COLOR_YELLOW);
      }
      delay(10);
    }
  }

  else {
    for (int i = 0; i < CircuitPlayground.strip.numPixels(); i++) {
      if ((pixelStep * i) + INPUT_FLOOR <= peakToPeak) {
        CircuitPlayground.setPixelColor(i, PIXEL_COLOR_RED);
      }
      delay(10);
    }
  }
  for (int i = CircuitPlayground.strip.numPixels() - 1; i >= 0; i--) {
    if ((pixelStep * i) + INPUT_FLOOR > peakToPeak) {
      CircuitPlayground.setPixelColor(i, OFF);
    }
  }

}



void setPixelsWithThreeRoundsMoreFancy(float peakToPeak) {

  int maxPosition = 30;
  int pixelStep = 2;
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
        CircuitPlayground.setPixelColor(i % numPixels, PIXEL_COLOR_YELLOW);
        delay(100);
      } else if (i > 9 && i < 20 ) {
        CircuitPlayground.setPixelColor(i % numPixels, PIXEL_COLOR_GREEN);
        delay(100);
      } else if (i < 10) {
        CircuitPlayground.setPixelColor(i % numPixels, OFF);
        delay(100);
      }
    }
  }


  oldPosition = currentPosition;

}


void setAllPixels(long color) {

  for (int i = 0; i < CircuitPlayground.strip.numPixels(); i++) {
    CircuitPlayground.setPixelColor(i, color);
  }
  delay(200);
}

void updateMode() {
  if (CircuitPlayground.leftButton()) {
    mode = ++mode % 4;
    Serial.print("Mode: ");
    Serial.println(mode);
    CircuitPlayground.clearPixels();
  }
}
