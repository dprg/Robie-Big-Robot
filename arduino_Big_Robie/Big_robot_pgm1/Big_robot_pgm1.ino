/* pan/tilt demostration
 * ---------------------
 * Head moves from limit swith to limit switch for both
 * Pan / tilt motors.
 *
 * Pan is driven by a unipolar 5 wire stepper motor
 * using a ULN2003 motor driver board.
 * Tilt is driven by a DC gearhead motor using
 * an L298 motor driver board.
 *
 * Doug Paradis copyright 2014
 *
 */

#include "Big_robot_pgm1.h"
#include <Stream.h>
#include <Servo.h>

// Pin definitions
const int LED = 13;
const int BUTTON = 12;
const int POT_LIFT = A2; // Pot 1
const int POT_ROT = A3;  // Pot 2

// Serial setup stuff
const int bufLen = 36;    // length of inputString buffer
char inputString[bufLen]; // a string (buffer) to hold incoming data
int baudRate = 9600;      // baud rate

// servo objects
Servo waveServo; // create servo object to control left wrist servo
Servo waveServo_right; // create servo object to control left wrist servo
                 // a maximum of eight servo objects can be created

// define motors
motor tiltMtr;
motor panMtr;
motor rightShoulderLiftMtr;
motor rightShoulderRotMtr;
motor *ptrTiltMtr = &tiltMtr;
motor *ptrPanMtr = &panMtr;
motor *ptrRightShoulderLiftMtr = &rightShoulderLiftMtr;
motor *ptrRightShoulderRotMtr = &rightShoulderRotMtr;

// define delays
int dcMtrTimeOn = 40;

void setup()
{
  initialize(); // Assumes initialize() sets up pins, serial, servos etc.

  // Initialize motor states (assuming previous refactor is applied)
  // ptrTiltMtr->state = MOTOR_IDLE;
  // ptrPanMtr->state = MOTOR_IDLE;
  // ptrRightShoulderLiftMtr->state = MOTOR_IDLE;
  // ptrRightShoulderRotMtr->state = MOTOR_IDLE;
  // isMovingToPose = false;

  // Initialize Wave states
  waveLeftState = WAVE_IDLE;
  waveRightState = WAVE_IDLE;

  // set starting direction (if needed)
  // ptrTiltMtr->dir = CW;
  ptrTiltMtr->speed = NORM_SPD;
  ptrPanMtr->dir = CW;
}

void loop()
{
  // if button pushed, stop all motors
  // note: ESTOP should be when button goes high - fix when estop button added
  //       Also ESTOP should be power not microcontrolle
  // if (digitalRead(BUTTON) == LOW) {
  // ptrTiltMtr->dirChgFlg = ESTOP;
  // ptrPanMtr->dirChgFlg = ESTOP;
  //   digitalWrite(LED, HIGH);
  //}

  // --- Update State Machines ---
  // updateMotors();     // From previous refactor
  // updateMoveToPose(); // From previous refactor
  updateWaveLeft();   // Update left wave state machine
  updateWaveRight();  // Update right wave state machine

  // --- Check for Input ---
  int cmdLen = 0;
  cmdLen = readCmdLine(); // Check if a new command has arrived
  if (cmdLen > 0) {
    // A new command arrived.
    // Check if it's a wave command and if that wave is already running.
    bool ignoreCmd = false;
    if (inputString[0] == 'W' || inputString[0] == 'w') {
        if (waveLeftState != WAVE_IDLE) {
            Serial.println("Ignoring 'W' command, left wave already running.");
            ignoreCmd = true;
        }
    } else if (inputString[0] == 'E' || inputString[0] == 'e') {
        if (waveRightState != WAVE_IDLE) {
             Serial.println("Ignoring 'E' command, right wave already running.");
            ignoreCmd = true;
        }
    }
    // Add interrupt logic for other async actions if needed (e.g., stop moveToPose)
    // if (!ignoreCmd && isMovingToPose) {
    //    isMovingToPose = false;
    //    Serial.println("MoveToPose interrupted by new command.");
    // }


    if (!ignoreCmd) {
        parse(); // Parse and initiate the new command if not ignored
    } else {
        // Clear the buffer if the command was ignored
        for (int i = 0; i < bufLen; i++) { inputString[i] = '\0'; }
    }
  }
}
