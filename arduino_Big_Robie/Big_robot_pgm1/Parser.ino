/************************************************
 *
 * Parser
 *
 ************************************************/
void parse(void)
{
	String cmdLine = inputString;
	// cmdLine.toUpperCase();
	int cmdLen = cmdLine.length();
	char cmd = '0';

	int j = 0;
	while (j < cmdLen)
	{
		// for (int j = 0 ; j < cmdLen; j++) {
		// String cmd = cmdLine.substring(j,(j + 1));
		if ((inputString[j] >= 97) && (inputString[j] <= 122)) // char is lower case
			inputString[j] -= 32;							   // change to upper case

		cmd = inputString[j];

		int temp = 0;
		switch (cmd)
		{
		case 'L': // turn pan motor left one step
			setMotor(CW, DUMMY, ptrPanMtr);
			Serial.println("L");
			break;
		case 'R': // turn pan motor right one step
			setMotor(CCW, DUMMY, ptrPanMtr);
			Serial.println("R");
			break;
		case 'U': // turn tilt motor upward one step
			dcMtrTakeStep(CCW, TILT_UP_SPD, dcMtrTimeOn, ptrTiltMtr);
			Serial.println("U");
			break;
		case 'D': // turn tilt motor downward one step
			dcMtrTakeStep(CW, TILT_DWN_SPD, dcMtrTimeOn, ptrTiltMtr);
			Serial.println("D");
			break;
		case '1':
			dcMtrTakeStep(CCW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderLiftMtr);
			temp = analogRead(POT_LIFT);
			Serial.print(temp);
			Serial.print(',');
			temp = analogRead(POT_ROT);
			Serial.print(temp);
			Serial.print('\n');
			Serial.println("1");
			break;
		case '2': // toral down
			dcMtrTakeStep(CCW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderRotMtr);
			temp = analogRead(POT_LIFT);
			Serial.print(temp);
			Serial.print(',');
			temp = analogRead(POT_ROT);
			Serial.print(temp);
			Serial.print('\n');
			Serial.println("2");
			break;
		case '3':
			dcMtrTakeStep(CW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderLiftMtr);
			temp = analogRead(POT_LIFT);
			Serial.print(temp);
			Serial.print(',');
			temp = analogRead(POT_ROT);
			Serial.print(temp);
			Serial.print('\n');
			Serial.println("3");
			break;
		case '4':
			dcMtrTakeStep(CW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderRotMtr);
			temp = analogRead(POT_LIFT);
			Serial.print(temp);
			Serial.print(',');
			temp = analogRead(POT_ROT);
			Serial.print(temp);
			Serial.print('\n');
			Serial.println("4");
			break;
		case '5':
			moveToPose(342, 550, DEAD_BAND, ptrRightShoulderLiftMtr, ptrRightShoulderRotMtr); // 500,500
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);8
			// Serial.print('\n');
			Serial.println("5");
			break;
		case '6':
			moveToPose(275, 540, DEAD_BAND, ptrRightShoulderLiftMtr, ptrRightShoulderRotMtr); // 200,560
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);
			// Serial.print('\n');
			Serial.println("6");
			break;
		case '7':
			moveToPose(600, 875, DEAD_BAND, ptrRightShoulderLiftMtr, ptrRightShoulderRotMtr); // 770,900
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);
			// Serial.print('\n');
			Serial.println("7");
			break;
		case '8':
			moveToPose(600, 225, DEAD_BAND, ptrRightShoulderLiftMtr, ptrRightShoulderRotMtr); // 770, 250
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// erial.print(temp);
			// Serial.print('\n');
			Serial.println("8");
			break;
		case 'W':
			wave();
			Serial.println("W");
			break;
			// case 'X':   // complex cmds
			// Serial.println("X");
			// temp = parseXcmd(j,cmdLen);
			// for (int k = j; k = temp; k++)
			//     inputString[k] = '\0';
			// j = j + (temp);
			//   break;
		default:
			Serial.print(cmd);
			Serial.println(" - bad cmd");
		}
		// inputString[j] = '\0';   // re-initialize char
		j++;
	}

	j = 0;
	while (j <= cmdLen)
	{
		inputString[j] = '\0'; // re-initialize char
		j++;
	}
}
