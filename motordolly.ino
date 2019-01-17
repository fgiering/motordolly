//-----Libraries-----//
#include <NewTone.h>
#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>

//-----Sound pin-----//
#define TONE_PIN 7
//-----Motor pins-----//
#define HALFSTEP 8
#define motorPin1  2
#define motorPin2  3
#define motorPin3  4
#define motorPin4  5

//-----LED Pins-----//
const int LEDr = 9;
const int LEDg = 10;
const int LEDb = 6;
int brightness = 100;

//-------Menu-------//
int nextmenustep = 0;
int menustep = 0; // 1= dolly{1=direction, 2=duration, 3=range}, 4=timelapse{4=shots, 5=interval, 6=distance}, 5= setup etc.
const int numScreens = 12;
String menuscreens[numScreens][2][3] = {
  {{"Mode", ""}, {"Dolly", "Timelapse", "Setup"}}, //root 0
  {{"Direction", ""}, {"forwards", "backwards"}}, //dolly 1
  {{"Duration", "s"}, {"0", "0"}},            //dolly 2
  {{"Distance", "cm"}, {"0", "0"}},           //dolly 3
  {{"Shots", "shots"}, {"0", "0"}},           //timelapse 4
  {{"Interval", "s"}, {"0", "0"}},            //timelapse 5
  {{"Distance", "cm"}, {"0", "0"}},           //timelapse 6
  {{"Want to start?", ""}, {"0", "0"}},       //timelapse 7
  {{"Sound", ""}, {"On", "Off"}},           //setup 8
  {{"LED", ""}, {"On", "Off"}},             //setup 9
  {{"Ease In/Out", ""}, {"On", "Off"}},     //setup 10
  {{"Repeat Movement", ""}, {"Yes", "No"}} //repeat 11
};
int parameters[numScreens]; //Array for saving parameter values

//-------Input-------//
int keyvalue = 0;
boolean getnumber = false;
int numArray [4] = {0, 0, 0, 1}; //for 4-digit NumberInputs
boolean cancel = false;
int activeDigit = 0;

//------Motor------//
AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
const int stepsPerCm = 450;
const int fullSpeed = 900.0;
int movedirection = 0; //0=forwards, 1=backwards;
//const int movespeed = fullSpeed; //rpm (rounds per minute) (max = 15 /9 at Stepper.h)
//const int movesteps = 0; //64 = 1 Umdrehung

//-----IR-Remote-----//
int IRmodul = 8;
IRrecv irrec(IRmodul);
decode_results recieved;
boolean getNewInput = true;

//------Display------//
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  irrec.enableIRIn();

  //-----LEDs Setup-----//
  pinMode(LEDr, OUTPUT);
  pinMode(LEDg, OUTPUT);
  pinMode(LEDb, OUTPUT);

  //-----Motor Setup-----//
  stepper.setAcceleration(80.0);

  //-----LCD Setup-----//
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < numScreens; i++) { //Reset all parameters to 0000
    parameters[i] = 0000;
  }

  printScreen(); //Show first menustep on Startup
}

void loop() {       //Main Function
  cancel = false;
  if (getNewInput) {
  keyvalue = recieveIR();
  }
  if (keyvalue != 0 && keyvalue != 1) {
    inputAction(keyvalue);
    printScreen();
  }
  getNewInput = true;
}


void printScreen() { //Function to print text from the menuscreens Array on the Screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuscreens[menustep][0][0]);
  lcd.setCursor(0, 1);
  lcd.print("> ");
  String activeParameter = menuscreens[menustep][1][parameters[menustep]];
  if (!getnumber) {
    if (activeParameter == "0") { //screen with 4digit parameter + unit
      lcd.print(parameters[menustep]);
      lcd.print(" ");
      lcd.print(menuscreens[menustep][0][1]);
    }
    else if (menustep == 7) { //screen before start
      lcd.print(parameters[4]);
      lcd.print("p ");
      lcd.print(parameters[5]);
      lcd.print("s ");
      lcd.print(parameters[6]);
      lcd.print("cm");
    }
    else { //screen with fixed values
      lcd.print(activeParameter);
    }
  } else if (getnumber) {
    lcd.blink();
    if (activeDigit < 4) {
      if (keyvalue < 22 && keyvalue > 12 || keyvalue == 10) {
        if (keyvalue == 10) {
          numArray[activeDigit] = 0;
        } else {
          numArray[activeDigit] = keyvalue - 12;
        }
        for (int i; i < 4; i++) {
          lcd.print(numArray[i]);
        }
        lcd.print(" ");
        lcd.print(menuscreens[menustep][0][1]); //unit
        activeDigit++;
        lcd.setCursor(activeDigit + 2, 1);
      }
    }
    if (activeDigit == 4) {
      parameters[menustep] = numArray[0] * 1000 + numArray[1] * 100 + numArray[2] * 10 + numArray[3];
      activeDigit = 0;
    }
    lcd.noBlink();
  }
}


void reset() { //Function to Reset all e.g. after canceling
  stepper.stop();
  delay(300);
  changeMenuStep(0);
}

void piep(int times, int waitTime) { //function to piep the speaker with Parameters 1-how often 2-how long to wait between in ms
  unsigned long preMillis = 0;
  int pieped = 0;
  if (parameters[7] == 0) {
    do {
      if (millis() - preMillis >= waitTime) {
        NewTone(TONE_PIN, 125);
        delay(300);
        noNewTone(TONE_PIN);
        pieped++;
        preMillis = millis();
      }
      recieveIR();
    } while (!cancel && pieped < times);
  }
}

void changeMenuStep(int newStep) { //function to change the MenuStep in the menu
  menustep = newStep;
  for (int i = 0; i < 4; ++i) {//or reset numArray
    numArray[i] = 0;
  }
  piep(1, 0);
}

void changeParameter(int values) { //function to change a Parameter in the Parameter Array with Paramter 1-how many values do i have
  if (keyvalue == 9 && parameters[menustep] < values) {
    if (parameters[menustep] == values - 1) {
      parameters[menustep] = 0;
    } else {
      parameters[menustep]++;
    }
  } else if (keyvalue == 7 && parameters[menustep] >= 0) {
    if (parameters[menustep] == 0) {
      parameters[menustep] = values - 1;
    } else {
      parameters[menustep]--;
    }
  }
}

void numberInput () { //Function to input 4-digit numbers with remote an display it on Screen
  getnumber = true;
  lcd.blink();
}
