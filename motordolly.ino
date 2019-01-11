#include <NewTone.h>
#include <Stepper.h>
#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TONE_PIN 7

//LED
int LEDr = 9;
int LEDg = 10;
int LEDb = 12;
int brightness = 150;

//Menu
int nextmenustep = 0;
int menustep = 0; // 1= dolly{10=direction, 11=speed, 12=range}, 2=timelapse{20=shots, 21=interval, 22=length}
int movedirection = 0; //0=neutral, 1=forward, 2=back;
const int movespeed = 15 / 9; //rpm (rounds per minute) (max = 15)
const int movesteps = 2048 / 8; //2048 = 1 Umdrehung
boolean cancel = false;
long rideArray[3] = {movedirection, movespeed, movesteps}; //{movedirection, movespeed, movesteps} or Timelapse {shots,interval,length}
long numArray [4]; //for 4-digit NumberInputs


//Motor
int SPU = 2048;
Stepper Motor(SPU, 3, 5, 4, 6);

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
  pinMode(LEDr, OUTPUT);
  pinMode(LEDg, OUTPUT);
  pinMode(LEDb, OUTPUT);
  Motor.setSpeed(5);
  lcd.init();
  lcd.backlight();
}

void loop() {
  int keyvalue = 0;
  //lcd.setCursor(0, 0);
  //lcd.print("Motorized Skater Dolly");
  //lcd.setCursor(0, 1);
  //lcd.print("Los gehts!");
  cancel = false;

  //Menu Structure
  switch (menustep) {
    case 0:
      //Serial.println("Dolly oder Timelapse? Up/Down/Play");
      /*lcd.setCursor(0, 0);
      lcd.print("[*] Dollyshot");
      lcd.setCursor(0, 1);
      lcd.print("[ ] Timelapse");*/
      keyvalue = recieveIR();
      switch (keyvalue) {
        case 9:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("[*] Dollyshot");
          lcd.setCursor(0, 1);
          lcd.print("[ ] Timelapse");
          nextmenustep = 10;
          break;
        case 7:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("[ ] Dollyshot");
          lcd.setCursor(0, 1);
          lcd.print("[*] Timelapse");
          nextmenustep = 20;
          break;
        case 5: //enter
          if (nextmenustep == 10 || nextmenustep == 20) {
            changeMenuStep(nextmenustep);
          }
          break;
      }
      break;

    //Dollymenu
    case 10: //dolly - direction
      analogWrite(LEDb, brightness);
      //Serial.println("Which direction to move? (vol +/-)");
      lcd.setCursor(0, 0);
      lcd.print("Which direction to move? (vol +/-)");
      keyvalue = recieveIR();
      switch (keyvalue) {
        case 7:
          lcd.setCursor(0, 1);
          lcd.print("backwards");
          rideArray[0] = 2;
          nextmenustep = 11;
          break;
        case 9:
          lcd.setCursor(0, 1);
          lcd.print("forward");
          rideArray[0] = 1;
          nextmenustep = 11;
          break;
        case 5: //enter
          changeMenuStep(nextmenustep);
          break;
      }
      keyvalue = 0;
      break;
    case 11: //dolly - speed
      analogWrite(LEDb, brightness);
      analogWrite(LEDg, brightness);
      //Serial.println("How fast to move? (r per min)");
      lcd.setCursor(0, 0);
      lcd.print("How fast to move? (r per min)");
      keyvalue = recieveIR();
      //Serial.println(keyvalue);
      switch (keyvalue) {
        case 13: //key: 1
          rideArray[1] = movespeed * 1;
          changeMenuStep(12);
          break;
        case 14: //key: 2
          rideArray[1] = movespeed * 2;
          changeMenuStep(12);
          break;
        case 15: //key: 3
          rideArray[1] = movespeed * 3;
          changeMenuStep(12);
          break;
        case 16: //key: 4
          rideArray[1] = movespeed * 4;
          changeMenuStep(12);
          break;
        case 17: //key: 5
          rideArray[1] = movespeed * 5;
          changeMenuStep(12);
          break;
        case 18: //key: 6
          rideArray[1] = movespeed * 6;
          changeMenuStep(12);
          break;
        case 19: //key: 7
          rideArray[1] = movespeed * 7;
          changeMenuStep(12);
          break;
        case 20: //key: 8
          rideArray[1] = movespeed * 8;
          changeMenuStep(12);
          break;
        case 21: //key: 9
          rideArray[1] = movespeed * 9;
          changeMenuStep(12);
          break;
      }
      keyvalue = 0;
      break;
    case 12: //dolly - length
      analogWrite(LEDb, brightness);
      analogWrite(LEDg, brightness);
      analogWrite(LEDr, brightness);
      //Serial.println("How far to move? (in eigth-steps)");
      lcd.setCursor(0, 0);
      lcd.print("How far to move? (in eigth-steps)");
      keyvalue = recieveIR();
      switch (keyvalue) {
        case 13: //key: 1
          rideArray[2] = movesteps * 1;
          changeMenuStep(13);
          break;
        case 14: //key: 2
          rideArray[2] = movesteps * 2;
          changeMenuStep(13);
          break;
        case 15: //key: 3
          rideArray[2] = movesteps * 3;
          changeMenuStep(13);
          break;
        case 16: //key: 4
          rideArray[2] = movesteps * 4;
          changeMenuStep(13);
          break;
        case 17: //key: 5
          rideArray[2] = movesteps * 5;
          changeMenuStep(13);
          break;
        case 18: //key: 6
          rideArray[2] = movesteps * 6;
          changeMenuStep(13);
          break;
        case 19: //key: 7
          rideArray[2] = movesteps * 70;
          changeMenuStep(13);
          break;
        case 20: //key: 8
          rideArray[2] = movesteps * 8;
          changeMenuStep(13);
          break;
        case 21: //key: 9
          rideArray[2] = movesteps * 9;
          changeMenuStep(13);
          break;
      }
      keyvalue = 0;
      break;
    case 13: //dolly - drive
      moveDolly(rideArray[0], rideArray[1], rideArray[2]);
      repeatMovement();
      break;


    //Timelapsemenu
    case 20: //timelapse - shots
      lcd.setCursor(0, 0);
      lcd.print("How many pictures to take?");
      lcd.setCursor(0, 1);
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
      rideArray[0] = numArray[0] * 100 + numArray[1] * 10 + numArray[2];
      changeMenuStep(21);
      break;
    case 21: //timelapse - interval
      lcd.setCursor(0, 0);
      lcd.print("How long to wait? in s");
      lcd.setCursor(0, 1);
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
      rideArray[1] = numArray[0] * 1000000 + numArray[1] * 100000 + numArray[2] * 10000 + numArray[3] * 1000; //from ms to sec
      changeMenuStep(22);
      break;
    case 22: //timelapse - length
      lcd.setCursor(0, 0);
      lcd.print("How far to move?");

      keyvalue = recieveIR();//just FOR TESTING #################################################
      if (keyvalue == 5) {
        changeMenuStep(23);
      }

      rideArray[2] = 1024;
      keyvalue = 0;
      break;
    case 23: //timelapse - start?
      lcd.setCursor(0, 0);
      lcd.print("Want to start?");
      lcd.setCursor(0, 1);
      lcd.print(rideArray[0]);
      lcd.print("shots  ");
      lcd.print(rideArray[1] / 1000);
      lcd.print("s  ");
      lcd.print(rideArray[2]);
      lcd.print("steps");
      keyvalue = recieveIR();
      if (keyvalue == 5) {
        changeMenuStep(24);
      }
      keyvalue = 0;
      break;
    case 24: //timelapse - drive
      moveTimelapse(rideArray[0], rideArray[1], rideArray[2]);
      reset();
      break;
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
  lcd.clear();
  analogWrite(LEDb, 0);
  analogWrite(LEDg, 0);
  analogWrite(LEDr, 0);
}

void moveDolly(int movedirection, int movespeed, int movesteps) {
  if (movedirection == 1) {
    analogWrite(LEDg, brightness);
  }
  else if (movedirection == 2) {
    movesteps = 0 - movesteps;
    analogWrite(LEDr, brightness);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Driving...");
  Motor.setSpeed(movespeed);
  Motor.step(movesteps); // Der Motor macht 2048 Schritte, das entspricht einer Umdrehung.
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
        lcd.print("Yes");
        input = true;
        break;
      case 9:
        lcd.setCursor(0, 1);
        lcd.print("No ");
        input = false;
        break;
      case 5: //enter
        if (input) {
          for (int i = 0; i < 2; i++) { //drive twice, once back, once forward
            if (rideArray[0] == 1) { //change direction
              rideArray[0] = 2;
            } else if (rideArray[0] == 2) {
              rideArray[0] = 1;
            }
            moveDolly(rideArray[0], rideArray[1], rideArray[2]);
          }
        } else {
          repeat = false;
        }
        break;
    }
    keyvalue = 0;
    //irrec.resume();
  }
  reset();
}

void moveTimelapse (int shotcount, int interval, int movesteps) {
  int shotsDone = 0;
  unsigned long preMillis = millis();
  moveDolly(1, 12, movesteps);
  shotsDone++;
  while (!cancel && shotsDone < shotcount) {
    if (millis() - preMillis >= interval) {
      // for (int i = 0; i < shotcount; i++) {
      preMillis = millis();
      moveDolly(1, 12, movesteps);
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
    }
  }
}
