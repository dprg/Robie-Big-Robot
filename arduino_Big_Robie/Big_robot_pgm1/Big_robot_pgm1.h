/********************************
 *
 *  main header file
 *
 *
 ********************************/
// directions
#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4
// speeds
#define STOP 0
#define TILT_UP_SPD 125
#define TILT_DWN_SPD 125
#define NORM_SPD 125 // 0 to 255
#define DUMMY 1		 // used for stepper motors
// motor directions
#define SHUTDOWN 0
#define CW 1
#define CCW -1
// limit switch states
#define TRIPPED 0
#define NOT_TRIPPED 1
#define ESTOP 2
// limit switch types
#define NONE 0
#define NO_SW 1
#define NC_SW 2
#define OPTO 3
// motor types
#define DC_MTR 1
#define UNIPOLAR 2
#define BIPOLAR 3

// Motor States
typedef enum {
  MOTOR_IDLE,
  MOTOR_STEPPING_MICROSTEP, // For stepper during its sequence
  MOTOR_STEPPING_DC         // For DC motor during its timed pulse
} MotorState;

// Wave States
typedef enum {
  WAVE_IDLE,
  WAVE_UP,
  WAVE_DOWN
} WaveState;

// shoulder movement related
#define DEAD_BAND 50 // 25

// Global state for Left Wave (Standard Servo)
extern WaveState waveLeftState;
extern unsigned long waveLeftNextStepMillis;
extern int waveLeftCurrentPos;
extern int waveLeftRepetitions;

// Global state for Right Wave (Continuous Servo)
extern WaveState waveRightState;
extern unsigned long waveRightNextStepMillis;
extern int waveRightCurrentPos;
extern int waveRightRepetitions;

// Full motor struct definition
typedef struct motor
{
	int type;
  MotorState state; // Current state of the motor
	int dir;
	int speed;
	int enPin;
	int seqNum;
	int in1Pin;
	int in2Pin;
	int in3Pin;
	int in4Pin;
	int potPin;
	int potVal;
	int delayBetweenSteps;
	int limitSwType;
	int limitCWpin;
	int limitCCWpin;
	int limitCWstate;
	int limitCCWstate;
	int dirChgFlg;

  // State variables for non-blocking control
  unsigned long stepTargetMillis; // End time for DC step or time for next micro-step
  int microStepsRemaining; // For stepper motor control
} motor;


// Pose Movement State (Now declared after motor type is defined)
extern bool isMovingToPose;
extern int poseTargetLift;
extern int poseTargetRot;
extern int poseDeadband;
extern motor *poseLiftMotorPtr;
extern motor *poseRotMotorPtr;


// Function Prototypes for non-blocking operations
void updateDcMotors(void);
void updateStepperMotor(void); // Specifically for the pan motor
void updateMoveToPose(void);
void startDcMotorStep(int direction, int speed, int timeOn, motor *mtr);
void startStepperStep(int direction, motor *mtr); // Stepper takes fixed steps/delay
void startMoveToPose(int targetLift, int targetRot, int deadband, motor *liftMtr, motor *rotMtr);

// Function Prototypes for non-blocking wave operations (already exist)
void startWaveLeft();
void updateWaveLeft();
void startWaveRight();
void updateWaveRight();
