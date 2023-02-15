/**************************************************************************
  12V Lamp with M16 and corn bulb

  Used Tom Igoe's ExponentialFade: https://github.com/tigoe/LightProjects/blob/main/FadeCurves/ExponentialFade/ExponentialFade.ino
  and Arduino's Debounce on a Pushbutton: https://docs.arduino.cc/built-in-examples/digital/Debounce

  created 13 Feb 2023
  modified 14 Feb 2023
  by I-Jon Hsieh
 **************************************************************************/

int lightPin = 5;
int btnPin = 3;

int btnState;            // the current reading from the input pin
int lastBtnState = LOW;  // the previous reading from the input pin
int mode = 0;            // there modes (off, dimmer, auto fading);
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


// analogWrite resolution (can be 10 for SAMD boards, has to be 8 for Uno):
const int resolution = 10;
const int steps = pow(2, resolution); // number of steps = 2^resolution:
int change = 1;                       // change between steps:
int currentLevel = 1;
int levelTable[steps];                // pre-calculated PWM levels:



void setup() {
  Serial.begin(9600);
  if (!Serial) delay(3000);

  pinMode(btnPin, INPUT_PULLUP);
  pinMode(lightPin, OUTPUT);

  analogWriteResolution(resolution);

  // pre-calculate the PWM levels for auto fading (exponential fading):
  fillLevelTable();
}

void loop() {

  //MODE BUTTON --------------------------------------
  int reading = digitalRead(btnPin);

  if (reading != lastBtnState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != btnState) {
      btnState = reading;

      if (btnState == LOW) {
        mode ++;
        if (mode > 2){
          mode = 0;
        }
      }
    }
  }
  lastBtnState = reading;

  //CHANGE MODE --------------------------------------
  if (mode == 0) {
    analogWrite(lightPin, 0);
    Serial.println("off");
  } else if (mode == 1) {
    setDimmer();
  } else {
    setAutoFading();
  }

}

void setDimmer() {
  int potReading = analogRead(A1);

  analogWrite(lightPin, potReading);
  Serial.print("MODE: Dimmer, VALUE: ");
  Serial.println(potReading);
}

void setAutoFading() {
  // decrease or increase by 1 point each time
  if (currentLevel <= 0 || currentLevel >= steps - 1) {
    change = -change;
  }
  currentLevel += change;

  //PWM output the result:
  analogWrite(lightPin, levelTable[currentLevel]);
  delay(5);
  Serial.print("MODE: Auto Fading, VALUE: ");
  Serial.println(levelTable[currentLevel]);

}

void fillLevelTable() {
  float scalingFactor = (steps * log10(2)) / (log10(steps));

  // iterate over the array and calculate the right value for it:
  for (int l = 0; l < steps; l++) {
    int lightLevel = pow(2, (l / scalingFactor)) - 1;
    levelTable[l] = lightLevel;
  }
}