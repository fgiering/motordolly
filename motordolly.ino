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

const char *menuscreens[numScreens][2][3] = {
    {{"Mode", ""}, {"Dolly", "Timelapse", "Setup"}},                 //root 0       //menustep 0
    {{"Direction", ""}, {"forwards", "backwards"}},                  //dolly 1      //menustep 10
    {{"Duration", "s"}, {"", ""}},                                   //dolly 2      //menustep 11
    {{"Distance", "cm"}, {"", ""}},                                  //dolly 3      //menustep 12
    {{"Want to start?", ""}, {"", ""}},                              //dolly 4      //menustep 13
    {{"Shots", "shots"}, {"", ""}},                                  //timelapse 5  //menustep 30
    {{"Interval", "s"}, {"", ""}},                                   //timelapse 6  //menustep 31
    {{"Distance", "cm"}, {"", ""}},                                  //timelapse 7  //menustep 32
    {{"Want to start?", ""}, {"", ""}},                              //timelapse 8  //menustep 33
    {{"Sound", ""}, {"On", "Off"}},                                  //setup 9      //menustep 50
    {{"LED", ""}, {"On", "Off"}},                                    //setup 10     //menustep 51
    {{"Ease In/Out", ""}, {"On", "Off"}},                            //setup 11     //menustep 52
    {{"Repeat Movement", ""}, {"No", "Back & Again", "Go straight"}} //repeat 12    //menustep 70
};
unsigned int parameters[numScreens] = {0, 0, 3, 5, 0, 3, 15, 5, 0, 0, 0, 0, 0};

boolean studioMode = true;
byte waitTimeStudio = 5; //in seconds

//-------Input-------//
byte keyvalue = 0;
boolean getnumber = false;
byte numArray[4] = {0, 0, 0, 1}; //for 4-digit NumberInputs
boolean cancel = false;
byte activeDigit = 0;

//------Motor------//
AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
const long stepsPerCm = 400; //gear ratio of motor to wheel
const int fullSpeed = 1000;  //maxSpeed = 2cm/s
byte movedirection = 0;      //0=forwards, 1=backwards;

//-----IR-Remote-----//
const byte IRmodul = 6; //pin of IR module
IRrecv irrec(IRmodul);
decode_results recieved;
boolean getNewInput = true;

//------Display------//
LiquidCrystal_I2C lcd(0x27, 16, 2);
boolean backlightState = true;

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
  keyvalue = recieveIR(200);                     //recieve Input from IR
  if (keyvalue != 0 && keyvalue != 1 && !cancel) // do not if canceled
  {
    inputAction(keyvalue); //do something
    printScreen();         //print something
  }
  inputAction(0); //back to startscreen
}

void printScreen()
{ //Function to print text from the menuscreens Array on the Screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuscreens[lookUp(menustep)][0][0]);
  lcd.setCursor(0, 1);
  lcd.print(F("> "));
  if (getnumber) // if number input active show number input
  {
    if (activeDigit < 4)
    {
      if (keyvalue < 22 && keyvalue > 12 || keyvalue == 10) // check if keyvalue is a number
      {
        if (keyvalue == 10) // check if keyvalue is zero
        {
          numArray[activeDigit] = 0;
        }
        else
        {
          numArray[activeDigit] = keyvalue - 12; //save value in array
        }
        activeDigit++;
        parameters[lookUp(menustep)] = numArray[0] * 1000 + numArray[1] * 100 + numArray[2] * 10 + numArray[3];
      }
      if (parameters[lookUp(menustep)] < 1000)
      {
        lcd.print(F("0"));
      }
      if (parameters[lookUp(menustep)] < 100)
      {
        lcd.print(F("0"));
      }
      if (parameters[lookUp(menustep)] < 10)
      {
        lcd.print(F("0"));
      }
      lcd.print(parameters[lookUp(menustep)]); // print last number
      lcd.print(F(" "));
      lcd.print(menuscreens[lookUp(menustep)][0][1]); //print unit
    }
    if (activeDigit >= 4) // if last Digit reached, jump to first
    {
      activeDigit = 0;
    }
  }
  else
  {
    if (menustep == 13)
    { //screen before start dolly
      if (parameters[1] == 0)
      {
        //TODO: make a beautiful char for forwards
        lcd.print(F("f"));
      }
      if (parameters[1] == 1)
      {
        //TODO: make a beautiful char for backwards
        lcd.print(F("b"));
      }
      lcd.print(F("  "));
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
  cancel = true;
  stepper.stop();
  delay(50);
  changeMenuStep(0);
  ledOff();
  printScreen();
}

void piep(int times, int waitTime) //function to piep the speaker with Parameters 1-how often 2-how long to wait between in ms
{
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

void changeMenuStep(byte newStep) //function to change the MenuStep in the menu
{
  menustep = newStep;
  getnumber = false;
  activeDigit = 0;
  lcd.noBlink();
  piep(1, 0);
  switch (menustep)
  {
  case 11:
    numberInput();
    break;
  case 12:
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

void changeParameter(int values) //function to change a Parameter in the Parameter Array with Paramter 1-how many values do i have
{
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
{ //Function to input 4-digit numbers with remote and display it on screen
  getnumber = true;

  //Write Parameters of Menustep to numArray to show last Values
  numArray[3] = (parameters[lookUp(menustep)] % 10);
  numArray[2] = ((parameters[lookUp(menustep)] / 10) % 10);
  numArray[1] = ((parameters[lookUp(menustep)] / 100) % 10);
  numArray[0] = (parameters[lookUp(menustep)] / 1000);

  lcd.setCursor(activeDigit + 2, 1);
  lcd.blink();
}

byte lookUp(byte menustepInMenu)
{
  //convert Menustep in Array-Position
  if (menustepInMenu == 0)
  {
    return 0;
  } //root 0       //menustep 0
  if (menustepInMenu == 10)
  {
    return 1;
  } //dolly 1      //menustep 10
  if (menustepInMenu == 11)
  {
    return 2;
  } //dolly 2      //menustep 11
  if (menustepInMenu == 12)
  {
    return 3;
  } //dolly 3      //menustep 12
  if (menustepInMenu == 13)
  {
    return 4;
  } //dolly 4      //menustep 13
  if (menustepInMenu == 30)
  {
    return 5;
  } //timelapse 5  //menustep 30
  if (menustepInMenu == 31)
  {
    return 6;
  } //timelapse 6  //menustep 31
  if (menustepInMenu == 32)
  {
    return 7;
  } //timelapse 7  //menustep 32
  if (menustepInMenu == 33)
  {
    return 8;
  } //timelapse 8  //menustep 33
  if (menustepInMenu == 50)
  {
    return 9;
  } //setup 9      //menustep 50
  if (menustepInMenu == 51)
  {
    return 10;
  } //setup 10     //menustep 51
  if (menustepInMenu == 52)
  {
    return 11;
  } //setup 11     //menustep 52
  if (menustepInMenu == 70)
  {
    return 12;
  } //repeat 12    //menustep 70

  // TODO: LookUp for Names
  // Convert Name in menuStep
  //  if (menustepInMenu == "root" ) {  return "0";}   //root 0       //menustep 0
  //  if (menustepInMenu == "dollyDirection") {  return "10" ;}   //dolly 1      //menustep 10
  //  if (menustepInMenu == "dollyDuration") {  return "11" ;}   //dolly 2      //menustep 11
  //  if (menustepInMenu == "dollyDistance") {  return "12" ;}   //dolly 3      //menustep 12
  //  if (menustepInMenu == "dollyStart") {  return "13" ;}   //dolly 4      //menustep 13
  //  if (menustepInMenu == "timelapseShots") {  return "30" ;}   //timelapse 5  //menustep 30
  //  if (menustepInMenu == "timelapseInterval") {  return "31" ;}   //timelapse 6  //menustep 31
  //  if (menustepInMenu == "timelapseDistance") {  return "32" ;}   //timelapse 7  //menustep 32
  //  if (menustepInMenu == "timelapseStart") {  return "33" ;}   //timelapse 8  //menustep 33
  //  if (menustepInMenu == "setupSound") {  return "50" ;}   //setup 9      //menustep 50
  //  if (menustepInMenu == "setupLED") {  return "51";}   //setup 10     //menustep 51
  //  if (menustepInMenu == "setupEase") {  return "52";}   //setup 11     //menustep 52
  //  if (menustepInMenu == "repeatMovement") { return "70"; } //repeat 12    //menustep 70

  else
    return 0;
}

void switchBacklight()
{
  if (backlightState == true)
  {
    lcd.noBacklight();
    backlightState = false;
  }
  else
  {
    lcd.backlight();
    backlightState = true;
  }
}
