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
#define NORM_SPD 125     // 0 to 255
#define DUMMY 1          // used for stepper motors
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

// shoulder movement related
#define DEAD_BAND 50  //25


typedef struct motor {
	int type;
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
} motor;

