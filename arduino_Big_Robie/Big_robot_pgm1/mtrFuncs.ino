/*************************************************
 *
 * motor related funcs (Non-Blocking)
 *
 *************************************************/

// --- Stepper Motor Control (Pan) ---
const int STEPPER_MICROSTEPS_PER_COMMAND = 6; // Number of micro-steps per L/R command (was loop j=0 to 5)

// Starts a non-blocking sequence of micro-steps for the stepper motor
void startStepperStep(int direction, motor *mtr) {
    // Ignore if motor is already stepping or estopped
    if (mtr->state != MOTOR_IDLE || mtr->dirChgFlg == ESTOP || mtr->type != UNIPOLAR) {
        Serial.println("Stepper busy or invalid type/state. Ignoring command.");
        return;
    }

    // Check limit switches before starting
    chkLimitSwState(mtr); // This function might change mtr->dir
    // Re-check direction after limit switch check
    if ((direction == CW && mtr->limitCWstate == TRIPPED) ||
        (direction == CCW && mtr->limitCCWstate == TRIPPED)) {
        Serial.println("Stepper limit hit. Ignoring command.");
        return; // Don't start if already at limit in the requested direction
    }

    mtr->dir = direction; // Set the intended direction
    mtr->state = MOTOR_STEPPING_MICROSTEP;
    mtr->microStepsRemaining = STEPPER_MICROSTEPS_PER_COMMAND;
    mtr->stepTargetMillis = millis(); // Start first micro-step immediately

    Serial.print("Starting Stepper Step: Dir=");
    Serial.println(mtr->dir);
}

// Called from loop() to manage stepper motor micro-steps
void updateStepperMotor() {
    motor* mtr = ptrPanMtr; // Assuming only pan motor is stepper for now

    if (mtr->state != MOTOR_STEPPING_MICROSTEP) {
        return; // Not currently stepping
    }

    unsigned long currentTime = millis();
    if (currentTime >= mtr->stepTargetMillis) {
        // Time for the next micro-step

        // Check limit switches during stepping
        chkLimitSwState(mtr);
        if ((mtr->dir == CW && mtr->limitCWstate == TRIPPED) ||
            (mtr->dir == CCW && mtr->limitCCWstate == TRIPPED)) {
            Serial.println("Stepper limit hit during step sequence. Stopping.");
            // Turn off coils
            digitalWrite(mtr->in1Pin, LOW);
            digitalWrite(mtr->in2Pin, LOW);
            digitalWrite(mtr->in3Pin, LOW);
            digitalWrite(mtr->in4Pin, LOW);
            mtr->state = MOTOR_IDLE;
            mtr->microStepsRemaining = 0;
            return;
        }

        // Advance sequence number based on direction
        if (mtr->dir == CW) {
            mtr->seqNum++;
            if (mtr->seqNum > 3) mtr->seqNum = 0;
        } else { // CCW
            mtr->seqNum--;
            if (mtr->seqNum < 0) mtr->seqNum = 3;
        }

        // Set coils based on sequence number
        switch (mtr->seqNum) {
            case 0:
                digitalWrite(mtr->in1Pin, HIGH); digitalWrite(mtr->in2Pin, HIGH);
                digitalWrite(mtr->in3Pin, LOW); digitalWrite(mtr->in4Pin, LOW);
                break;
            case 1:
                digitalWrite(mtr->in1Pin, LOW); digitalWrite(mtr->in2Pin, HIGH);
                digitalWrite(mtr->in3Pin, HIGH); digitalWrite(mtr->in4Pin, LOW);
                break;
            case 2:
                digitalWrite(mtr->in1Pin, LOW); digitalWrite(mtr->in2Pin, LOW);
                digitalWrite(mtr->in3Pin, HIGH); digitalWrite(mtr->in4Pin, HIGH);
                break;
            case 3:
                digitalWrite(mtr->in1Pin, HIGH); digitalWrite(mtr->in2Pin, LOW);
                digitalWrite(mtr->in3Pin, LOW); digitalWrite(mtr->in4Pin, HIGH);
                break;
        }

        mtr->microStepsRemaining--;

        if (mtr->microStepsRemaining <= 0) {
            // Finished all micro-steps for this command
            // Optionally turn off coils to save power/reduce heat,
            // but stepper might not hold position as strongly.
            // digitalWrite(mtr->in1Pin, LOW); digitalWrite(mtr->in2Pin, LOW);
            // digitalWrite(mtr->in3Pin, LOW); digitalWrite(mtr->in4Pin, LOW);
            mtr->state = MOTOR_IDLE;
            Serial.println("Stepper Step Sequence Complete.");
        } else {
            // Schedule the next micro-step
            mtr->stepTargetMillis = currentTime + mtr->delayBetweenSteps;
        }
    }
}


// --- DC Motor Control (Tilt, Shoulders) ---

// Starts a timed step for a DC motor - NON-BLOCKING
void startDcMotorStep(int direction, int speed, int timeOn, motor *mtr)
{
	// Only start if motor is idle and not ESTOPPED and is DC type
	if (mtr->state != MOTOR_IDLE || mtr->dirChgFlg == ESTOP || mtr->type != DC_MTR) {
    Serial.println("DC Motor busy or invalid type/state. Ignoring command.");
		return;
	}

	mtr->dir = direction;
	mtr->speed = speed; // Speed is used directly by analogWrite

	if (mtr->dir == CW)
	{
		digitalWrite(mtr->in1Pin, LOW);
		digitalWrite(mtr->in2Pin, HIGH);
		analogWrite(mtr->enPin, mtr->speed);
		mtr->state = MOTOR_STEPPING_DC;
		mtr->stepTargetMillis = millis() + timeOn;
    Serial.print("Starting DC Step: Dir=CW, Time="); Serial.println(timeOn);
	}
	else if (mtr->dir == CCW)
	{
		digitalWrite(mtr->in1Pin, HIGH);
		digitalWrite(mtr->in2Pin, LOW);
		analogWrite(mtr->enPin, mtr->speed);
		mtr->state = MOTOR_STEPPING_DC;
		mtr->stepTargetMillis = millis() + timeOn;
    Serial.print("Starting DC Step: Dir=CCW, Time="); Serial.println(timeOn);
	}
	// If direction is not CW or CCW, do nothing, motor remains IDLE
}

// Called from loop() to manage timed DC motor steps
void updateDcMotors() {
  // Check motors that use timed steps (Tilt, Shoulders)
  // Note: Ensure these pointers are correctly initialized in setup()
  motor* motorsToCheck[] = {ptrTiltMtr, ptrRightShoulderLiftMtr, ptrRightShoulderRotMtr};
  int numMotors = sizeof(motorsToCheck) / sizeof(motorsToCheck[0]);

  unsigned long currentTime = millis();

  for (int i = 0; i < numMotors; i++) {
    motor* mtr = motorsToCheck[i];
    if (mtr != NULL && mtr->state == MOTOR_STEPPING_DC) { // Check pointer validity
      if (currentTime >= mtr->stepTargetMillis) {
        // Time's up, stop the motor
        analogWrite(mtr->enPin, STOP); // Use analogWrite for PWM control
        digitalWrite(mtr->in1Pin, LOW); // Ensure motor pins are low for brake/coast
        digitalWrite(mtr->in2Pin, LOW);
        mtr->state = MOTOR_IDLE;
        Serial.println("DC Step Complete.");
      }
    }
  }
}


// --- Common Motor Functions ---

// Checks limit switches and updates motor state if tripped
// Returns true if a limit was hit, false otherwise
bool chkLimitSwState(motor *mtr)
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
    return true; // Limit hit
	}
  return false; // No limit hit
}

// Removed old blocking dcMtrTakeStep function
// Removed old blocking step function (integrated into updateStepperMotor)
// Removed old setMotor function (replaced by startStepperStep/startDcMotorStep)
