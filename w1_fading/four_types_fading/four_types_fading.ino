/**************************************************************************
  Fading Lights (linear fade, sine fade, x squared fade, exponential fade)

  Used Tom Igoe's SimpleFade: https://github.com/tigoe/LightProjects/blob/main/FadeCurves/SimpleFade/SimpleFade.ino
  and SineFade: https://github.com/tigoe/LightProjects/blob/main/FadeCurves/SineFade/SineFade.ino
  and XSquaredFade: https://github.com/tigoe/LightProjects/blob/main/FadeCurves/XSquaredFade/XSquaredFade.ino
  and ExponentialFade: https://github.com/tigoe/LightProjects/blob/main/FadeCurves/ExponentialFade/ExponentialFade.ino
  
  created 30 Jan 2023
  modified 30 Jan 2023
  by I-Jon Hsieh
 **************************************************************************/

int linPin = 9;    // LED linear fade 
int sinPin = 10;    // LED sine fade
int xsqPin = 11;    // LED  x squared fade
int expPin = 12;    // LED  exponential fade
int btnPin = 2;

// analogWrite resolution (can be 10 for SAMD boards, has to be 8 for Uno):
const int resolution = 10;
// number of steps = 2^resolution:
const int steps = pow(2, resolution);
// change between steps:
int change = 1;
// current level:
int currentLevel = 1;
// pre-calculated PWM levels:
int linTable[steps];
int sinTable[steps];
int xsqTable[steps];
int expTable[steps];



void setup() {
  Serial.begin(9600);
  // wait for serial monitor to open:
  if (!Serial) delay(3000);

  fillLinTable();
  fillSinTable();
  fillXsqTable();
  fillExpTable();




  // set the analogWrite resolution:
  analogWriteResolution(resolution);
  // initialize digital pin 5 as an output:
  pinMode(linPin, OUTPUT);
  pinMode(sinPin, OUTPUT);
  pinMode(xsqPin, OUTPUT);
  pinMode(expPin, OUTPUT);
  pinMode(btnPin, INPUT_PULLUP);

}

void loop() {

  // decrease or increase by 1 point each time
  // if at the bottom or top, change the direction:
  if (currentLevel <= 0 || currentLevel >= steps - 1) {
    change = -change;
  }

  int btnState = digitalRead(btnPin);
  if(btnState == HIGH){
    currentLevel += change;
  }

  

  //PWM output the result:
  analogWrite(linPin, linTable[currentLevel]);
  analogWrite(sinPin, sinTable[currentLevel]);
  analogWrite(xsqPin, xsqTable[currentLevel]);
  analogWrite(expPin, expTable[currentLevel]);


  delay(5);
  Serial.print("linear: ");
  Serial.print(linTable[currentLevel]);
  Serial.print(", sine: ");
  Serial.print(sinTable[currentLevel]);
  Serial.print(", xSquared: ");
  Serial.print(xsqTable[currentLevel]);
  Serial.print(", exponential: ");
  Serial.println(expTable[currentLevel]);
  
}


// fill tables ============================================
void fillLinTable(){
  for (int l = 0; l < steps; l++) {
    int lightLevel = map(l, 0, steps, 0, 1023);
    linTable[l] = lightLevel;
  }
}

void fillSinTable() {
  // iterate over the array and calculate the right value for it:
  for (int l = 0; l < steps; l++) {
    // map input to a 0-360 range:
    int angle = map(l, 0, steps, 0, 180);
    // convert to radians:
    float lightLevel = angle * PI / 180;
    // now subtract PI/2 to offset by 90 degrees, so yuu can start fade at 0:
    lightLevel -= PI / 2;
    // get the sine of that:
    lightLevel = sin(lightLevel);
    // now you have -1 to 1. Add 1 to get 0 to 2:
    lightLevel += 1;
    // multiply to get 0-1023:
    lightLevel *= (steps) / 2;
    // put it in the array:
    sinTable[l] = int(lightLevel);
  }
}

void fillXsqTable() {
  // iterate over the array and calculate the right value for it:
  for (int l = 0; l <= steps; l++) {
    // square the current value:
    float lightLevel = pow(l, 2);
    // map the result back to the resolution range:
    lightLevel = map(lightLevel, 0, pow(steps, 2), 0, 1023);
    xsqTable[l] = lightLevel;
  }
}

void fillExpTable() {

  // Calculate the scaling factor based on the
  // number of PWM steps you want:
  float scalingFactor = (steps * log10(2)) / (log10(steps));

  // iterate over the array and calculate the right value for it:
  for (int l = 0; l < steps; l++) {
    int lightLevel = pow(2, (l / scalingFactor)) - 1;
    expTable[l] = lightLevel;
  }
}