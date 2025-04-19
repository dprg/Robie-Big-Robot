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
  initialize(); // Assumes initialize() sets up pins, serial, servos, motor structs etc.

  // Initialize ALL motor states
  if (ptrTiltMtr) ptrTiltMtr->state = MOTOR_IDLE;
  if (ptrPanMtr) ptrPanMtr->state = MOTOR_IDLE;
  if (ptrRightShoulderLiftMtr) ptrRightShoulderLiftMtr->state = MOTOR_IDLE;
  if (ptrRightShoulderRotMtr) ptrRightShoulderRotMtr->state = MOTOR_IDLE;

  // Initialize Pose state
  isMovingToPose = false;

  // Initialize Wave states
  waveLeftState = WAVE_IDLE;
  waveRightState = WAVE_IDLE;

  // Set initial motor directions/speeds if needed (most are set by commands now)
  // if (ptrTiltMtr) ptrTiltMtr->speed = NORM_SPD;
  // if (ptrPanMtr) ptrPanMtr->dir = CW;
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

  // --- Update ALL State Machines ---
  updateDcMotors();       // Handles Tilt, Shoulder motors after startDcMotorStep
  updateStepperMotor();   // Handles Pan motor after startStepperStep
  updateMoveToPose();     // Handles coordinated shoulder movement
  updateWaveLeft();       // Handles left servo wave
  updateWaveRight();      // Handles right servo wave

  // --- Check for Input ---
  int cmdLen = 0;
  cmdLen = readCmdLine(); // Check if a new command has arrived
  if (cmdLen > 0) {
    // A new command arrived. Check if it should be ignored or if it interrupts moveToPose.
    bool ignoreCmd = false;
    char cmdChar = toupper(inputString[0]); // Use uppercase for checks

    // --- Check for Busy States / Ignore Logic ---
    switch(cmdChar) {
        case 'W':
            if (waveLeftState != WAVE_IDLE) { ignoreCmd = true; Serial.println("Ignoring 'W': Left wave busy."); }
            break;
        case 'E':
            if (waveRightState != WAVE_IDLE) { ignoreCmd = true; Serial.println("Ignoring 'E': Right wave busy."); }
            break;
        case 'L':
        case 'R':
            if (ptrPanMtr && ptrPanMtr->state != MOTOR_IDLE) { ignoreCmd = true; Serial.println("Ignoring 'L'/'R': Pan motor busy."); }
            break;
        case 'U':
        case 'D':
            if (ptrTiltMtr && ptrTiltMtr->state != MOTOR_IDLE) { ignoreCmd = true; Serial.println("Ignoring 'U'/'D': Tilt motor busy."); }
            break;
        case '1': // Lift Up
        case '3': // Lift Down
            if (ptrRightShoulderLiftMtr && ptrRightShoulderLiftMtr->state != MOTOR_IDLE) { ignoreCmd = true; Serial.println("Ignoring '1'/'3': Shoulder Lift motor busy."); }
            break;
        case '2': // Rot More
        case '4': // Rot Less
            if (ptrRightShoulderRotMtr && ptrRightShoulderRotMtr->state != MOTOR_IDLE) { ignoreCmd = true; Serial.println("Ignoring '2'/'4': Shoulder Rot motor busy."); }
            break;
        case '5':
        case '6':
        case '7':
        case '8':
            if (isMovingToPose) { ignoreCmd = true; Serial.println("Ignoring '5'-'8': MoveToPose busy."); }
            // Also check if individual shoulder motors are busy from single steps
            else if (ptrRightShoulderLiftMtr && ptrRightShoulderLiftMtr->state != MOTOR_IDLE) { ignoreCmd = true; Serial.println("Ignoring '5'-'8': Shoulder Lift motor busy."); }
            else if (ptrRightShoulderRotMtr && ptrRightShoulderRotMtr->state != MOTOR_IDLE) { ignoreCmd = true; Serial.println("Ignoring '5'-'8': Shoulder Rot motor busy."); }
            break;
        default:
            // Unknown commands are handled in parse()
            break;
    }

    // --- Interrupt Logic ---
    // If the command is NOT ignored AND moveToPose is active, interrupt moveToPose.
    // Exception: Don't interrupt for another moveToPose command ('5'-'8'), as those are ignored above if busy.
    if (!ignoreCmd && isMovingToPose && (cmdChar < '5' || cmdChar > '8')) {
       isMovingToPose = false;
       // Stop the shoulder motors immediately
       if(poseLiftMotorPtr && poseLiftMotorPtr->state == MOTOR_STEPPING_DC) {
           analogWrite(poseLiftMotorPtr->enPin, STOP);
           digitalWrite(poseLiftMotorPtr->in1Pin, LOW); digitalWrite(poseLiftMotorPtr->in2Pin, LOW);
           poseLiftMotorPtr->state = MOTOR_IDLE;
       }
       if(poseRotMotorPtr && poseRotMotorPtr->state == MOTOR_STEPPING_DC) {
           analogWrite(poseRotMotorPtr->enPin, STOP);
           digitalWrite(poseRotMotorPtr->in1Pin, LOW); digitalWrite(poseRotMotorPtr->in2Pin, LOW);
           poseRotMotorPtr->state = MOTOR_IDLE;
       }
       Serial.println("MoveToPose interrupted by new command.");
    }

    // --- Execute or Clear ---
    if (!ignoreCmd) {
        parse(); // Parse and initiate the new command if not ignored
    } else {
        // Clear the buffer because the command was ignored
       inputString[0] = '\0'; // Clear first char is enough for readCmdLine logic
       // Optional: Clear whole buffer for safety
       // for (int i = 0; i < bufLen; i++) { inputString[i] = '\0'; }
    }
  }
}
