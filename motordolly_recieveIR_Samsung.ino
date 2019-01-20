int recieveIR(int waitTime) {     //Function to recieve IR-Codes from the Remote
  if (irrec.decode(&recieved)) {
    delay(waitTime);
    irrec.resume();

    switch (recieved.value) {
      case 3772793023: //key on Remote: power
        cancel = true;
        reset();
        return (1);
        break;
      case 3772833823: //key on Remote: vol +
        return (2);
        break;
      case 3772837903: //key on Remote: mute
        return (3);
        break;
      case 3772819033: //key on Remote: left
        return (4);
        break;
      case 3772782313: //key on Remote: enter
        return (5);
        break;
      case 3772794553: //key on Remote: right
        return (6);
        break;
      case 3772810873: //key on Remote: down
        return (7);
        break;
      case 3772829743: //key on Remote: vol-
        return (8);
        break;
      case 3772778233: //key on Remote: up
        return (9);
        break;
      case 3772811383: //key on Remote: 0
        return (10);
        break;
      case 3772789963: //key on Remote: TTX
        return (11);
        break;
      case 3772827703: //key on Remote: PRE-CH
        return (12);
        break;
      case 3772784863: //key on Remote: 1
        return (13);
        break;
      case 3772817503: //key on Remote: 2
        return (14);
        break;
      case 3772801183: //key on Remote: 3
        return (15);
        break;
      case 3772780783: //key on Remote: 4
        return (16);
        break;
      case 3772813423: //key on Remote: 5
        return (17);
        break;
      case 3772797103: //key on Remote: 6
        return (18);
        break;
      case 3772788943: //key on Remote: 7
        return (19);
        break;
      case 3772821583: //key on Remote: 8
        return (20);
        break;
      case 3772805263: //key on Remote: 9
        return (21);
        break;
    }
  }
  else {
    return (0);
  }
}
