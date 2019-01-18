void moveDolly(unsigned int movedirection, int movespeed, long movesteps) { //function to move the Motor with Parameters
  if (movespeed > 0 && movesteps > 0 && !cancel) {
    analogWrite(LEDr, 0);
    analogWrite(LEDg, 0);
    analogWrite(LEDb, 0);
    if (movedirection == 0) { //forwards
      ledOff();
      ledOn('g');
    }
    else if (movedirection == 1) { //backwards
      movesteps = 0 - movesteps;
      ledOff();
      ledOn('r');
    }
    lcd.clear();
    lcd.noBlink();
    lcd.setCursor(0, 0);
    lcd.print(F("Driving..."));
    if (movespeed > fullSpeed) {
      movespeed = fullSpeed;
    }
    stepper.setMaxSpeed(movespeed);
    stepper.move(movesteps);
    if (parameters[10] == 1) {
      stepper.setSpeed(movespeed);
      while (stepper.distanceToGo() != 0 && !cancel) {
        stepper.runSpeed();
        recieveIR(0);
      }
    } else {
      while (stepper.distanceToGo() != 0 && !cancel) {
        stepper.run();
        recieveIR(0);
      }
    }
    ledOff();
    piep(2, 100);
  }
}

void moveTimelapse (unsigned int shotCount, unsigned long interval, unsigned long movesteps) { //Function to move the Motor more times and with an interval
  if (shotCount > 0 && interval > 0 && movesteps > 0 && !cancel) {
    int shotsDone = 0;
    int directionTimelapse = 0;
    unsigned long preMillis = 0;
    preMillis = millis();
    moveDolly(directionTimelapse, fullSpeed, movesteps); //firsttime drive
    shotsDone++;
    while (!cancel && shotsDone < shotCount) // drive while shots left
      if (millis() >= preMillis + interval) { //drive after interval time
        preMillis = millis();
        moveDolly(directionTimelapse, fullSpeed, movesteps);
        shotsDone++;
      } else { //show wait screen
        lcd.setCursor(0, 0);
        lcd.print(F("Timelapse - Wait"));
        lcd.setCursor(0, 1);
        lcd.print(shotCount - shotsDone);
        lcd.print(F(" shots in "));
        lcd.print((interval - (millis() - preMillis)) / 1000);
        lcd.print(F("s  "));
        recieveIR(0);
      }
    piep(5, 200); //timelapse ended alarm
  }
}
