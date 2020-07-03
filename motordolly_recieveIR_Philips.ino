int recieveIR(int waitTime)
{ //Function to recieve IR-Codes from the Remote
  if (irrec.decode(&recieved))
  {
    Serial.println(recieved.value);
    delay(waitTime);
    irrec.resume();

    switch (recieved.value)
    {
    case 1642233374: //key on Remote: power
      reset();
      return (1);
      break;
    case 3680795202: //key on Remote: vol +
      return (2);
      break;
    case 1336001886:     //key on Remote: mute
      parameters[9] = 1; // muting sound
      return (3);
      break;
    case 2197058977: //key on Remote: left
      return (4);
      break;
    case 2076862012: //key on Remote: enter
      return (5);
      break;
    case 1341079762: //key on Remote: enter
      return (5);
      break;
    case 1516242620: //key on Remote: right
      return (6);
      break;
    case 1542584743: //key on Remote: down
      return (7);
      break;
    case 4217290035: //key on Remote: vol-
      return (8);
      break;
    case 1400868310: //key on Remote: up
      return (9);
      break;
    case 1845395494: //key on Remote: 0
      return (10);
      break;
    case 2799873664: //key on Remote: 0
      return (10);
      break;
    case 909022751:      //key on Remote: VIEW
      switchBacklight(); // switch Backlight from LCD
      return (11);
      break;
    case 1828147625:     //key on Remote: VIEW
      switchBacklight(); // switch Backlight from LCD
      return (11);
      break;
    case 1785606946: //key on Remote: Music Follows Me
      return (12);
      break;
    case 3217609334: //key on Remote: 1
      return (13);
      break;
    case 2179774928: //key on Remote: 2
      return (14);
      break;
    case 3763127690: //key on Remote: 3
      return (15);
      break;
    case 3556927966: //key on Remote: 4
      return (16);
      break;
    case 548345768: //key on Remote: 5
      return (17);
      break;
    case 557442243: //key on Remote: 6
      return (18);
      break;
    case 3463053182: //key on Remote: 7
      return (19);
      break;
    case 3811405335: //key on Remote: 8
      return (20);
      break;
    case 1878480480: //key on Remote: 9
      return (21);
      break;
    }
  }
  else
  {
    return (0);
  }
}
