void inputAction (int selection) {  //Function to manage the MenuSctructure
  switch (menustep) {
    case 0: //root
      switch (selection) {
        case 9: //dollyshot or timelapse or setup
          changeParameter(3);
          break;
        case 7: //dollyshot or timelapse or setup
          changeParameter(3);
          break;
        case 5: //enter
          if (parameters[menustep] == 0) {
            nextmenustep = 1; //to dolly-direction
            ledOff();
            ledOn('b');
          } else if (parameters[menustep] == 1) {
            nextmenustep = 4; //to timelapse
          } else {
            nextmenustep = 8; //to setup-Sound
          }
          changeMenuStep(nextmenustep);
          break;
      }
      break;

    //Dollymenu
    case 1: //dolly - direction
      switch (selection) {
        case 7:
          changeParameter(2);
          break;
        case 9:
          changeParameter(2);
          break;
        case 5: //enter
          changeMenuStep(menustep + 1); //to dolly-duration
          ledOff();
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
          getnumber=false;
          changeMenuStep(menustep + 1); // to dolly-distance
          ledOff();
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
          getnumber=false;
          if (parameters[2] > 0 && parameters[3] > 0) {
            parameters[3] = parameters[3] * stepsPerCm; //calc cm to steps
            parameters[2] = parameters[3] / parameters[2]; //calc rpm(steps per second) from distance(steps)/duration(s)
            moveDolly(parameters[1], parameters[2], parameters[3]);
            changeMenuStep(11); //to repeatMovement
          } else {
            changeMenuStep(0);
          }
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
          getnumber=false;
          changeMenuStep(menustep + 1);
          break;
      }
      break;
    case 5: //timelapse - interval
      switch (selection) {
        default:
          numberInput();
          break;
        case 5:
        getnumber=false;
          changeMenuStep(menustep + 1);
          break;
      }
      break;
    case 6: //timelapse - distance
      switch (selection) {
        default:
          numberInput();
          break;
        case 5:
        getnumber=false;
          changeMenuStep(menustep + 1);
          break;
      }
      break;
    case 7: //timelapse - start and drive
      switch (selection) {
        case 4:
          changeMenuStep(4);
          break;
        case 5:
          Serial.println(parameters[4]);
          Serial.println(parameters[5]);
          Serial.println(parameters[6]);
          moveTimelapse(parameters[4], parameters[5], parameters[6]);
          changeMenuStep(0);
          break;
      }
      break;
    case 8: //setup - Sound
      switch (selection) {
        case 7:
          changeParameter(2);
          break;
        case 9:
          changeParameter(2);
          break;
        case 5: //enter
          changeMenuStep(menustep + 1);
          break;
      }
      break;
    case 9: //setup - LED
      switch (selection) {
        case 7:
          changeParameter(2);
          break;
        case 9:
          changeParameter(2);
          break;
        case 5: //enter
          changeMenuStep(menustep + 1);
          break;
      }
      break;
    case 10: //setup - Ease
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
    case 11: //repeatMovement
      switch (selection) {
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
            changeMenuStep(11);
          } else {
            changeMenuStep(0);
            //reset();
          }
          break;
      }
      break;
  }
}
