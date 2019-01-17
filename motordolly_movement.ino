void moveDolly(int movedirection, int movespeed, int movesteps) { //function to move the Motor with Parameters
  if (movespeed > 0 && movesteps > 0 && !cancel) {
    analogWrite(LEDr, 0);
    analogWrite(LEDg, 0);
    analogWrite(LEDb, 0);
    if (movedirection == 0) {
      ledOff();
      ledOn('g');
    }
    else if (movedirection == 1) {
      movesteps = 0 - movesteps;
      ledOff();
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
    ledOff();
    piep(2, 100);
  }
}

void moveTimelapse (int shotCount, int interval, int movesteps) { //Function to move the Motor more times and with an interval
  if (shotCount > 0 && interval > 0 && movesteps > 0 && !cancel) {
    movesteps = movesteps * stepsPerCm; //movesteps needs to be calculatet from cm to steps
    interval = interval * 1000; //calc interval from seconds to ms
    int shotsDone = 0;
    unsigned long preMillis = 0;
    do {
      if (millis() - preMillis >= interval) {
        preMillis = millis();
        moveDolly(1, fullSpeed, movesteps);
        shotsDone++;
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Timelapse - Wait");
        lcd.setCursor(0, 1);
        lcd.print(shotCount - shotsDone);
        lcd.print(" shots in ");
        lcd.print((interval - (millis() - preMillis)) / 1000);
        lcd.print("s");
      }
      recieveIR();
    } while (!cancel && shotsDone < shotCount);
    shotCount = 0;
    piep(5, 200); //timelapse ended alarm
  }
} 
