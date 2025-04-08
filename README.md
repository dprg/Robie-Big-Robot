# Robie-Big-Robot
Files related to Robie, the 7-foot tall DPRG mascot.

## Repo Contents

CAD models are in mechanical/cad_files. At the time of repo creation they contain 
the design files delivered by Ron Grant to Paul Bouchier on 26 Feb 2025.

Arduino files are as delivered by Doug Paradis to Paul Bouchier on March 2 2025. There
are two source code directories: Big_robot_pgm1 and Big_robot_pgm2.
Except for the name of two files having been changed, all files in the two directories
are identical. 

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
