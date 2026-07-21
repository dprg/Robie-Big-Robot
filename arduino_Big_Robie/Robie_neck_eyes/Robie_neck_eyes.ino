/***********************************
// Robie_neck_eyes Arduino sketch
// Stored on GitHub 
// https://github.com/dprg/Robie-Big-Robot
//
// Two steppers, head PAN and TILT
// 27 Eye LEDs (neo-pixel type)
//
// Head serial character commands:
// C - Centers the head looking slightly down
// L - Look left
// R - Look right
// U - Look up
// D - look down
// Pan full range is 20 Rs or Ls
// Tilt full range 10 Us or Ds
// Multiple commnads can be sent per line string
//
// Eye default motion is a RED dot cycling left and right
// But while responding to LRUD the eyes are blue dots in the
// direction of movement
//
// The serial interface to the 2 stepper modules is a 
// single bi-dir multi-drop wire
// Each stepper module has its own address
//
// 7/7/2026 Mike Williamson
***********************************/

#include <TMCStepper.h>
#include <SoftwareSerial.h>
#include "AccelStepper.h"
#include <Adafruit_NeoPixel.h>

#define USB_SERIAL_BAUD 115200L
#define STEPPER_SERIAL_BAUD 115200L

#define stepper0_enaPin   5
#define stepper0_stepPin  4
#define stepper0_dirPin   3
#define stepper0_diagPin  2

#define stepper1_enaPin   9
#define stepper1_stepPin  8
#define stepper1_dirPin   7
#define stepper1_diagPin  6

#define SW_RX            10 // SoftwareSerial receive pin
#define SW_TX            11 // SoftwareSerial transmit pin

#define eyesPin          12
#define diagLedPin			 13

AccelStepper stepper0(AccelStepper::DRIVER, stepper0_stepPin, stepper0_dirPin); //initialize tilt stepper
AccelStepper stepper1(AccelStepper::DRIVER, stepper1_stepPin, stepper1_dirPin); //initialize pan stepper

SoftwareSerial swSerial(SW_RX, SW_TX); // For stepper config

#define SERIAL_PORT Serial3 // TMC2208/TMC2224 HardwareSerial port
#define DRIVER0_ADDRESS 0b00 // MS2,MS1  = 00
#define DRIVER1_ADDRESS 0b01 // MS2,MS1  = 01


#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11

// TILT driver0 and stepper0 controls the head tilt
TMC2209Stepper driver0(&swSerial, R_SENSE, DRIVER0_ADDRESS);  
#define STEPPER0_STALLVALUE  15 // [0..255]
#define STEPPER0_I 			     1800 // ma
#define STEPPER0_MICROSTEPS  2 // 1/N micro stepping 2,4,8,16,32

// PAN driver1 and stepper1 controls the head pan
TMC2209Stepper driver1(&swSerial, R_SENSE, DRIVER1_ADDRESS);  
#define STEPPER1_STALLVALUE  0 // [0..255]
#define STEPPER1_I 			     800 // ma
#define STEPPER1_MICROSTEPS  2 // 1/N micro stepping 2,4,8,16,32

// stepper motion configuration - does not configure 2290 module
// TILT Up Down
#define STEPPER0_TIME_FS	 1.0 // full scale travel time (sec)
#define STEPPER0_NCMDS_FS    10 // Number of UD commands to travel full scale
#define STEPPER0_STEPS_FS    50 //34 // full scale range steps not micro steps
#define STEPPER0_STEPS       (STEPPER0_MICROSTEPS*STEPPER0_STEPS_FS) // micro steps
#define STEPPER0_MAXSPEED    STEPPER0_STEPS/STEPPER0_TIME_FS // steps per sec
#define STEPPER0_ACCELERATION STEPPER0_MAXSPEED*2.0 // accelerate in 1/2 sec (steps/sec^2)
#define STEPPER0_UDSTEPS	 (STEPPER0_STEPS/STEPPER0_NCMDS_FS) // UD commaand steps
#define STEPPER0_UDIR        1 // tilt up stepper direction
#define STEPPER0_DDIR        0 // tilt down stepper direction
// PAN Right Left
#define STEPPER1_TIME_FS	 2.0 // full scale travel time (sec)
#define STEPPER1_NCMDS_FS    20 // Number of RL commands to travel full scale
#define STEPPER1_STEPS_FS    124 // full scale range steps not micro steps
#define STEPPER1_STEPS       (STEPPER1_MICROSTEPS*STEPPER1_STEPS_FS) // micro steps
#define STEPPER1_MAXSPEED    STEPPER1_STEPS/STEPPER1_TIME_FS // steps per sec
#define STEPPER1_ACCELERATION STEPPER1_MAXSPEED*2.0 // accelerate in 1/2 sec (steps/sec^2)
#define STEPPER1_LRSTEPS		 (STEPPER1_STEPS/STEPPER1_NCMDS_FS) // RL command steps
#define STEPPER1_LDIR        1 // pan left stepper direction
#define STEPPER1_RDIR        0 // pan right stepper direction

enum headCmds {
	Idle,
	Center,
	Left,
	Right,
	Up,
	Down
};

headCmds headCmd = Idle;
headCmds headCmdLast = Idle;

bool tiltStallDet = false;
bool panStallDet  = false;
int headCmdStallCnt = 0;

// States for head centering
enum centerStates {
	Wait,
	TiltLimitA,
	TiltLimitB,
	TiltCenterA,
	TiltCenterB,
	PanLimitA,
	PanLimitB,
	PanCenterA,
	PanCenterB,
	Done
};

bool panRightBusy = false;
bool panLeftBusy  = false;
bool tiltUpBusy   = false;
bool tiltDownBusy = false;


//EYES
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 27 // Num leds in Robies eye plate
#define DELAYVAL 50 // Time (in milliseconds) to pause between pixels

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, eyesPin, NEO_RGB + NEO_KHZ800);

/////////////////////////////////////////////////////////////////////////
// SETUP functions

void setupDrivers() {
  //SERIAL_PORT.begin(STEPPER_SERIAL_BAUD);
  swSerial.begin(STEPPER_SERIAL_BAUD);

	// Stepper driver 0
  driver0.begin();

  driver0.toff(4);
  driver0.blank_time(24);
  driver0.TCOOLTHRS(0xFFFFF); // 20bit max
  driver0.semin(5);
  driver0.semax(2);
  driver0.sedn(0b01);

  driver0.rms_current(STEPPER0_I);        // Set motor RMS current
  driver0.microsteps(STEPPER0_MICROSTEPS);          // Set microsteps to 1/16th
  driver0.SGTHRS(STEPPER0_STALLVALUE);

	// Stepper driver 1
  driver1.begin();

  driver1.toff(4);
  driver1.blank_time(24);
  driver1.TCOOLTHRS(0xFFFFF); // 20bit max
  driver1.semin(5);
  driver1.semax(2);
  driver1.sedn(0b01);

  driver1.rms_current(STEPPER1_I);        // Set motor RMS current
  driver1.microsteps(STEPPER1_MICROSTEPS);          // Set microsteps to 1/16th
  driver1.SGTHRS(STEPPER1_STALLVALUE);

}

void setupSteppers() {
	//enable both motors and set directions
	pinMode(stepper0_enaPin, OUTPUT);
	pinMode(stepper0_dirPin, OUTPUT);
	pinMode(stepper0_diagPin, INPUT);
	digitalWrite(stepper0_enaPin, LOW);
	digitalWrite(stepper0_dirPin, LOW);

	pinMode(stepper1_enaPin, OUTPUT);
	pinMode(stepper1_dirPin, OUTPUT);
	pinMode(stepper1_diagPin, INPUT);
	digitalWrite(stepper1_enaPin, LOW);
	digitalWrite(stepper1_dirPin, LOW);

	//initialize each of 2 motors with their index and their step pin
	stepper0.setEnablePin(stepper0_enaPin);
	stepper0.enableOutputs();
	stepper0.setMaxSpeed(STEPPER0_MAXSPEED);
	stepper0.setAcceleration(STEPPER0_ACCELERATION);
	stepper1.setEnablePin(stepper1_enaPin);
	stepper1.enableOutputs();
	stepper1.setMaxSpeed(STEPPER1_MAXSPEED);
	stepper1.setAcceleration(STEPPER1_ACCELERATION);
}

void setupEyes(){
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
}

//////////////////////////////////////////////////////////////////////////////
// LOOP tasks

// Process serial port for commands
// Single char comads are:
// C Center head pan and tilt (uses stops to calibrate movement)
// R L Pan head Right and left 1 movement
// U D Tilt head Up and Down 1 movement
void	commandsTask(uint32_t now_ms) {
	if(headCmd != Idle) {
		return;
	}

	if(!Serial.available()) return;
	// read serial port character
	char c = Serial.read();
  if ((c >= 97) && (c <= 122))  // char is lower case
    c -= 32;                    // change to upper case
	if (c=='C') {
		// Serial.println("Center head position");
		headCmd = Center;
		headCmdLast = headCmd;
	}
	else if (c=='R') {
		// Serial.println("Pan head RIGHT");
		headCmd = Right;
		headCmdLast = headCmd;
	}
	else if (c=='L') {
		// Serial.println("Pan head LEFT");
		headCmd = Left;
		headCmdLast = headCmd;
	}
	else if (c=='U') {
		// Serial.println("Tilt head UP");
		headCmd = Up;
		headCmdLast = headCmd;
	}
	else if (c=='D') {
		// Serial.println("Tilt head DOWN");
		headCmd = Down;
		headCmdLast = headCmd;
	}
}

void diagTask(uint32_t now_ms) {
  static uint32_t last_time=0;
	static int diag0 = 0;
	static int diag1 = 0;
	static bool lastDiag0Pin = false;
	static bool lastDiag1Pin = false;

	bool currentDiag0Pin = digitalRead(stepper0_diagPin);
	bool currentDiag1Pin = digitalRead(stepper1_diagPin);

	diag0 += ((!lastDiag0Pin)&&currentDiag0Pin)?1:0;
	diag1 += ((!lastDiag1Pin)&&currentDiag1Pin)?1:0;
	
	if (headCmd != headCmdLast) {
		headCmdStallCnt = 0;
	}
	else if (currentDiag0Pin) {
		if (headCmdLast==Up)   headCmd = Idle;
		if (headCmdLast==Down) headCmd = Idle;
	}
	else if (currentDiag1Pin) {
		if (headCmdLast==Right) headCmd = Idle;
		if (headCmdLast==Left)  headCmd = Idle;
	}

	lastDiag0Pin = currentDiag0Pin;
	lastDiag1Pin = currentDiag1Pin;

  if((now_ms-last_time) > 100) { //run every 0.1s
    last_time = now_ms;

		digitalWrite(diagLedPin, (diag0>0|diag1>0));
    Serial.print(diag1); Serial.print(" "); Serial.print(diag0);

		if(diag0>0) tiltStallDet = true;
		if(diag1>0) panStallDet  = true;

		diag0 = 0;
		diag1 = 0;

		// NOTE: reading driver consumes a lot of cycles and slows the stepper motion and eyes
		// uint16_t sg = driver0.SG_RESULT();
    // Serial.print(" ");
    // Serial.print(sg, DEC);

		Serial.println();
	}
}

void steppersTask(uint32_t now_us){

	stepper0.run();
	stepper1.run();

	bool stepper0_finished = stepper0.distanceToGo() == 0;
	bool stepper1_finished = stepper1.distanceToGo() == 0;

	//turn off busy flags when command finishes
	if (stepper1_finished) {
		if (panLeftBusy)  panLeftBusy  = false;
		if (panRightBusy) panRightBusy = false;
	}
	if (stepper0_finished) {
		if (tiltUpBusy)   tiltUpBusy   = false;
		if (tiltDownBusy) tiltDownBusy = false;
	}

	if (headCmd == Idle) return;

	
	if (stepper1_finished) {
		if (headCmd == Left) {
			stepper1.move(STEPPER1_LRSTEPS);
			headCmd = Idle;
			panLeftBusy = true;
		}
		else if (headCmd == Right) {
			stepper1.move(-STEPPER1_LRSTEPS);
			headCmd = Idle;
			panRightBusy = true;
		}	
	}

	if (stepper0_finished) {
		if (headCmd == Up) {
			stepper0.move(STEPPER0_UDSTEPS);
			headCmd = Idle;
			tiltUpBusy = true;
		}
		else if (headCmd == Down) {
			stepper0.move(-STEPPER0_UDSTEPS);
			headCmd = Idle;
			tiltDownBusy = true;
		}
	}

	// Head centering command is a sequence of movements
	static centerStates centerState = Wait;

	if (headCmd == Center) {
		if(centerState==Wait) {
			centerState  = TiltLimitA;
			tiltStallDet = false;
			panStallDet  = false;
		}
		else if(centerState==TiltLimitA) {
			// Move head Up 
			if(stepper0_finished) {
				digitalWrite(stepper0_dirPin, STEPPER0_UDIR); // UP direction
				// steppers.start_finite(0, STEPPER0_STEPDELAY, int(STEPPER0_UDSTEPS*STEPPER0_NCMDS_FS));
				centerState = TiltLimitB;
			}
		}
		else if(centerState==TiltLimitB) {
			// keep moving up until stall or stepper finished movement
			if(tiltStallDet || stepper0_finished) {
				tiltStallDet = false;
				stepper0.stop();
				centerState = TiltCenterA;
			}
		}
		else if(centerState==TiltCenterA) {
			// Move Down to center position
			if(stepper0_finished) {
				digitalWrite(stepper0_dirPin, STEPPER0_DDIR); // DOWN direction
				// steppers.start_finite(0, STEPPER0_STEPDELAY, 
				// 				int(STEPPER0_UDSTEPS*STEPPER0_NCMDS_FS*0.75));
				centerState = TiltCenterB;
			}
		}
		else if(centerState==TiltCenterB) {
			if(stepper0_finished) {
				centerState = PanLimitA;
			}
		}
		else if(centerState==PanLimitA) {
			// Move head Left 
			if(stepper1_finished) {
				digitalWrite(stepper1_dirPin, STEPPER1_LDIR); // LEFT direction
				//steppers.start_finite(1, STEPPER1_STEPDELAY, int(STEPPER1_LRSTEPS*STEPPER1_NCMDS_FS));
				centerState = PanLimitB;
			}
		}
		else if(centerState==PanLimitB) {
			// keep moving left until stall or stepper finished movement
			if(panStallDet || stepper1_finished) {
				panStallDet = false;
				stepper0.stop();
				stepper1.stop();
				centerState = PanCenterA;
			}
		}
		else if(centerState==PanCenterA) {
			// Move Right to center position
			if(stepper1_finished) {
				digitalWrite(stepper1_dirPin, STEPPER1_RDIR); // RIGHT direction
				//steppers.start_finite(1, STEPPER1_STEPDELAY, int(STEPPER1_LRSTEPS*STEPPER1_NCMDS_FS*0.5));
				centerState = PanCenterB;
			}
		}
		else if(centerState==PanCenterB) {
			if(stepper1_finished) {
				centerState = Done;
			}
		}
		else if(centerState==Done) {
			centerState = Wait;
			headCmd = Idle;
		}
	}
}

void eyesTask(uint32_t now_ms) {
static int i = 0;
static bool dir = true;
static uint32_t last_ms = 0;

	if((now_ms-last_ms) < DELAYVAL) return;

	last_ms = now_ms;
	pixels.clear(); // Set all pixel colors to 'off'
	// The first NeoPixel in a strand is #0, second is 1, all the way up
	// to the count of pixels minus one.
	// for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

	if(panLeftBusy) {
		pixels.setPixelColor(7, pixels.Color(0, 0, 200));
		pixels.show();   // Send the updated pixel colors to the hardware.
	}
	else if(panRightBusy) {
		pixels.setPixelColor(20, pixels.Color(0, 0, 200));
		pixels.show();   // Send the updated pixel colors to the hardware.
	}
	else if(tiltUpBusy) {
		pixels.setPixelColor(0, pixels.Color(0, 0, 200));
		pixels.setPixelColor(26, pixels.Color(0, 0, 200));
		pixels.show();   // Send the updated pixel colors to the hardware.
	}
	else if(tiltDownBusy) {
		pixels.setPixelColor(12, pixels.Color(0, 0, 200));
		pixels.setPixelColor(13, pixels.Color(0, 0, 200));
		pixels.setPixelColor(14, pixels.Color(0, 0, 200));
		pixels.show();   // Send the updated pixel colors to the hardware.
	}
	else {

		// pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
		// Here we're using a moderately bright red color:
		pixels.setPixelColor(i, pixels.Color(150, 0, 0));
		pixels.show();   // Send the updated pixel colors to the hardware.

		if(dir) {
			i++;
			if(i==NUMPIXELS) {
				dir = false;
				i--;
			}
		} else {
			i--;
			if(i==-1) {
				dir = true;
				i++;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// setup and loop executive code

void setup() {
  Serial.begin(USB_SERIAL_BAUD);         // Init serial port and set baudrate
  while(!Serial);               // Wait for serial port to connect
  Serial.print("\nStart...");

	setupDrivers();

  int ver0 = driver0.version();
  int ver1 = driver1.version();
  Serial.print("Driver version for stepper modules 0, 1 : "); Serial.print(ver0,DEC);
  Serial.print(", "); Serial.println(ver1,DEC);

	setupSteppers();

	setupEyes();
}

void loop() {
  uint32_t now_ms = millis();
	uint32_t now_us = micros();

	commandsTask(now_ms);

    steppersTask(now_us);

	diagTask(now_ms);

	eyesTask(now_ms);

}
