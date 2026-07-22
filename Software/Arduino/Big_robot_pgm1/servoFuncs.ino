/*************************************************
 *
 * servo related funcs (Non-Blocking)
 *
 *************************************************/

// --- Configuration ---
const int WAVE_REPETITIONS = 2;
const int WAVE_STEP_DEGREES = 2;
const unsigned long WAVE_STEP_DELAY_MS = 15;
const int WAVE_MIN_POS = 0;
const int WAVE_MAX_POS = 180;
const int WAVE_RIGHT_PARK_POS = 93; // Neutral position for continuous servo

// --- Global State Variables (defined in .h, declared here) ---
WaveState waveLeftState = WAVE_IDLE;
unsigned long waveLeftNextStepMillis = 0;
int waveLeftCurrentPos = 0; // Start at min position
int waveLeftRepetitions = 0;

WaveState waveRightState = WAVE_IDLE;
unsigned long waveRightNextStepMillis = 0;
int waveRightCurrentPos = 0; // Start at min position
int waveRightRepetitions = 0;


// --- Left Wave (Standard Servo) ---

void startWaveLeft() {
  // Ignore command if already waving
  if (waveLeftState != WAVE_IDLE) {
    Serial.println("Left wave already in progress. Ignoring command.");
    return;
  }
  Serial.println("Starting Left Wave...");
  waveLeftRepetitions = WAVE_REPETITIONS;
  waveLeftCurrentPos = WAVE_MIN_POS; // Ensure starting position
  waveServo.write(waveLeftCurrentPos); // Go to start position
  waveLeftState = WAVE_UP; // Start by moving up
  waveLeftNextStepMillis = millis() + WAVE_STEP_DELAY_MS; // Schedule first step
}

void updateWaveLeft() {
  if (waveLeftState == WAVE_IDLE) {
    return; // Nothing to do
  }

  unsigned long currentTime = millis();
  if (currentTime >= waveLeftNextStepMillis) {
    // Time for the next step

    if (waveLeftState == WAVE_UP) {
      waveLeftCurrentPos += WAVE_STEP_DEGREES;
      if (waveLeftCurrentPos >= WAVE_MAX_POS) {
        waveLeftCurrentPos = WAVE_MAX_POS; // Clamp to max
        waveLeftState = WAVE_DOWN; // Change direction
      }
      waveServo.write(waveLeftCurrentPos);
      waveLeftNextStepMillis = currentTime + WAVE_STEP_DELAY_MS; // Schedule next step
    }
    else if (waveLeftState == WAVE_DOWN) {
      waveLeftCurrentPos -= WAVE_STEP_DEGREES;
      if (waveLeftCurrentPos <= WAVE_MIN_POS) {
        waveLeftCurrentPos = WAVE_MIN_POS; // Clamp to min
        waveLeftRepetitions--; // Completed one full cycle (up and down)
        if (waveLeftRepetitions <= 0) {
          waveLeftState = WAVE_IDLE; // Finished all repetitions
          Serial.println("Left Wave Complete.");
          // No need to park standard servo, it holds position
          return; // Exit early, no more steps scheduled
        } else {
          waveLeftState = WAVE_UP; // Start next repetition
        }
      }
      waveServo.write(waveLeftCurrentPos);
      waveLeftNextStepMillis = currentTime + WAVE_STEP_DELAY_MS; // Schedule next step
    }
  }
}


// --- Right Wave (Continuous Rotation Servo) ---

void startWaveRight() {
  // Ignore command if already waving
  if (waveRightState != WAVE_IDLE) {
     Serial.println("Right wave already in progress. Ignoring command.");
    return;
  }
  Serial.println("Starting Right Wave...");
  waveRightRepetitions = WAVE_REPETITIONS;
  waveRightCurrentPos = WAVE_MIN_POS; // Start at one end of the "sweep"
  waveServo_right.write(waveRightCurrentPos); // Go to start position
  waveRightState = WAVE_UP; // Start by moving "up" (increasing position value)
  waveRightNextStepMillis = millis() + WAVE_STEP_DELAY_MS; // Schedule first step
}

void updateWaveRight() {
  if (waveRightState == WAVE_IDLE) {
    return; // Nothing to do
  }

  unsigned long currentTime = millis();
  if (currentTime >= waveRightNextStepMillis) {
    // Time for the next step

    if (waveRightState == WAVE_UP) {
      waveRightCurrentPos += WAVE_STEP_DEGREES;
      if (waveRightCurrentPos >= WAVE_MAX_POS) {
        waveRightCurrentPos = WAVE_MAX_POS; // Clamp to max
        waveRightState = WAVE_DOWN; // Change direction
      }
      waveServo_right.write(waveRightCurrentPos);
      waveRightNextStepMillis = currentTime + WAVE_STEP_DELAY_MS; // Schedule next step
    }
    else if (waveRightState == WAVE_DOWN) {
      waveRightCurrentPos -= WAVE_STEP_DEGREES;
      if (waveRightCurrentPos <= WAVE_MIN_POS) {
        waveRightCurrentPos = WAVE_MIN_POS; // Clamp to min
        waveRightRepetitions--; // Completed one full cycle (up and down)
        if (waveRightRepetitions <= 0) {
          waveServo_right.write(WAVE_RIGHT_PARK_POS); // Park the continuous servo
          waveRightState = WAVE_IDLE; // Finished all repetitions
          Serial.println("Right Wave Complete.");
          return; // Exit early, no more steps scheduled
        } else {
          waveRightState = WAVE_UP; // Start next repetition
        }
      }
      waveServo_right.write(waveRightCurrentPos);
      waveRightNextStepMillis = currentTime + WAVE_STEP_DELAY_MS; // Schedule next step
    }
  }
}
