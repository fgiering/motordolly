/* int recieveIR(int waitTime) {     //Function to recieve IR-Codes from the Remote
  if (irrec.decode(&recieved)) {
    Serial.println(recieved.value, DEC);
    delay(waitTime);
    irrec.resume();

    switch (recieved.value) {
      case 16753245: //key on Remote: power
        cancel = true;
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
      case 16712445: //key on Remote: > (Play)
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
    }
  }
  else {
    return (0);
  }
} */
