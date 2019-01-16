#include <NewTone.h>
#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>

//Sound pin
#define TONE_PIN 7
// Motor pins
#define HALFSTEP 8
#define motorPin1  2
#define motorPin2  3
#define motorPin3  4
#define motorPin4  5

//LED Pins
int LEDr = 9;
int LEDg = 10;
int LEDb = 6;
int brightness = 100;

//Menu
int keyvalue = 0;
int nextmenustep = 0;
int menustep = 0; // 1= dolly{1=direction, 2=duration, 3=range}, 4=timelapse{4=shots, 5=interval, 6=distance}
int movedirection = 0; //0=forwards, 1=backwards;
int fullSpeed = 900.0;
const int movespeed = fullSpeed / 9; //rpm (rounds per minute) (max = 15 /9 at Stepper.h)
const int movesteps = 2048 / 8; //2048 = 1 Umdrehung
boolean cancel = false;
long rideArray[3] = {movedirection, movespeed, movesteps}; //{movedirection, movespeed, movesteps} or Timelapse {shots,interval,distance}
int numArray [4] = {0, 0, 0, 1}; //for 4-digit NumberInputs
const int numScreens = 10;
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
};
int parameters[numScreens];

//Motor
AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
int stepsPerCm = 2048;

//IR-Remote
int IRmodul = 8;
IRrecv irrec(IRmodul);
decode_results recieved;
boolean getNewInput = true;

//Display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  irrec.enableIRIn();

  //LEDs Setup
  pinMode(LEDr, OUTPUT);
  pinMode(LEDg, OUTPUT);
  pinMode(LEDb, OUTPUT);

  //Motor Setup
  stepper.setAcceleration(80.0);

  //LCD Setup
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < numScreens; i++) {
    parameters[i] = 0;
  }

  printScreen();
}

void loop() {
  while (getNewInput) {
    keyvalue = recieveIR();
  }
  if (keyvalue != 0) {
    inputAction(keyvalue);
    printScreen();
  }
  getNewInput = true;
}

int recieveIR() {
  if (irrec.decode(&recieved)) {
    Serial.println(recieved.value, DEC);
    getNewInput = false;
    irrec.resume();
  }
  switch (recieved.value) {
    case 16753245: //key: power
      cancel = true;
      stepper.stop();
      reset();
      return (1);
      break;
    case 16736925: //key: vol+
      return (2);
      break;
    case 16769565: //key: func/stop
      return (3);
      break;
    case 16720605: //key: <<
      return (4);
      break;
    case 16712445: //key: >
      return (5);
      break;
    case 16761405: //key: >>
      return (6);
      break;
    case 16769055: //key: down
      return (7);
      break;
    case 16754775: //key: vol-
      return (8);
      break;
    case 16748655: //key: up
      return (9);
      break;
    case 16738455: //key: 0
      return (10);
      break;
    case 16750695: //key: EQ
      return (11);
      break;
    case 16756815: //key: ST/REPT
      return (12);
      break;
    case 16724175: //key: 1
      return (13);
      break;
    case 16718055: //key: 2
      return (14);
      break;
    case 16743045: //key: 3
      return (15);
      break;
    case 16716015: //key: 4
      return (16);
      break;
    case 16726215: //key: 5
      return (17);
      break;
    case 16734885: //key: 6
      return (18);
      break;
    case 16728765: //key: 7
      return (19);
      break;
    case 16730805: //key: 8
      return (20);
      break;
    case 16732845: //key: 9
      return (21);
      break;
    default:
      return (0);
      break;
  }
}


void inputAction (int selection) {
  //Menu Structure
  switch (menustep) {
    case 0: //root
      switch (selection) {
        case 9: //dollyshot or timelapse
          changeParameter(3, 1);
          break;
        case 7: //dollyshot or timelapse
          changeParameter(3, 1);
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
          changeParameter(2, 1);
          break;
        case 9:
          changeParameter(2, 1);
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
          parameters[2] = parameters[3] * stepsPerCm / parameters[2]; //calc rpm(steps per second) from distance(steps)/duration(s)
          moveDolly(parameters[1], parameters[2], parameters[3]);
          repeatMovement();
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
              moveTimelapse(parameters[4], parameters[5], parameters[6]);
              reset();
          }
      }
      break;
    case 7: //setup - Sound
      switch (selection) {
        case 7:
          changeParameter(2, 1);
          break;
        case 9:
          changeParameter(2, 1);
          break;
        case 5: //enter
          changeMenuStep(8);
          break;
      }
      break;
    case 8: //setup - LED
      switch (selection) {
        case 7:
          changeParameter(2, 1);
          break;
        case 9:
          changeParameter(2, 1);
          break;
        case 5: //enter
          changeMenuStep(9);
          break;
      }
      break;
    case 9: //setup - Ease
      switch (selection) {
        case 7:
          changeParameter(2, 1);
          break;
        case 9:
          changeParameter(2, 1);
          break;
        case 5: //enter
          changeMenuStep(0);
          break;
      }
      break;
  }
}

void printScreen () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuscreens[menustep][0][0]);
  lcd.setCursor(0, 1);
  lcd.print("> ");
  String activePrameter = menuscreens[menustep][1][parameters[menustep]];
  if (activePrameter != "") {
    lcd.print(activePrameter);
  }
  else
  {
    lcd.print(parameters[menustep]);
    lcd.print(" ");
    lcd.print(menuscreens[menustep][0][1]);
  }
}

void reset() {
  changeMenuStep(0);
  printScreen();
  cancel = false;
}

void piep() {
  if (parameters[7] == 0) {
    NewTone(TONE_PIN, 125);
    delay(200);
    noNewTone(TONE_PIN);
  }
}

void ledOn (char color) {
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

void changeMenuStep(int newStep) {
  menustep = newStep;
  piep();
  analogWrite(LEDb, 0);
  analogWrite(LEDg, 0);
  analogWrite(LEDr, 0);
}

void changeParameter(int values, int decimal) {
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


void moveDolly(int movedirection, int movespeed, int movesteps) {
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
  piep();
  delay(100);
  piep();
}

void repeatMovement() {
  boolean repeat = true;
  boolean input = true;
  lcd.clear();
  while (repeat && !cancel) {
    int keyvalue = recieveIR();
    lcd.setCursor(0, 0);
    lcd.print("Want to repeat?");
    switch (keyvalue) {
      case 7:
        lcd.setCursor(0, 1);
        lcd.print("> Yes");
        input = true;
        break;
      case 9:
        lcd.setCursor(0, 1);
        lcd.print("> No ");
        input = false;
        break;
      case 5: //enter
        if (input) {
          for (int i = 0; i < 2; i++) { //drive twice, once back, once forward
            if (parameters[1] == 0) { //change direction
              parameters[1] = 1;
            } else if (parameters[1] == 1) {
              parameters[1] = 0;
            }
            moveDolly(parameters[1], parameters[2], parameters[3]);
          }
        } else {
          repeat = false;
        }
        break;
    }
  }
  reset();
}

void moveTimelapse (int shotcount, int interval, int movesteps) {
  movesteps = movesteps * stepsPerCm; //movesteps needs to be calculatet from cm to steps
  int shotsDone = 0;
  unsigned long preMillis = millis();
  moveDolly(1, fullSpeed, movesteps);
  shotsDone++;
  while (!cancel && shotsDone < shotcount) {
    if (millis() - preMillis >= interval) {
      preMillis = millis();
      moveDolly(1, fullSpeed, movesteps);
      shotsDone++;
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Timelapse - Standby");
      lcd.setCursor(0, 1);
      lcd.print("Shots left: ");
      lcd.print(shotcount - shotsDone);
      recieveIR();
    }
  }
  shotcount = 0;
  for (int i = 0; i <= 5; i++) { //timelapse ended alarm
    piep();
    delay (50);
  }
}


void numberInput () {
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
    parameters[menustep] = numArray[0] * 1000 + numArray[1] * 100 + numArray[2] * 10 + numArray[3];
    printScreen();
    delay(200);
  }
  lcd.noBlink();
  for (int i = 0; i < sizeof(numArray); ++i) {//reset Array for new
    numArray[i] = 0;
  }
}
