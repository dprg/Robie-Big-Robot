
#include <TMCStepper.h>
#include <SoftwareSerial.h>
#include "MultiStepperLite.h"
#include <Adafruit_NeoPixel.h>

#define USB_SERIAL_BAUD 115200L
#define STEPPER_SERIAL_BAUD 115200L

#define motor0_enaPin 5
#define motor0_stepPin    4
#define motor0_dirPin     3
#define motor0_diagPin    2

#define motor1_enaPin 9
#define motor1_stepPin    8
#define motor1_dirPin     7
#define motor1_diagPin    6

#define SW_RX            10 // SoftwareSerial receive pin
#define SW_TX            11 // SoftwareSerial transmit pin

#define eyesPin          12
#define diagLedPin			 13

MultiStepperLite steppers(2); //initialize for 2 motors

bool motor0_finish_signalled = false;

SoftwareSerial swSerial(SW_RX, SW_TX);

#define SERIAL_PORT Serial3 // TMC2208/TMC2224 HardwareSerial port
#define DRIVER0_ADDRESS 0b00 // MS2,MS1  = 00
#define DRIVER1_ADDRESS 0b01 // MS2,MS1  = 01


#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11

// Select your stepper driver type
//TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);
TMC2209Stepper driver0(&swSerial, R_SENSE, DRIVER0_ADDRESS);  
TMC2209Stepper driver1(&swSerial, R_SENSE, DRIVER1_ADDRESS);  

#define STALL_VALUE     50 // [0..255]

//EYES
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 27 // Num leds in Robies eye plate
#define DELAYVAL 50 // Time (in milliseconds) to pause between pixels

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, eyesPin, NEO_RGB + NEO_KHZ800);


void setupDrivers() {
  //SERIAL_PORT.begin(STEPPER_SERIAL_BAUD);
  swSerial.begin(STEPPER_SERIAL_BAUD);

	// Stepper driver 0
  driver0.begin();

  driver0.toff(4);
  driver0.blank_time(24);

  driver0.rms_current(200);        // Set motor RMS current
  driver0.microsteps(16);          // Set microsteps to 1/16th

  driver0.TCOOLTHRS(0xFFFFF); // 20bit max
  driver0.semin(5);
  driver0.semax(2);
  driver0.sedn(0b01);
  driver0.SGTHRS(STALL_VALUE);

	// Stepper driver 1
  driver1.begin();

  driver1.toff(4);
  driver1.blank_time(24);

  driver1.rms_current(200);        // Set motor RMS current
  driver1.microsteps(16);          // Set microsteps to 1/16th

  driver1.TCOOLTHRS(0xFFFFF); // 20bit max
  driver1.semin(5);
  driver1.semax(2);
  driver1.sedn(0b01);
  driver1.SGTHRS(STALL_VALUE);

//driver0.en_pwm_mode(true);       // Toggle stealthChop on TMC2130/2160/5130/5160
//driver0.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208/2209/2224
//driver0.pwm_autoscale(true);     // Needed for stealthChop
}


void setupSteppers() {
	//enable both motors and set directions
	pinMode(motor0_enaPin, OUTPUT);
	pinMode(motor0_dirPin, OUTPUT);
	pinMode(motor0_diagPin, INPUT);
	digitalWrite(motor0_enaPin, LOW);
	digitalWrite(motor0_dirPin, LOW);

	pinMode(motor1_enaPin, OUTPUT);
	pinMode(motor1_dirPin, OUTPUT);
	pinMode(motor1_diagPin, INPUT);
	digitalWrite(motor1_enaPin, LOW);
	digitalWrite(motor1_dirPin, LOW);

	//initialize each of 2 motors with their index and their step pin
	steppers.init_stepper(0, motor0_stepPin);
	steppers.init_stepper(1, motor1_stepPin);
}

void setupEyes(){
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
}


void setup() {
  Serial.begin(USB_SERIAL_BAUD);         // Init serial port and set baudrate
  while(!Serial);               // Wait for serial port to connect
  Serial.println("\nStart...");

	setupDrivers();

  int ver0 = driver0.version();
  int ver1 = driver1.version();
  Serial.print("Driver versions : "); Serial.print(ver0,DEC);
  Serial.print(", "); Serial.println(ver1,DEC);

	setupSteppers();

	//start motor 0, with 400 microseconds delay between steps and with finite steps of 2500
	steppers.start_finite(0, 400, 2500);

	//start motor 1 to run indefinitely
	steppers.start_continuous(1, 400);

	setupEyes();

}

void loop() {
  uint32_t now_ms = millis();
	uint32_t now_us = micros();

	steppersTask(now_us);

	diagTask(now_ms);

	eyesTask(now_ms);

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


void diagTask(uint32_t now_ms) {
  static uint32_t last_time=0;

  if((now_ms-last_time) > 100) { //run every 0.1s
    last_time = now_ms;

		bool diag0 = digitalRead(motor0_diagPin);
		bool diag1 = digitalRead(motor1_diagPin);
		digitalWrite(diagLedPin, (diag0|diag1));
    Serial.print(diag1); Serial.print(diag0);

		// NOTE: reading driver consumes a lot of cycles and slows the stepper
		// uint16_t sg = driver0.SG_RESULT();
		// uint16_t cs = driver0.cs2rms(driver0.cs_actual());
    // Serial.print("0 ");
    // Serial.print(sg, DEC);
    // Serial.print(" ");
    // Serial.print(cs , DEC);

		Serial.println();
	}
}
void steppersTask(uint32_t now_us){
	steppers.do_tasks(now_us);
	//alternatively, define uint32_t now_us = micros() and call steppers.do_tasks(now_us)
	//this can be useful if micros() is already called for other purposes, as micros() is rather costly to call
	//without an argument, the function calls micros() internally

	if (steppers.is_finished(0)){ //if motor 0 completed all the steps
		if (!motor0_finish_signalled) { //if end of motor task is not signalled already
			// Serial.println("Motor 0 is finished.");
			motor0_finish_signalled = true;

      digitalWrite(motor0_dirPin, !digitalRead(motor0_dirPin));
      steppers.start_finite(0, 400, 2500);
			motor0_finish_signalled = false;

		}
	}

}
