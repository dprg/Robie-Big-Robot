# Robie-Big-Robot
Files related to Robie, the 7-foot tall DPRG mascot.

## Repo Contents

CAD models are in mechanical/cad_files. At the time of repo creation they contain 
the design files delivered by Ron Grant to Paul Bouchier on 26 Feb 2025.

Arduino files are as delivered by Doug Paradis to Paul Bouchier on March 2 2025. There
are two source code directories: Big_robot_pgm1 and Big_robot_pgm2.
Except for the name of two files having been changed, all files in the two directories
are identical. 

## Motors

### Head Pan Motor

The head pan motor is a step motor, with limit sensing provided by a slotted disk and opto
sensor in the torso. At startup, the head needs to not be at a limit. Each step motor step
lasts 15ms. If a limit is hit, the motor changes direction and steps the remainder of the step
count in the opposite direction. When asked to pan, the motor takes 20 steps for each
command (5 iterations of the 4-step phase sequence).
There are ***  *** moves from limit to limit.
Current position could be found by tracking move commands, within the accuracy bounds
set by the auto-reverse-on-limit behavior.

### Head tilt motor

The head tilt motor is a DC motor, with HW limit switches that interrupt
motor power when a limit is hit. The limit state is not available to FW.
When commanded to move, the motor is run for 40ms with a PWM speed of 125.
There are *** TBD ?? *** moves from limit to limit.  Current position
could be found by tracking move commands, within the accuracy bounds
set by DC motor run distance per request.

### Shoulder rotate and lift motors

The shoulder rotate and lift motors are DC motors, with HW limit switches
that interrupt motor power when a limit is hit. The limit state is not
available to FW, but the current position is sensed by a potentiometer
and is available to FW.  The rotate motor rotates the right arm about
the shoulder in the forward direction, and the lift motor lifts the
right arm sideways.  The range of motion available between limits is
180 degrees for both rotate and lift. The lift sensing pot reads 100 -
770 in the ADC at each extreme.  The rotate sensing pot reads 230 -
950 in the ADC at each extreme.  When commanded to take a step move,
the motor is run for 40ms with a PWM speed of 125.  This motor has a
"move to position" function in FW, wherein the FW drives the motor until
the potentiometer achieves a requested feedback value. Note that the
weight of the arm means the arm travels downward further than upward
for a single move request.

### Left wave motor

The left wave motor twists the hand on the forearm. It is a regular servo
with 360 degrees (approx) range of motion, and it moves to the position
requested by the applied PWM.

### Right wave motor

The right wave motor twists the hand on the forearm. It is a continuous-rotation servo
and is stationary for mid-range applied PWM, and rotates continuously at different speeds
based on how far the applied PWM is from mid-range.

## Arduino API

The commands available from the arduino are listed here. They are received on the serial
port, and can come from a human on a comms terminal or a program driving serial commands.

---

### Legacy Commands

These commands are the API as of early 2025. Commands are single letters and
are executed immediately upon receipt (no CR-LF required). Multiple commands
are buffered and executed in order.

- L - Pan head left. Responds with L
- R - Pan head right. Responds with R
- U - Tilt head up. Responds with U
- D - Tilt head down. Responds with D.
- W - Rotates left wrist twice in each direction. Responds with W.
- E - Rotates right wrist twice in each direction. Responds with E.
- 1 - Lift the arm sideways (up/away from body). Responds with:<br>
lift_feedback, rotate_feedback<br>
1<br>
- 2 - Rotate the arm CCW on the shoulder. If the arm is hanging down this results
in forward and up)motion. Similar response as 1 except echoes 2
- 3 - The opposite of 1. Similar response as 1 except echoes 3
- 4 - The opposite of 2. Similar response as 1 except echoes 4
- 5, 6, 6, 7 - Move to fixed poses. Responds with a series of (lift_feedback, rotate_feedback)
pairs until movement is complete.

## Robie's Want List

Things Robie could really use:

### Mechanical upgrades & fixes desired

- Mechanical: Head pan seems to get bound up - doesn't run freely from
limit to limit. Mechanical connection to all-thread is not stable. Encoder
disk doesn't secure properly.
- Mount for step motor prevents electronics door from closing.

### Electrical upgrades & fixes desired

- The connectors for the right shoulder pots should be on keyed wiremount
connectors that don't fall apart all the time
- The flying lead going to D35 which drives the right wrist needs to
get wired to a terminal block, and thence to a 3-wire (at least) unique
connector going up into the arm, replacing the XT30 that currently
supplies power to the right wrist.
- An elbow (with a BLDC motor) would give it much more expressiveness.

### Software upgrades & fixes desired

- Provide a parser better suited to computer control. The current
parser is good for human interaction, but has weaknesses for
API interaction (irregular feedback) and is not suited to async operation.
It would be desirable for it to use a different serial port, and be a
whole different parser that uses json to pass commands & status, so
as to retain the debug output and human-interactive aspects of the
current parser on the arduino serial port.
- Replace the haar-cascade face recognizer with an AI face recognizer, which
would be much more reliable at detecting faces.

## Media

Robie Resuscitation 2025
![Robie Resuscitated](images/RobieResuscitated.png)
<br>
Robie at iMake Ft Worth (2017)
![Robie at iMake Ft Worth (2017)](images/RobieAtFtWorthIMake.png)
<br>
Robie with Doug
![Robie with Doug](images/Doug_w_big_robie_in_gargage2017.JPG)
<br>
### Videos
[Video: Resuscitated Robie moving](https://youtu.be/EISYc3Z7FQA)
<br>
[Video: Early Robie at a DPRG show](https://youtu.be/n2Y_eyLx5xM)

## Changelog

Mike Williamson 3/21/2025<br>
Added opencv code to detect faces which will control the head to point to it and maybe even wave when centered on face<br>

start code: cd to opencv in terminal run py -f facedet_test.py<br>

Mark R 3/26/2025
Added support for Linux, as well as some error catching stuff

<br>
It is based on the instructions from this page:
https://thepythoncode.com/article/detect-faces-opencv-python 
<br>
Paul B 4/1/2025
Removed duplicate arduino directory Big_robot_pgm2. Added image to README.md
