void inputAction(int selection)
{ //Function to manage the MenuSctructure
  switch (menustep)
  {
  case 0: //root
    switch (selection)
    {
    case 9: //dollyshot or timelapse or setup
      changeParameter(3);
      break;
    case 7: //dollyshot or timelapse or setup
      changeParameter(3);
      break;
    case 5: //enter
      if (parameters[lookUp(menustep)] == 0)
      {
        nextmenustep = 10; //to dolly-direction
        ledOff();
        ledOn('b');
      }
      else if (parameters[lookUp(menustep)] == 1)
      {
        nextmenustep = 30; //to timelapse
      }
      else
      {
        nextmenustep = 50; //to setup-Sound
      }
      changeMenuStep(nextmenustep);
      break;
    }
    break;

  //Dollymenu
  case 10: //dolly - direction
    switch (selection)
    {
    case 7:
      changeParameter(2);
      break;
    case 9:
      changeParameter(2);
      break;
    case 5:                         //enter
      changeMenuStep(menustep + 1); //to dolly-duration
      ledOff();
      ledOn('b');
      ledOn('g');
      break;
    }
    break;
  case 11: //dolly - duration
    switch (selection)
    {
    default:
      numberInput();
      break;
    case 5:                         //enter
      changeMenuStep(menustep + 1); // to dolly-distance
      ledOff();
      ledOn('b');
      ledOn('g');
      ledOn('r');
      break;
    }
    break;
  case 12: //dolly - distance
    switch (selection)
    {
    default:
      numberInput();
      break;
    case 5: //enter
      changeMenuStep(menustep + 1);
      break;
    }
    break;
  case 13: //dolly - start and drive
    switch (selection)
    {
    case 4:
      changeMenuStep(10); //back
      break;
    case 5: //enter
            //starts drive
      if (parameters[2] > 0 && parameters[3] > 0)
      {
        unsigned long tempparam3 = parameters[3] * stepsPerCm; //calc cm to steps
        unsigned int tempparam2 = tempparam3 / parameters[2];  //calc rpm(steps per second) from distance(steps)/duration(s)
        moveDolly(parameters[1], tempparam2, tempparam3);
        if (studioMode == true)
        {
          while (!cancel)
          {
            backAndAgain();
          }
        }
        changeMenuStep(70); //to repeatMovement
      }
      else
      {
        changeMenuStep(0);
      }
      break;
    }
    break;

  //Timelapsemenu
  case 30: //timelapse - shots
    switch (selection)
    {
    default:
      numberInput();
      break;
    case 5:
      changeMenuStep(menustep + 1);
      break;
    }
    break;
  case 31: //timelapse - interval
    switch (selection)
    {
    default:
      numberInput();
      break;
    case 5:
      changeMenuStep(menustep + 1);
      break;
    }
    break;
  case 32: //timelapse - distance
    switch (selection)
    {
    default:
      numberInput();
      break;
    case 5:
      changeMenuStep(menustep + 1);
      break;
    }
    break;
  case 33: //timelapse - start and drive
    switch (selection)
    {
    case 4:
      changeMenuStep(30);
      break;
    case 5:
      moveTimelapse(parameters[5], parameters[6] * 1000L, parameters[7] * stepsPerCm);
      //calc interval from seconds to ms AND movesteps needs to be calculatet from cm to steps
      changeMenuStep(0);
      break;
    }
    break;
  case 50: //setup - Sound
    switch (selection)
    {
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
  case 51: //setup - LED
    switch (selection)
    {
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
  case 52: //setup - Ease
    switch (selection)
    {
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
  case 70: //repeatMovement
    switch (selection)
    {
    case 7:
      changeParameter(3);
      break;
    case 9:
      changeParameter(3);
      break;
    case 5:                                  //enter
      if (parameters[lookUp(menustep)] == 0) // no repeat
      {
        changeMenuStep(0);
      }
      if (parameters[lookUp(menustep)] == 1) // Back & Again
      {
        backAndAgain();
      }
      changeMenuStep(menustep);
    }
    if (parameters[lookUp(menustep)] == 2)
    { // Go straight on
      straightOn();
    }
    break;
  }
  break;
}
}
