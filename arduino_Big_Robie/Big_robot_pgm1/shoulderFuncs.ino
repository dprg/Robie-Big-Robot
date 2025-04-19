/***************************
 *
 *  shoulder movement Funcs (Non-Blocking)
 *
 ****************************/

// Global state variables for moveToPose (defined in .h, declared here)
bool isMovingToPose = false;
int poseTargetLift = 0;
int poseTargetRot = 0;
int poseDeadband = DEAD_BAND; // Default deadband
motor *poseLiftMotorPtr = NULL;
motor *poseRotMotorPtr = NULL;

// Initiates the move to a target pose - NON-BLOCKING
void startMoveToPose(int targetLift, int targetRot, int deadband, motor *liftMtr, motor *rotMtr) {
  // Ignore command if already moving to a pose
  if (isMovingToPose) {
      Serial.println("MoveToPose already in progress. Ignoring command.");
      return;
  }
  // Ignore if motors are invalid
  if (!liftMtr || !rotMtr || liftMtr->type != DC_MTR || rotMtr->type != DC_MTR) {
      Serial.println("Invalid motors for MoveToPose. Ignoring command.");
      return;
  }

  // Stop any individual motor steps if they are running (optional, depends on desired behavior)
  // if (liftMtr->state == MOTOR_STEPPING_DC) { analogWrite(liftMtr->enPin, STOP); liftMtr->state = MOTOR_IDLE; }
  // if (rotMtr->state == MOTOR_STEPPING_DC) { analogWrite(rotMtr->enPin, STOP); rotMtr->state = MOTOR_IDLE; }


  poseTargetLift = targetLift;
  poseTargetRot = targetRot;
  poseDeadband = deadband;
  poseLiftMotorPtr = liftMtr;
  poseRotMotorPtr = rotMtr;
  isMovingToPose = true; // Signal that the movement should start/continue

  Serial.print("Starting MoveToPose: Lift=");
  Serial.print(poseTargetLift);
  Serial.print(", Rot=");
  Serial.print(poseTargetRot);
  Serial.print(", DB=");
  Serial.println(poseDeadband);
}

// Called from loop() to manage the moveToPose state machine
void updateMoveToPose() {
  if (!isMovingToPose) {
    return; // Not actively moving to a pose
  }

  // Ensure motor pointers are valid (checked in start, but good practice)
  if (!poseLiftMotorPtr || !poseRotMotorPtr) {
      Serial.println("Error: MoveToPose motor pointers invalid during update.");
      isMovingToPose = false;
      return;
  }

  // Read current positions
  int actualLift = analogRead(poseLiftMotorPtr->potPin);
  int actualRot = analogRead(poseRotMotorPtr->potPin);

  bool liftInPosition = false;
  bool rotInPosition = false;

  // --- Lift Motor Control ---
  // Check if lift motor is IDLE (not currently executing a timed step from this function)
  if (poseLiftMotorPtr->state == MOTOR_IDLE) {
    if (actualLift < (poseTargetLift - poseDeadband)) {
      startDcMotorStep(CCW, NORM_SPD, dcMtrTimeOn, poseLiftMotorPtr); // Use non-blocking step
    } else if (actualLift > (poseTargetLift + poseDeadband)) {
      startDcMotorStep(CW, NORM_SPD, dcMtrTimeOn, poseLiftMotorPtr); // Use non-blocking step
    } else {
      liftInPosition = true; // Lift is within deadband
    }
  } else {
      // Lift motor is currently stepping (MOTOR_STEPPING_DC state).
      // Check if it has reached the target zone anyway.
      if (actualLift >= (poseTargetLift - poseDeadband) && actualLift <= (poseTargetLift + poseDeadband)) {
          // It reached the target while stepping. We could potentially stop it early,
          // but for simplicity, we just mark it as in position and let the timer expire.
          liftInPosition = true;
      }
  }


  // --- Rotation Motor Control ---
  // Check if rotation motor is IDLE
   if (poseRotMotorPtr->state == MOTOR_IDLE) {
    if (actualRot < (poseTargetRot - poseDeadband)) {
      // Assuming CW increases rotation pot value based on original logic
      startDcMotorStep(CW, NORM_SPD, dcMtrTimeOn, poseRotMotorPtr); // Use non-blocking step
    } else if (actualRot > (poseTargetRot + poseDeadband)) {
      // Assuming CCW decreases rotation pot value
      startDcMotorStep(CCW, NORM_SPD, dcMtrTimeOn, poseRotMotorPtr); // Use non-blocking step
    } else {
      rotInPosition = true; // Rotation is within deadband
    }
  } else {
      // Rotation motor is currently stepping. Check if it reached target zone.
       if (actualRot >= (poseTargetRot - poseDeadband) && actualRot <= (poseTargetRot + poseDeadband)) {
          rotInPosition = true;
      }
  }

  // --- Check Completion ---
  if (liftInPosition && rotInPosition) {
    // Ensure motors are stopped if they were stepping into the zone
    if(poseLiftMotorPtr->state == MOTOR_STEPPING_DC) {
        analogWrite(poseLiftMotorPtr->enPin, STOP);
        digitalWrite(poseLiftMotorPtr->in1Pin, LOW);
        digitalWrite(poseLiftMotorPtr->in2Pin, LOW);
        poseLiftMotorPtr->state = MOTOR_IDLE;
    }
     if(poseRotMotorPtr->state == MOTOR_STEPPING_DC) {
        analogWrite(poseRotMotorPtr->enPin, STOP);
        digitalWrite(poseRotMotorPtr->in1Pin, LOW);
        digitalWrite(poseRotMotorPtr->in2Pin, LOW);
        poseRotMotorPtr->state = MOTOR_IDLE;
    }

    isMovingToPose = false; // Target pose reached
    Serial.print("MoveToPose Complete. Final Pos: Lift=");
    Serial.print(actualLift);
    Serial.print(", Rot=");
    Serial.println(actualRot);
  } else {
     // Optional: Print status during movement
     // Serial.print("Moving: L:"); Serial.print(actualLift); Serial.print("->"); Serial.print(poseTargetLift);
     // Serial.print(" R:"); Serial.print(actualRot); Serial.print("->"); Serial.print(poseTargetRot);
     // Serial.print(" States(L/R):"); Serial.print(poseLiftMotorPtr->state); Serial.print("/"); Serial.println(poseRotMotorPtr->state);
     // delay(10); // Avoid flooding serial
  }
}

// Removed old blocking moveToPose function
