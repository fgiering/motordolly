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
#define motorPin1 2
#define motorPin2 3
#define motorPin3 4
#define motorPin4 5

//-----LED Pins-----//
const byte LEDr = 8;
const byte LEDg = 9;
const byte LEDb = 10;
byte brightness = 100;

//-------Menu-------//
byte nextmenustep = 0;
byte menustep = 0; // 10= dolly{10=direction, 11=duration, 12=range}, 30=timelapse{31=shots, 32=interval, 33=distance}, 50= setup, 70=repeat
const byte numScreens = 13;
const String menuscreens[numScreens][2][3] = {
    {{"Mode", ""}, {"Dolly", "Timelapse", "Setup"}}, //root 0       //menustep 0
    {{"Direction", ""}, {"forwards", "backwards"}},  //dolly 1      //menustep 10
    {{"Duration", "s"}, {"", ""}},                   //dolly 2      //menustep 11
    {{"Distance", "cm"}, {"", ""}},                  //dolly 3      //menustep 12
    {{"Want to start?", ""}, {"", ""}},              //dolly 4      //menustep 13
    {{"Shots", "shots"}, {"", ""}},                  //timelapse 5  //menustep 30
    {{"Interval", "s"}, {"", ""}},                   //timelapse 6  //menustep 31
    {{"Distance", "cm"}, {"", ""}},                  //timelapse 7  //menustep 32
    {{"Want to start?", ""}, {"", ""}},              //timelapse 8  //menustep 33
    {{"Sound", ""}, {"On", "Off"}},                  //setup 9      //menustep 50
    {{"LED", ""}, {"On", "Off"}},                    //setup 10     //menustep 51
    {{"Ease In/Out", ""}, {"On", "Off"}},            //setup 11     //menustep 52
    {{"Repeat Movement", ""}, {"Yes", "No"}}         //repeat 12    //menustep 70
};
unsigned int parameters[numScreens] = {0, 0, 3, 5, 0, 3, 15, 5, 0, 0, 0, 0, 0};

//-------Input-------//
byte keyvalue = 0;
boolean getnumber = false;
byte numArray[4] = {0, 0, 0, 1}; //for 4-digit NumberInputs
boolean cancel = false;
byte activeDigit = 0;

//------Motor------//
AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
const long stepsPerCm = 400; //dislocation of motor to wheel
const int fullSpeed = 1000;  //maxSpeed = 2cm/s
byte movedirection = 0;      //0=forwards, 1=backwards;

//-----IR-Remote-----//
byte IRmodul = 6;
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
  stepper.setAcceleration(100.0);

  //-----LCD Setup-----//
  lcd.init();
  lcd.backlight();

  printScreen(); //Show first menustep on Startup
}

void loop()
{ //Main Function
  cancel = false;
  keyvalue = recieveIR(200);
  if (keyvalue != 0 && keyvalue != 1 && !cancel)
  {
    inputAction(keyvalue);
    printScreen();
  }
  inputAction(0);
}

void printScreen()
{ //Function to print text from the menuscreens Array on the Screen
  //String activeParam = menuscreens[lookUp(menustep)][1][parameters[lookUp(menustep)]];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuscreens[lookUp(menustep)][0][0]);
  lcd.setCursor(0, 1);
  lcd.print(F("> "));
  if (getnumber)
  { // if number input active
    if (activeDigit < 4)
    {
      if (keyvalue < 22 && keyvalue > 12 || keyvalue == 10)
      {
        if (keyvalue == 10)
        {
          numArray[activeDigit] = 0;
        }
        else
        {
          numArray[activeDigit] = keyvalue - 12;
        }
        activeDigit++;
      }
    }
    if (activeDigit >= 4)
    {
      activeDigit = 0;
    }
    if (keyvalue != 5)
    {
      parameters[lookUp(menustep)] = numArray[0] * 1000 + numArray[1] * 100 + numArray[2] * 10 + numArray[3];
    }
    if (parameters[lookUp(menustep)] < 1000)
      lcd.print(F("0"));
    if (parameters[lookUp(menustep)] < 100)
      lcd.print(F("0"));
    if (parameters[lookUp(menustep)] < 10)
      lcd.print(F("0"));
    lcd.print(parameters[lookUp(menustep)]);

    lcd.print(F(" "));
    lcd.print(menuscreens[lookUp(menustep)][0][1]); //unit
  }
  else
  {
     if (menustep == 13)
    { //screen before start dolly
      lcd.print(parameters[1]);
      lcd.print(F("p "));
      lcd.print(parameters[2]);
      lcd.print(F("s "));
      lcd.print(parameters[3]);
      lcd.print(F("cm"));
    }
    if (menustep == 33)
    { //screen before start timelapse
      lcd.print(parameters[5]);
      lcd.print(F("p "));
      lcd.print(parameters[6]);
      lcd.print(F("s "));
      lcd.print(parameters[7]);
      lcd.print(F("cm"));
    }
     
    else
    { //screen with fixed values
      lcd.print(menuscreens[lookUp(menustep)][1][parameters[lookUp(menustep)]]);
    }
  }
}

void reset()
{ //Function to Reset all e.g. after canceling
  stepper.stop();
  delay(50);
  changeMenuStep(0);
  ledOff();
  printScreen();
}

void piep(int times, int waitTime)
{ //function to piep the speaker with Parameters 1-how often 2-how long to wait between in ms
  unsigned long preMillis = 0;
  int pieped = 0;
  if (parameters[9] == 0)
  {
    while (pieped < times)
    {
      if (millis() >= preMillis + waitTime)
      {
        NewTone(TONE_PIN, 2025); //pin, freq
        delay(300);
        noNewTone(TONE_PIN);
        pieped++;
        preMillis = millis();
      }
    }
  }
}

void changeMenuStep(byte newStep)
{ //function to change the MenuStep in the menu
  menustep = newStep;
  getnumber = false;
  activeDigit = 0;
  lcd.noBlink();
  piep(1, 0);
  for (byte i = 0; i < 4; ++i)
  { //reset numArray
    // TODO: Is this Reset to late?
    numArray[i] = 0;
  }
  switch (menustep)
  {
  case 12:
    numberInput();
    break;
  case 13:
    numberInput();
    break;
  case 30:
    numberInput();
    break;
  case 31:
    numberInput();
    break;
  case 32:
    numberInput();
    break;
  }
}

void changeParameter(int values)
{ //function to change a Parameter in the Parameter Array with Paramter 1-how many values do i have
  if (keyvalue == 9 && parameters[lookUp(menustep)] < values)
  {
    if (parameters[lookUp(menustep)] == values - 1)
    {
      parameters[lookUp(menustep)] = 0;
    }
    else
    {
      parameters[lookUp(menustep)]++;
    }
  }
  else if (keyvalue == 7 && parameters[lookUp(menustep)] >= 0)
  {
    if (parameters[lookUp(menustep)] == 0)
    {
      parameters[lookUp(menustep)] = values - 1;
    }
    else
    {
      parameters[lookUp(menustep)]--;
    }
  }
}

void numberInput()
{ //Function to input 4-digit numbers with remote an display it on Screen
  getnumber = true;
  lcd.setCursor(activeDigit + 2, 1);
  lcd.blink();
}


void lookUp(byte menustepInMenu)
{
  if (menustepInMenu == 0 ) {  return 0 }   //root 0       //menustep 0
  if (menustepInMenu == 10) {  return 1 }   //dolly 1      //menustep 10
  if (menustepInMenu == 11) {  return 2 }   //dolly 2      //menustep 11
  if (menustepInMenu == 12) {  return 3 }   //dolly 3      //menustep 12
  if (menustepInMenu == 13) {  return 4 }   //dolly 4      //menustep 13
  if (menustepInMenu == 30) {  return 5 }   //timelapse 5  //menustep 30
  if (menustepInMenu == 31) {  return 6 }   //timelapse 6  //menustep 31
  if (menustepInMenu == 32) {  return 7 }   //timelapse 7  //menustep 32
  if (menustepInMenu == 33) {  return 8 }   //timelapse 8  //menustep 33
  if (menustepInMenu == 50) {  return 9 }   //setup 9      //menustep 50
  if (menustepInMenu == 51) {  return 10}   //setup 10     //menustep 51
  if (menustepInMenu == 52) {  return 11}   //setup 11     //menustep 52
  if (menustepInMenu == 70) {  return 12}   //repeat 12    //menustep 70
  else
    return 0;
}