#include <NewTone.h>
//#include <Stepper.h>
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
int menustep = 0; // 1= dolly{1=direction, 2=speed, 3=range}, 4=timelapse{4=shots, 5=interval, 6=length}
int movedirection = 0; //0=forward, 1=back;
int fullSpeed = 900.0;
const int movespeed = fullSpeed / 9; //rpm (rounds per minute) (max = 15 /9 at Stepper.h)
const int movesteps = 2048 / 8; //2048 = 1 Umdrehung
boolean cancel = false;
long rideArray[3] = {movedirection, movespeed, movesteps}; //{movedirection, movespeed, movesteps} or Timelapse {shots,interval,length}
long numArray [4] = {0, 0, 0, 1}; //for 4-digit NumberInputs
const int numScreens = 10;
String menuscreens[numScreens][2][2] = {
  {{"Mode", ""}, {"Dolly", "Timelapse"}},
  {{"Direction", ""}, {"forward", "backwards"}}, //dolly 1
  {{"Duration", "ms"}, {"", ""}},         //dolly 2
  {{"Length", "cm"}, {"", ""}},          //dolly 3
  {{"Shots", "pics"}, {"", ""}},         //timelapse 4
  {{"Interval", "ms"}, {"", ""}},         //timelapse 5
  {{"Length", "cm"}, {"", ""}},           //timelapse 6
  {{"Sound", ""}, {"On", "Off"}},    //setup 7
  {{"LED", ""}, {"On", "Off"}},       //setup 8
  {{"Ease In/Out", ""}, {"On", "Off"}}, //setup 9
};
int parameters[numScreens];

//Motor
int SPU = 2048;
//Stepper Motor(SPU, 3, 5, 4, 6);
AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

//IR-Remote
int IRmodul = 8;
IRrecv irrec(IRmodul);
decode_results recieved;

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

  //Motor.setSpeed(5);
  stepper.setAcceleration(80.0);

  lcd.init();
  lcd.backlight();

  // lcd.setCursor(0, 0);
  //lcd.print("> Dollyshot");
  //lcd.setCursor(0, 1);
  //lcd.print("  Timelapse");
  //nextmenustep = 10;
  printScreen();
}

void loop() {
  keyvalue = 0;
  keyvalue = recieveIR();
  if (keyvalue != 0) {
    inputAction(keyvalue);
    printScreen();
  }
}

void reset() {
  changeMenuStep(0);
}

void piep() {
  NewTone(TONE_PIN, 125);
  delay(200);
  noNewTone(TONE_PIN);
}

void changeMenuStep(int newStep) {
  menustep = newStep;
  piep();
  analogWrite(LEDb, 0);
  analogWrite(LEDg, 0);
  analogWrite(LEDr, 0);
}

void changeParameter(int values, int decimal) {
  if (values == 2) {
    if (parameters[menustep] == 0) {
      parameters[menustep]=1;
    } else {
      parameters[menustep]=0;
    }
  } else {
    if (keyvalue == 9) {
      parameters[menustep]++;
    } else if (keyvalue == 7 && parameters[menustep] > 0) {
      parameters[menustep]--;
    }
  }
}


void moveDolly(int movedirection, int movespeed, int movesteps) {
  if (movedirection == 0) {
    analogWrite(LEDg, brightness);
  }
  else if (movedirection == 1) {
    movesteps = 0 - movesteps;
    analogWrite(LEDr, brightness);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Driving...");
  stepper.setMaxSpeed(movespeed);
  stepper.move(movesteps);
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }
  //Motor.setSpeed(movespeed);
  //Motor.step(movesteps); // Der Motor macht 2048 Schritte, das entspricht einer Umdrehung.
  analogWrite(LEDg, 0);
  analogWrite(LEDr, 0);
  piep();
  delay(100);
  piep();
}

void repeatMovement() {
  boolean repeat = true;
  boolean input = true;
  while (repeat) {
    lcd.setCursor(0, 0);
    lcd.print("Want to repeat?");
    int keyvalue = recieveIR();
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
            //moveDolly(rideArray[0], rideArray[1], rideArray[2]);
          }
        } else {
          repeat = false;
        }
        break;
    }
    keyvalue = 0;
  }
  reset();
}

void moveTimelapse (int shotcount, int interval, int movesteps) {
  int shotsDone = 0;
  unsigned long preMillis = millis();
  moveDolly(1, fullSpeed, movesteps);
  shotsDone++;
  while (!cancel && shotsDone < shotcount) {
    if (millis() - preMillis >= interval) {
      // for (int i = 0; i < shotcount; i++) {
      preMillis = millis();
      moveDolly(1, fullSpeed, movesteps);
      shotsDone++;
      // }
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Timelapse - Standby");
      lcd.setCursor(0, 1);
      lcd.print("Shots left: ");
      lcd.print(shotcount - shotsDone);
      recieveIR();
    }
  }
  for (int i = 0; i <= 5; i++) { //timelapse ended alarm
    piep();
    delay (50);
  }
}


int recieveIR() {
  if (irrec.decode(&recieved)) {
    Serial.println(recieved.value, DEC);
    irrec.resume();
  }
  switch (recieved.value) {
    case 16753245: //key: power
      cancel = true;
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
    default :
      return (0);
      break;
  }
}

void inputAction (int keyvalue) {
  //Menu Structure
  switch (menustep) {
    case 0: //root
      switch (keyvalue) {
        case 9: //dollyshot or timelapse
          changeParameter(2, 1);
          break;
        case 7: //dollyshot or timelapse
          changeParameter(2, 1);
          break;
        case 5: //enter
          if (parameters[menustep] == 0) {
            nextmenustep = 1; //to dolly-direction
            analogWrite(LEDb, brightness);
          } else {
            nextmenustep = 4; //to timelapse
          }
          Serial.println(nextmenustep);
          changeMenuStep(nextmenustep);
          break;
      }
      break;

    //Dollymenu
    case 1: //dolly - direction
      switch (keyvalue) {
        case 7:
          changeParameter(2, 1);
          break;
        case 9:
          changeParameter(2, 1);
          break;
        case 5: //enter
          changeMenuStep(2); //to dolly-speed
          analogWrite(LEDb, brightness);
          analogWrite(LEDg, brightness);
          break;
      }
      break;
    case 2: //dolly - speed
      switch (keyvalue) {
        case 7:
          changeParameter(1, 4);
          break;
        case 9:
          changeParameter(1, 4);
          break;
        case 5: //enter
          changeMenuStep(3); // to dolly-length
          analogWrite(LEDb, brightness);
          analogWrite(LEDg, brightness);
          analogWrite(LEDr, brightness);
          break;
      }
      //keyvalue = 0;
      break;
    case 3: //dolly - length
      switch (keyvalue) {
        case 7:
          changeParameter(1, 4);
          break;
        case 9:
          changeParameter(1, 4);
          break;
        case 5: //enter
          moveDolly(parameters[1], parameters[2], parameters[3]);
          repeatMovement();
          break;
      }
      //keyvalue = 0;
      break;
    case 31: //dolly - drive
      moveDolly(parameters[1], parameters[2], parameters[3]);
      //moveDolly(rideArray[0], rideArray[1], rideArray[2]);
      repeatMovement();
      break;

    //Timelapsemenu
    case 4: //timelapse - shots
      //lcd.setCursor(0, 0);
      //lcd.print("How many pictures to take?");
      lcd.setCursor(2, 1);
      for (int i = 0; i <= 2; i++) {
        while (keyvalue == 0) {
          keyvalue = recieveIR();
          if (keyvalue < 22 && keyvalue > 12) {
            numArray[i] = keyvalue - 12;
          } else if (keyvalue == 10) {
            numArray[i] = 0;
          }
          else {
            keyvalue = 0;
          }
        }
        lcd.print(numArray[i]);
        keyvalue = 0;
      }
      parameters[menustep] = numArray[0] * 100 + numArray[1] * 10 + numArray[2];
      changeMenuStep(5);
      break;
    case 5: //timelapse - interval
      //lcd.setCursor(0, 0);
      //lcd.print("How long to wait? in s");
      lcd.setCursor(2, 1);
      for (int i = 0; i <= 3; i++) {
        while (keyvalue == 0) {
          keyvalue = recieveIR();
          if (keyvalue < 22 && keyvalue > 12) {
            numArray[i] = keyvalue - 12;
          } else if (keyvalue == 10) {
            numArray[i] = 0;
          }
          else {
            keyvalue = 0;
          }
        }
        lcd.print(numArray[i]);
        keyvalue = 0;
      }
      parameters[menustep] = numArray[0] * 1000000 + numArray[1] * 100000 + numArray[2] * 10000 + numArray[3] * 1000; //from ms to sec
      changeMenuStep(6);
      break;
    case 6: //timelapse - length
      //lcd.setCursor(0, 0);
      //lcd.print("How far to move?");

      keyvalue = recieveIR();//just FOR TESTING #################################################
      if (keyvalue == 5) {
        changeMenuStep(61);
      }

      parameters[menustep] = 1024;
      keyvalue = 0;
      break;
    case 61: //timelapse - start?
      lcd.setCursor(0, 0);
      lcd.print("Want to start?");
      lcd.setCursor(0, 1);
      lcd.print(parameters[4]);
      lcd.print("shots  ");
      lcd.print(parameters[5] / 1000);
      lcd.print("s  ");
      lcd.print(parameters[6]);
      lcd.print("steps");
      keyvalue = recieveIR();
      if (keyvalue == 5) {
        changeMenuStep(62);
      }
      keyvalue = 0;
      break;
    case 62: //timelapse - drive
      moveTimelapse(parameters[4], parameters[5], parameters[6]);
      reset();
      break;
  }
}

void printScreen () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menuscreens[menustep][0][0]);
  lcd.setCursor(0, 1);
  lcd.print("> ");
  if (menuscreens[menustep][1][parameters[menustep]] != "") {
    lcd.print(menuscreens[menustep][1][parameters[menustep]]);
  }
  else
  {
    for (int i = 0; i <= 3; i++) {
      lcd.print(numArray[i]);
    }
    lcd.print(" ");
    lcd.print(menuscreens[menustep][0][1]);
  }

}
