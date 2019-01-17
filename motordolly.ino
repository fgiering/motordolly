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
int keyvalue = 0;
int nextmenustep = 0;
int menustep = 0; // 1= dolly{1=direction, 2=duration, 3=range}, 4=timelapse{4=shots, 5=interval, 6=distance}
boolean cancel = false;
int numArray [4] = {0, 0, 0, 1}; //for 4-digit NumberInputs
const int numScreens = 11;
String menuscreens[numScreens][2][3] = {
  {{"Mode", ""}, {"Dolly", "Timelapse", "Setup"}},
  {{"Direction", ""}, {"forwards", "backwards"}}, //dolly 1
  {{"Duration", "s"}, {"", ""}},         //dolly 2
  {{"Distance", "cm"}, {"", ""}},          //dolly 3
  {{"Shots", "shots"}, {"", ""}},         //timelapse 4
  {{"Interval", "ms"}, {"", ""}},         //timelapse 5
  {{"Distance", "cm"}, {"", ""}},           //timelapse 6
  {{"Sound", ""}, {"On", "Off"}},    //setup 7
  {{"LED", ""}, {"On", "Off"}},       //setup 8
  {{"Ease In/Out", ""}, {"On", "Off"}}, //setup 9
  {{"Repeat Movement", ""}, {"Yes", "No"}}, //repeat 10
};
int parameters[numScreens];

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

  for (int i = 0; i < numScreens; i++) {
    parameters[i] = 0;
  }

  printScreen();
}

void loop() {       //Main Function
  if (getNewInput) {
    keyvalue = recieveIR();
  }
  if (keyvalue != 0) {
    inputAction(keyvalue);
    printScreen();
  }
  getNewInput = true;
}

int recieveIR() {     //Function to recieve IR-Codes from the Remote
  if (irrec.decode(&recieved)) {
    Serial.println(recieved.value, DEC);
    getNewInput = false;
    irrec.resume();
  }
  switch (recieved.value) {
    case 16753245: //key on Remote: power
      cancel = true;
      stepper.stop();
      reset();
      return (1);
      break;
    case 16736925: //key on Remote: vol+
      return (2);
      break;
    case 16769565: //key on Remote: func/stop
      return (3);
      break;
    case 16720605: //key on Remote: <<
      return (4);
      break;
    case 16712445: //key on Remote: >
      return (5);
      break;
    case 16761405: //key on Remote: >>
      return (6);
      break;
    case 16769055: //key on Remote: down
      return (7);
      break;
    case 16754775: //key on Remote: vol-
      return (8);
      break;
    case 16748655: //key on Remote: up
      return (9);
      break;
    case 16738455: //key on Remote: 0
      return (10);
      break;
    case 16750695: //key on Remote: EQ
      return (11);
      break;
    case 16756815: //key on Remote: ST/REPT
      return (12);
      break;
    case 16724175: //key on Remote: 1
      return (13);
      break;
    case 16718055: //key on Remote: 2
      return (14);
      break;
    case 16743045: //key on Remote: 3
      return (15);
      break;
    case 16716015: //key on Remote: 4
      return (16);
      break;
    case 16726215: //key on Remote: 5
      return (17);
      break;
    case 16734885: //key on Remote: 6
      return (18);
      break;
    case 16728765: //key on Remote: 7
      return (19);
      break;
    case 16730805: //key on Remote: 8
      return (20);
      break;
    case 16732845: //key on Remote: 9
      return (21);
      break;
    default:
      return (0);
      break;
  }
}


void inputAction (int selection) {  //Function to manage the MenuSctructure
  //Menu Structure
  switch (menustep) {
    case 0: //root
      switch (selection) {
        case 9: //dollyshot or timelapse
          changeParameter(3);
          break;
        case 7: //dollyshot or timelapse
          changeParameter(3);
          break;
        case 5: //enter
          if (parameters[menustep] == 0) {
            nextmenustep = 1; //to dolly-direction
          } else if (parameters[menustep] == 1) {
            nextmenustep = 4; //to timelapse
          } else {
            nextmenustep = 7; //to setup-Sound
          }
          changeMenuStep(nextmenustep);
          break;
      }
      break;

    //Dollymenu
    case 1: //dolly - direction
      ledOn('b');
      switch (selection) {
        case 7:
          changeParameter(2);
          break;
        case 9:
          changeParameter(2);
          break;
        case 5: //enter
          changeMenuStep(2); //to dolly-duration
          ledOn('b');
          ledOn('g');
          break;
      }
      break;
    case 2: //dolly - duration
      switch (selection) {
        default:
          numberInput();
          break;
        case 5: //enter
          changeMenuStep(3); // to dolly-distance
          ledOn('b');
          ledOn('g');
          ledOn('r');
          break;
      }
      break;
    case 3: //dolly - distance
      switch (selection) {
        default:
          numberInput();
          break;
        case 5: //enter starts drive
          parameters[3] = parameters[3] * stepsPerCm; //calc cm to steps
          parameters[2] = parameters[3] / parameters[2]; //calc rpm(steps per second) from distance(steps)/duration(s)
          moveDolly(parameters[1], parameters[2], parameters[3]);
          changeMenuStep(10);
          //repeatMovement();
          break;
      }
      break;

    //Timelapsemenu
    case 4: //timelapse - shots
      switch (selection) {
        default:
          numberInput();
          break;
        case 5:
          changeMenuStep(5);
          break;
      }
      break;
    case 5: //timelapse - interval
      switch (selection) {
        default:
          numberInput();
          break;
        case 5:
          changeMenuStep(6);
          break;
      }
      break;
    case 6: //timelapse - distance / timelapse-start / timelapse-drive
      switch (selection) {
        default:
          numberInput();
          break;
        case 5:
          switch (selection) {
            default:
              lcd.setCursor(0, 0);
              lcd.print("Want to start?");
              lcd.setCursor(0, 1);
              lcd.print(parameters[4]);
              lcd.print("shots  ");
              lcd.print(parameters[5]);
              lcd.print("ms  ");
              lcd.print(parameters[6]);
              lcd.print("cm");
            case 5:
              Serial.println(parameters[4]);
              Serial.println(parameters[5]);
              Serial.println(parameters[6]);
              moveTimelapse(parameters[4], parameters[5], parameters[6]);
              reset();
          }
      }
      break;
    case 7: //setup - Sound
      switch (selection) {
        case 7:
          changeParameter(2);
          break;
        case 9:
          changeParameter(2);
          break;
        case 5: //enter
          changeMenuStep(8);
          break;
      }
      break;
    case 8: //setup - LED
      switch (selection) {
        case 7:
          changeParameter(2);
          break;
        case 9:
          changeParameter(2);
          break;
        case 5: //enter
          changeMenuStep(9);
          break;
      }
      break;
    case 9: //setup - Ease
      switch (selection) {
        case 7:
          changeParameter(2);
          break;
        case 9:
          changeParameter(2);
          break;
        case 5: //enter
          changeMenuStep(0);
          break;
      }
      break;
    case 10: //repeatMovement
      switch (keyvalue) {
        case 7:
          changeParameter(2);
          break;
        case 9:
          changeParameter(2);
          break;
        case 5: //enter
          if (parameters[10] == 1) {
            for (int i = 0; i < 2; i++) { //drive twice, once back, once forward
              if (parameters[1] == 0) { //change direction
                parameters[1] = 1;
              } else if (parameters[1] == 1) {
                parameters[1] = 0;
              }
              moveDolly(parameters[1], parameters[2], parameters[3]);
            }
            changeMenuStep(10);
          } else {
            changeMenuStep(0);
            //reset();
          }
          break;
      }
  }
}


void printScreen() { //Function to print text from the menuscreens Array on the Screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuscreens[menustep][0][0]);
  lcd.setCursor(0, 1);
  lcd.print("> ");
  String activePrameter = menuscreens[menustep][1][parameters[menustep]];
  if (activePrameter != "" || activePrameter != NULL) {
    lcd.print(activePrameter);
  }
  else
  {
    lcd.print(numArray[0]);
    lcd.print(numArray[1]);
    lcd.print(numArray[2]);
    lcd.print(numArray[3]);
    lcd.print(" ");
    lcd.print(menuscreens[menustep][0][1]);
  }
}

void reset() { //Function to Reset all e.g. after canceling
  changeMenuStep(0);
  printScreen();
  cancel = false;
}

void piep(int times, int waitTime) { //function to piep the speaker with Parameters 1-how often 2-how long to wait between in ms
  unsigned long preMillis = millis();
  int pieped = 0;
  if (parameters[7] == 0) {
    do {
      if (millis() - preMillis >= waitTime) {
        preMillis = millis();
        NewTone(TONE_PIN, 125);
        delay(200);
        noNewTone(TONE_PIN);
        pieped++;
      }
    } while (!cancel && pieped < times);
  }
}

void ledOn (char color) { //function to turn on an LED Color with Parameter 1- which color (r,g,b)
  if (parameters[8] == 0) {
    switch (color) {
      case 'r':
        analogWrite(LEDr, brightness);
        break;
      case 'g':
        analogWrite(LEDg, brightness);
        break;
      case 'b':
        analogWrite(LEDb, brightness);
        break;
    }
  }
}

void changeMenuStep(int newStep) { //function to change the MenuStep in the menu
  menustep = newStep;
  for (int i = 0; i < sizeof(numArray); ++i) {//reset numArray for new numberInput
    numArray[i] = 0;
  }
  piep(1,0);
  analogWrite(LEDb, 0);
  analogWrite(LEDg, 0);
  analogWrite(LEDr, 0);
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


void moveDolly(int movedirection, int movespeed, int movesteps) { //function to move the Motor with Parameters
  analogWrite(LEDr, 0);
  analogWrite(LEDg, 0);
  analogWrite(LEDb, 0);
  if (movedirection == 0) {
    ledOn('g');
  }
  else if (movedirection == 1) {
    movesteps = 0 - movesteps;
    ledOn('r');
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Driving...");
  stepper.setMaxSpeed(movespeed);
  stepper.move(movesteps);
  if (parameters[9] == 1) {
    stepper.setSpeed(movespeed);
    while (stepper.distanceToGo() != 0 && !cancel) {
      stepper.runSpeed();
      recieveIR();
    }
  } else {
    while (stepper.distanceToGo() != 0 && !cancel) {
      stepper.run();
      recieveIR();
    }
  }
  analogWrite(LEDg, 0);
  analogWrite(LEDr, 0);
  piep(2, 100);
}

void moveTimelapse (int shotCount, int interval, int movesteps) { //Function to move the Motor more times and with an interval
  movesteps = movesteps * stepsPerCm; //movesteps needs to be calculatet from cm to steps
  int shotsDone = 0;
  unsigned long preMillis = millis();
  do {
    if (millis() - preMillis >= interval) {
      preMillis = millis();
      moveDolly(1, fullSpeed, movesteps);
      shotsDone++;
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Timelapse - Standby");
      lcd.setCursor(0, 1);
      lcd.print("Shots left: ");
      lcd.print(shotCount - shotsDone);
      recieveIR();
    }
  } while (!cancel && shotsDone < shotCount);
  shotCount = 0;
  piep(5, 50); //timelapse ended alarm

}


void numberInput () { //Function to input 4-digit numbers with remote an display it on Screen
  for (int i = 0; i <= 3; i++) {
    lcd.setCursor(2 + i, 1);
    lcd.blink();
    int tempinput = 0;
    while (tempinput == 0) {
      tempinput = recieveIR();
      if (tempinput < 22 && tempinput > 12) {
        numArray[i] = tempinput - 12;
      } else if (tempinput == 10) {
        numArray[i] = 0;
      }
      else {
        tempinput = 0;
      }
    }
    printScreen();
    delay(50);
  }
  lcd.noBlink();
  parameters[menustep] = numArray[0] * 1000 + numArray[1] * 100 + numArray[2] * 10 + numArray[3];
}
