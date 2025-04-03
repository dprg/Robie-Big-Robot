
/***************************
 *
 *  shoulder movement Funcs
 *
 ****************************/

int moveToPose(int targetLift, int targetRot, int deadband, motor *liftMtr, motor *rotMtr)
{
	int actualLift = 0;
	int actualRot = 0;
	int lift = 0;
	int rot = 0;
	int inPos = 0;
	while (inPos != 2)
	{
		// read pots
		actualLift = analogRead(liftMtr->potPin);
		actualRot = analogRead(rotMtr->potPin);
		// move lift mtr towards target
		if (actualLift < (targetLift - deadband))
			dcMtrTakeStep(CCW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderLiftMtr);
		else if (actualLift > (targetLift + deadband))
			dcMtrTakeStep(CW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderLiftMtr);
		else // good enough
			lift = 1;

		// move rotation towards target
		if (actualRot < (targetRot - deadband))
			dcMtrTakeStep(CW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderRotMtr);
		else if (actualRot > (targetRot + deadband))
			dcMtrTakeStep(CCW, NORM_SPD, dcMtrTimeOn, ptrRightShoulderRotMtr);
		else // good enough
			rot = 1;

		Serial.print(actualLift);
		Serial.print(',');
		Serial.print(actualRot);
		Serial.print('\n');

		inPos = lift + rot;
	}
	return inPos;
}
