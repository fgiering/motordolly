void ledOn(char color)
{ //function to turn on an LED Color with Parameter 1- which color (r,g,b)
  if (parameters[9] == 0)
  {
    switch (color)
    {
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

void ledOff()
{
  analogWrite(LEDb, 0);
  analogWrite(LEDg, 0);
  analogWrite(LEDr, 0);
}
