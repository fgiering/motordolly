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
const String menuscreens[numScreens][2][3] = {
  { {"Mode", ""},            {"Dolly", "Timelapse", "Setup"}   }, //root 0
  { {"Direction", ""},       {"forwards", "backwards"}         }, //dolly 1
  { {"Duration", "s"},       {"", ""}                          }, //dolly 2
  { {"Distance", "cm"},      {"", ""}                          }, //dolly 3
  { {"Shots", "shots"},      {"", ""}                          }, //timelapse 4
  { {"Interval", "s"},       {"", ""}                          }, //timelapse 5
  { {"Distance", "cm"},      {"", ""}                          }, //timelapse 6
  { {"Want to start?", ""},  {"", ""}                          }, //timelapse 7
  { {"Sound", ""},           {"On", "Off"}                     }, //setup 8
  { {"LED", ""},             {"On", "Off"}                     }, //setup 9
  { {"Ease In/Out", ""},     {"On", "Off"}                     }, //setup 10
  { {"Repeat Movement", ""}, {"Yes", "No"}                     }  //repeat 11
};
unsigned int parameters[numScreens] = {0, 0, 3, 1, 3, 15, 1, 0, 1, 0, 0, 0};

//-------Input-------//
int keyvalue = 0;
boolean getnumber = false;
int numArray [4] = {0, 0, 0, 1}; //for 4-digit NumberInputs
boolean cancel = false;
int activeDigit = 0;

//------Motor------//
AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
const long stepsPerCm = 450; //dislocation of motor to wheel
const int fullSpeed = 1000.0; //maxSpeed = 2cm/s
int movedirection = 0; //0=forwards, 1=backwards;

//-----IR-Remote-----//
int IRmodul = 8;
IRrecv irrec(IRmodul);
decode_results recieved;
boolean getNewInput = true;

//------Display------//
LiquidCrystal_I2C lcd(0x27, 16, 2);


int temp;


void setup()
{
  Serial.begin(9600);
  irrec.enableIRIn();

  //-----LEDs Setup-----//
  pinMode(LEDr, OUTPUT);
  pinMode(LEDg, OUTPUT);
  pinMode(LEDb, OUTPUT);

  //-----Motor Setup-----//
  stepper.setAcceleration(100.0);

  //-----LCD Setup-----//
  lcd.init();
  lcd.backlight();

  printScreen(); //Show first menustep on Startup
}

void loop() {       //Main Function
  cancel = false;
  keyvalue = recieveIR(200);
  if (keyvalue != 0 && keyvalue != 1 && !cancel) {
    inputAction(keyvalue);
    //Serial.println(parameters[menustep]);
    printScreen();
  }
  inputAction(0);

  /*Serial.print("I ran ");
    Serial.println(temp);
    temp++;*/
}

void printScreen() {  //Function to print text from the menuscreens Array on the Screen
  //String activeParam = menuscreens[menustep][1][parameters[menustep]];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuscreens[menustep][0][0]);
  lcd.setCursor(0, 1);
  lcd.print(F("> "));
  if (getnumber) { // if number input active
    if (activeDigit < 4) {
      if (keyvalue < 22 && keyvalue > 12 || keyvalue == 10) {
        if (keyvalue == 10) {
          numArray[activeDigit] = 0;
        } else {
          numArray[activeDigit] = keyvalue - 12;
        }
        activeDigit++;
      }
    }
    if (activeDigit >= 4) {
      activeDigit = 0;
    }
    if (keyvalue != 5) {
      parameters[menustep] = numArray[0] * 1000 + numArray[1] * 100 + numArray[2] * 10 + numArray[3];  
    }
    if (parameters[menustep] < 1000)
      lcd.print(F("0"));
    if (parameters[menustep] < 100)
      lcd.print(F("0"));
    if (parameters[menustep] < 10)
      lcd.print(F("0"));
    lcd.print(parameters[menustep]);
    
    /*for (int i; i < 4; i++) {
      lcd.print(numArray[i]);
    }*/
    lcd.print(F(" "));
    lcd.print(menuscreens[menustep][0][1]); //unit
  } else {
    if (menustep == 7) { //screen before start timelapse
      lcd.print(parameters[4]);
      lcd.print(F("p "));
      lcd.print(parameters[5]);
      lcd.print(F("s "));
      lcd.print(parameters[6]);
      lcd.print(F("cm"));
    }
    /*else if (menuscreens[menustep][1][parameters[menustep]] == "" || menuscreens[menustep][1][parameters[menustep]] == NULL) { //screen with 4digit parameter + unit
      if (parameters[menustep] < 1000)
        lcd.print(F("0"));
      if (parameters[menustep] < 100)
        lcd.print(F("0"));
      if (parameters[menustep] < 10)
        lcd.print(F("0"));
      lcd.print(parameters[menustep]);
      lcd.print(F(" "));
      lcd.print(menuscreens[menustep][0][1]); //unit
    }*/
    else { //screen with fixed values
      //Serial.println(parameters[menustep]);
      lcd.print(menuscreens[menustep][1][parameters[menustep]]);
    }
  }
}

void reset() { //Function to Reset all e.g. after canceling
  stepper.stop();
  delay(50);
  changeMenuStep(0);
  ledOff();
  printScreen();
}

void piep(int times, int waitTime) { //function to piep the speaker with Parameters 1-how often 2-how long to wait between in ms
  unsigned long preMillis = 0;
  int pieped = 0;
  if (parameters[8] == 0) {
    while (pieped < times) {
      if (millis() >= preMillis + waitTime) {
        NewTone(TONE_PIN, 125);
        delay(300);
        noNewTone(TONE_PIN);
        pieped++;
        preMillis = millis();
      }
    }
  }
}

void changeMenuStep(int newStep) { //function to change the MenuStep in the menu
  menustep = newStep;
  getnumber = false;
  activeDigit = 0;
  lcd.noBlink();
  piep(1, 0);
  for (int i = 0; i < 4; ++i) {//reset numArray
    numArray[i] = 0;
  }
  switch (menustep) {
    case 2:
      numberInput();
      break;
    case 3:
      numberInput();
      break;
    case 4:
      numberInput();
      break;
    case 5:
      numberInput();
      break;
    case 6:
      numberInput();
      break;
  }
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
  lcd.setCursor(activeDigit + 2, 1);
  lcd.blink();
}
