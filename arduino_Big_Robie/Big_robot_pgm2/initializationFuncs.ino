/***************************
*
*  Initialization Funcs
*
****************************/

void initialize(void){
	// initialize pan motor
	ptrPanMtr->dir = CW;
	ptrPanMtr->type = UNIPOLAR;
	ptrPanMtr->seqNum = 0;
	ptrPanMtr->in1Pin = 23;  
	ptrPanMtr->in2Pin = 25;  
	ptrPanMtr->in3Pin = 27; 
	ptrPanMtr->in4Pin = 29;  
    ptrPanMtr->delayBetweenSteps = 15;             // millisecs 
	ptrPanMtr->limitSwType = OPTO;
	ptrPanMtr->limitCWpin = 51;                    // only one limit switch (opto interrupter type)
	ptrPanMtr->limitCCWpin = 53;                   // used to drive IR diode (w/ series resistor)
	ptrPanMtr->limitCWstate = NOT_TRIPPED;         // open = not tripped (collecter HIGH), closed = tripped
	ptrPanMtr->limitCCWstate = NOT_TRIPPED;

    pinMode(ptrPanMtr->in1Pin, OUTPUT);
    pinMode(ptrPanMtr->in2Pin, OUTPUT);
    pinMode(ptrPanMtr->in3Pin, OUTPUT);
	pinMode(ptrPanMtr->in4Pin, OUTPUT);
    pinMode(ptrPanMtr->limitCWpin,INPUT_PULLUP);  // connected to open collector of opto xtr 
    pinMode(ptrPanMtr->limitCCWpin,OUTPUT);       // used to drive IR diode

    digitalWrite(ptrPanMtr->limitCCWpin,HIGH);      // turn on opto interrupter IR diode
	
	
	
	// initialize tilt motor           (yellow A)
    ptrTiltMtr->type = DC_MTR;
	ptrTiltMtr->dir = CW;
	ptrTiltMtr->speed = NORM_SPD;
	ptrTiltMtr->enPin = 3;     // must be pwm pin
	ptrTiltMtr->in1Pin = 22;
	ptrTiltMtr->in2Pin = 24;
	ptrTiltMtr->limitSwType = NONE;
	
    pinMode(ptrTiltMtr->enPin, OUTPUT);
	pinMode(ptrTiltMtr->in1Pin, OUTPUT);
	pinMode(ptrTiltMtr->in2Pin, OUTPUT);

	
	


	// initialize rightShoulderLiftMtr1       (blue A)
    ptrRightShoulderLiftMtr->type = DC_MTR;
	ptrRightShoulderLiftMtr->dir = CW;
	ptrRightShoulderLiftMtr->speed = STOP;
	ptrRightShoulderLiftMtr->enPin = 9;                         // must be pwm pin
	ptrRightShoulderLiftMtr->in1Pin = 46;
	ptrRightShoulderLiftMtr->in2Pin = 48;
	ptrRightShoulderLiftMtr->potPin = 2;
	ptrRightShoulderLiftMtr->limitSwType = NONE;
	
    pinMode(ptrRightShoulderLiftMtr->enPin, OUTPUT);
	pinMode(ptrRightShoulderLiftMtr->in1Pin, OUTPUT);
	pinMode(ptrRightShoulderLiftMtr->in2Pin, OUTPUT);
	
	
	// initialize rightShoulderRotMtr    (blue B)
    ptrRightShoulderRotMtr->type = DC_MTR;
	ptrRightShoulderRotMtr->dir = CW;
	ptrRightShoulderRotMtr->speed = STOP;
	ptrRightShoulderRotMtr->enPin = 10;                         // must be pwm pin
	ptrRightShoulderRotMtr->in1Pin = 50;
	ptrRightShoulderRotMtr->in2Pin = 52;
	ptrRightShoulderRotMtr->potPin = 3;
	ptrRightShoulderRotMtr->limitSwType = NONE;
	
    pinMode(ptrRightShoulderRotMtr->enPin, OUTPUT);
	pinMode(ptrRightShoulderRotMtr->in1Pin, OUTPUT);
	pinMode(ptrRightShoulderRotMtr->in2Pin, OUTPUT);
	
	
	


	// Setup Serial port
	// reserve a 12 char buffer for incoming string
	//inputString.reserve(bufLen); 
	//Initialize serial port
	Serial.begin(baudRate);
	establishContact();   // send "BIG ROBOT" to laptop
	

    // Misc item setup
	pinMode(BUTTON, INPUT_PULLUP);
	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);     // turn off led
	
	
	// Servo setup
	waveServo.attach(37);
	
	


}
	
	
