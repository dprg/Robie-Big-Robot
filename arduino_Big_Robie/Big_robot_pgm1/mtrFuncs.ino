/*************************************************
 *
 * motor related funcs
 *
 *************************************************/
void step(motor *mtr)
{
	for (int j = 0; j <= 5; j++)
	{	// 4.3 * 200 = 860 or 1 rev (~2.4 per deg)
		// check limit switches
		chkLimitSwState(ptrPanMtr);

		if (mtr->dir == CW)
		{
			mtr->seqNum++;
			if (mtr->seqNum > 3)
				mtr->seqNum = 0;
		}
		if (mtr->dir == CCW)
		{
			mtr->seqNum--;
			if (mtr->seqNum < 0)
				mtr->seqNum = 3;
		}
		if ((mtr->type == UNIPOLAR))
		{ // && (mtr->dirChgFlg != ESTOP)){  // don't move if limitStateFlg is tripped
			switch (mtr->seqNum)
			{
			case 0:
				digitalWrite(mtr->in1Pin, HIGH);
				digitalWrite(mtr->in2Pin, HIGH);
				digitalWrite(mtr->in3Pin, LOW);
				digitalWrite(mtr->in4Pin, LOW);
				break;
			case 1:
				digitalWrite(mtr->in1Pin, LOW);
				digitalWrite(mtr->in2Pin, HIGH);
				digitalWrite(mtr->in3Pin, HIGH);
				digitalWrite(mtr->in4Pin, LOW);
				break;
			case 2:
				digitalWrite(mtr->in1Pin, LOW);
				digitalWrite(mtr->in2Pin, LOW);
				digitalWrite(mtr->in3Pin, HIGH);
				digitalWrite(mtr->in4Pin, HIGH);
				break;
			case 3:
				digitalWrite(mtr->in1Pin, HIGH);
				digitalWrite(mtr->in2Pin, LOW);
				digitalWrite(mtr->in3Pin, LOW);
				digitalWrite(mtr->in4Pin, HIGH);
				break;
			// invalid seqNum (stop motor)
			default:
				digitalWrite(mtr->in1Pin, LOW);
				digitalWrite(mtr->in2Pin, LOW);
				digitalWrite(mtr->in3Pin, LOW);
				digitalWrite(mtr->in4Pin, LOW);
			}
			delay(mtr->delayBetweenSteps);
		}
	}
}

void setMotor(int direction, int speed, motor *mtr)
{
	mtr->dir = direction;
	mtr->speed = speed;
	if (mtr->dirChgFlg == ESTOP)
		mtr->speed = STOP;

	if ((mtr->type == UNIPOLAR) || (mtr->type == BIPOLAR))
		step(mtr);

	if (mtr->type == DC_MTR)
	{
		if (mtr->dir == CW)
		{
			digitalWrite(mtr->in1Pin, LOW);
			digitalWrite(mtr->in2Pin, HIGH);
			analogWrite(mtr->enPin, mtr->speed);
		}
		if (mtr->dir == CCW)
		{
			digitalWrite(mtr->in1Pin, HIGH);
			digitalWrite(mtr->in2Pin, LOW);
			analogWrite(mtr->enPin, mtr->speed);
		}
		if (mtr->dir == SHUTDOWN)
		{
			digitalWrite(mtr->in1Pin, LOW);
			digitalWrite(mtr->in2Pin, LOW);
			analogWrite(mtr->enPin, mtr->speed);
		}
	}
}

void chkLimitSwState(motor *mtr)
{
	int switchCW = digitalRead(mtr->limitCWpin);
	int switchCCW = digitalRead(mtr->limitCCWpin);

	switch (mtr->limitSwType)
	{
	case NONE:
		// nothing to do here...
		break;
	case NC_SW: //  assumes normally closed limit switch at both limits
		if (switchCW == LOW)
			mtr->limitCWstate = NOT_TRIPPED;
		else
		{
			mtr->limitCWstate = TRIPPED;
			mtr->dir = CCW;
		}
		if (switchCCW == LOW)
			mtr->limitCCWstate = NOT_TRIPPED;
		else
		{
			mtr->limitCCWstate = TRIPPED;
			mtr->dir = CW;
		}
		break;
	case NO_SW: //  assumes normally open limit switch at both limits
		if (switchCW == HIGH)
			mtr->limitCWstate = NOT_TRIPPED;
		else
		{
			mtr->limitCWstate = TRIPPED;
			mtr->dir = CCW;
		}
		if (switchCCW == HIGH)
			mtr->limitCCWstate = NOT_TRIPPED;
		else
		{
			mtr->limitCCWstate = TRIPPED;
			mtr->dir = CW;
		}
		break;
	case OPTO: //  assumes a single opto interrupter type limit switch
		if (switchCW == HIGH)
		{									 //  Opto interrupter senses a disk with slots at both limits
			mtr->limitCWstate = NOT_TRIPPED; //  (only limitCWstate used)
			if (mtr->dir == CW)				 //  While not tripped point at the opposit direction so
				mtr->dirChgFlg = CCW;		 //  when tripped, you can drive out of an overshot limit switch
			else							 //  condition. note: limitCWpin is used for open collector opto xtr
				mtr->dirChgFlg = CW;		 //  limitCCWpin is used for anode of IR diode
		}
		else
		{
			mtr->limitCWstate = TRIPPED;
			mtr->dir = mtr->dirChgFlg;
		}
		break;
	// bad limit switch argument
	default:
		mtr->speed = STOP;
		mtr->dirChgFlg = ESTOP;
	}
}
void dcMtrTakeStep(int direction, int speed, int timeOn, motor *mtr)
{
	mtr->dir = direction;
	mtr->speed = speed;
	if (mtr->dirChgFlg == ESTOP)
		mtr->speed = STOP;

	if (mtr->dir == CW)
	{
		digitalWrite(mtr->in1Pin, LOW);
		digitalWrite(mtr->in2Pin, HIGH);
		analogWrite(mtr->enPin, mtr->speed);
		delay(timeOn);
		digitalWrite(mtr->enPin, STOP);
	}
	if (mtr->dir == CCW)
	{
		digitalWrite(mtr->in1Pin, HIGH);
		digitalWrite(mtr->in2Pin, LOW);
		analogWrite(mtr->enPin, mtr->speed);
		delay(timeOn);
		digitalWrite(mtr->enPin, STOP);
	}
}
