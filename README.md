# Robie-Big-Robot
Files related to Robie, the 7-foot tall DPRG mascot

CAD models are in mechanical/cad_files. At the time of repo creation they contain 
the design files delivered by Ron Grant to Paul Bouchier on 26 Feb 2025.

Arduino files are as delivered by Doug Paradis to Paul Bouchier on March 2 2025. There
are two source code directories: Big_robot_pgm1 and Big_robot_pgm2.
Except for the name of two files having been changed, all files in the two directories
are identical. 

TODO: Delete one of the directories, and get it compiling on modern Arduino.

Mike Williamson 3/21/2025
Added opencv code to detect faces which will control the head to point to it and maybe even wave when centered on face

Mark R 3/26/2025
Added support for Linux, as well as some error catching stuff

It is based on the instructions from this page:
https://thepythoncode.com/article/detect-faces-opencv-python
