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
		case 'L': // turn pan motor left one step (Non-Blocking)
      // Check handled in loop() before calling parse()
			startStepperStep(CW, ptrPanMtr);
			// Serial.println("L"); // Output now in start/update funcs
			break;
		case 'R': // turn pan motor right one step (Non-Blocking)
      // Check handled in loop() before calling parse()
			startStepperStep(CCW, ptrPanMtr);
			// Serial.println("R"); // Output now in start/update funcs
			break;
		case 'U': // turn tilt motor upward one step (Non-Blocking)
      // Check handled in loop() before calling parse()
			startDcMotorStep(CCW, TILT_UP_SPD, dcMtrTimeOn, ptrTiltMtr);
			// Serial.println("U"); // Output now in start/update funcs
			break;
		case 'D': // turn tilt motor downward one step (Non-Blocking)
      // Check handled in loop() before calling parse()
			startDcMotorStep(CW, TILT_DWN_SPD, dcMtrTimeOn, ptrTiltMtr);
			// Serial.println("D"); // Output now in start/update funcs
			break;
		case '1': // Shoulder Lift Up (CCW) - Non-blocking step
      // Check handled in loop() before calling parse()
			startDcMotorStep(CCW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderLiftMtr);
			// Pot readings are meaningless here as step hasn't finished
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);
			// Serial.print('\n');
			// Serial.println("1"); // Output now in start/update funcs
			break;
		case '2': // Shoulder Rotate More (CCW?) - Non-blocking step
      // Check handled in loop() before calling parse()
			startDcMotorStep(CCW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderRotMtr);
			// Pot readings are meaningless here
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);
			// Serial.print('\n');
			// Serial.println("2"); // Output now in start/update funcs
			break;
		case '3': // Shoulder Lift Down (CW) - Non-blocking step
      // Check handled in loop() before calling parse()
			startDcMotorStep(CW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderLiftMtr);
      // Pot readings are meaningless here
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);
			// Serial.print('\n');
			// Serial.println("3"); // Output now in start/update funcs
			break;
		case '4': // Shoulder Rotate Less (CW?) - Non-blocking step
      // Check handled in loop() before calling parse()
			startDcMotorStep(CW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderRotMtr);
      // Pot readings are meaningless here
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);
			// Serial.print('\n');
			// Serial.println("4"); // Output now in start/update funcs
			break;
		case '5': // Start moving to pose 1 (Non-Blocking)
      // Check handled in loop() before calling parse()
			startMoveToPose(104, 872, DEAD_BAND, ptrRightShoulderLiftMtr, ptrRightShoulderRotMtr);
			// Status printing is now handled within start/updateMoveToPose()
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);
			// Serial.print('\n');
			// Serial.println("5"); // Output now in start/update funcs
			break;
		case '6': // Start moving to pose 2 (Non-Blocking)
      // Check handled in loop() before calling parse()
			startMoveToPose(103, 556, DEAD_BAND, ptrRightShoulderLiftMtr, ptrRightShoulderRotMtr);
			// Status printing is now handled within start/updateMoveToPose()
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);
			// Serial.print('\n');
			// Serial.println("6"); // Output now in start/update funcs
			break;
		case '7': // Start moving to pose 3 (Non-Blocking)
      // Check handled in loop() before calling parse()
			startMoveToPose(225, 552, DEAD_BAND, ptrRightShoulderLiftMtr, ptrRightShoulderRotMtr);
			// Status printing is now handled within start/updateMoveToPose()
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// Serial.print(temp);
			// Serial.print('\n');
			// Serial.println("7"); // Output now in start/update funcs
			break;
		case '8': // Start moving to pose 4 (Non-Blocking)
      // Check handled in loop() before calling parse()
			startMoveToPose(373, 543, DEAD_BAND, ptrRightShoulderLiftMtr, ptrRightShoulderRotMtr);
			// Status printing is now handled within start/updateMoveToPose()
			// temp = analogRead(POT_LIFT);
			// Serial.print(temp);
			// Serial.print(',');
			// temp = analogRead(POT_ROT);
			// erial.print(temp);
			// erial.print(temp);
			// Serial.print('\n');
			Serial.println("8"); // Command confirmation
			break;
		case 'W': // Start Left Wave (Non-Blocking)
      // Busy check handled in loop() before calling parse()
			startWaveLeft();
			// Serial output now handled within start/update functions
			break;
			// case 'X':   // complex cmds
			// Serial.println("X");
			// temp = parseXcmd(j,cmdLen);
			// for (int k = j; k = temp; k++)
			//     inputString[k] = '\0';
			// j = j + (temp);
			//   break;
		case 'E': // Start Right Wave (Non-Blocking)
      // Busy check handled in loop() before calling parse()
			startWaveRight();
      // Serial output now handled within start/update functions
			break;
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
