/**************************************************************************
  Candle
  Uses Adafruit_NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel
  Arduino LSM6DS3 library example (by Riccardo Rizzo): https://www.arduino.cc/en/Reference/ArduinoLSM6DS3
  Tom Igoe's WS281xCandle: https://github.com/tigoe/LightProjects/tree/main/Candles/WS281xCandle
  created 02 Feb 2023
  modified 04 Feb 2023
  by I-Jon Hsieh
 **************************************************************************/

#include <Adafruit_NeoPixel.h>
#include <Arduino_LSM6DS3.h>

const int neoPixelPin = 4;  // control pin
const int pixelCount = 7;    // number of pixels

// set up strip:
Adafruit_NeoPixel candle = Adafruit_NeoPixel(pixelCount, neoPixelPin, NEO_GRBW + NEO_KHZ800);

// arrays for each pixel's hue, sat, and intensity:
unsigned int hue[pixelCount];
int sat[pixelCount];
int intensity[pixelCount];

void setup() {
  // initialize the random number generator using a reading
  // from pin A6 (not connected to anything, so it will generate
  // a random number):
  randomSeed(analogRead(A6));
  
  // loop over the pixel arrays:
  for (int p = 0; p < pixelCount; p++) {
    // generate a random initial value for each pixel's
    // hue, saturation and intensity:
    hue[p] = random(2000, 5000);      // red-orange to mid-orange
    sat[p] = random(192, 255);        // high end of saturation
    intensity[p] = random(127, 192);  // mid-high range of intensity
  }
  
  candle.begin();    // initialize pixel strip
  candle.clear();    // turn all LEDs off
  candle.show();     // update strip

  if (!IMU.begin()) {
    Serial.println("IMU failed");
    while (1);
  }

}

void loop() {
  float gx, gy, gz; //gyroscope data
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gx, gy, gz);
  }

  float absGx = abs(gx);
  float absGy = abs(gy);
  float absGz = abs(gz);

  bool moving = false;
  if (absGx + absGy + absGz > 10){
    moving = true;
  }



  // loop over the pixels:
  for (int p = 0; p < pixelCount; p++) {
    // change hue -1 to 2 points:
    int hueChange = random(-1,2);
    hue[p] += hueChange;
    // constrain to red to orange: (800-8000)
    hue[p] = constrain(hue[p], 5000, 8000);

    // change saturation -1 to 1 points:
    int satChange = random(-1,1);
    sat[p] += satChange;
    // constrain to the top end of the range:
    sat[p] = constrain(sat[p], 192, 255);

    // change intensity -2 to 3 points:

    int intensityChange;

    if(moving){
      intensityChange = random(-18,19);
    } else if(millis()%random(300)<random(10)){
      intensityChange = random(-10,11);
    } else{
      intensityChange = random(-2,3);
    }

    intensity[p] += intensityChange;
    // constrain to 20-255
    intensity[p] = constrain(intensity[p], 20, 255);

    // get RGB from HSV:
    unsigned long color = candle.ColorHSV(hue[p], sat[p], intensity[p]);
    // do a gamma correction:
    unsigned long correctedColor = candle.gamma32(color);
    candle.setPixelColor(p, correctedColor);
  }
  candle.show();
  delay(5);
}