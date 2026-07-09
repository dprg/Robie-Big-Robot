import cv2
import mediapipe as mp
import serial
import time
from select import select
import sys
import traceback

windowsHeadArduino = "com39"
windowsBodyArduino = "com40"
linuxHeadArduino = "/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_953313030343518090D1-if00"     # Arduino Uno in Robie head
linuxBodyArduino = "/dev/serial/by-id/usb-Arduino__www.arduino.cc__0042_85332313036351F0A121-if00"     # Arduino Mega in Robie torso

class DummySerial:
    def __init__(self, port):
        print("DummySerial initialized. No actual serial communication will occur.")
        self.port = port

    def write(self, data):
        print(f"DummySerial write to `{self.port}`: {data}")

    def reset_input_buffer(self):
        pass

def get_os():
    platform = sys.platform
    if platform == "win32" or platform == "cygwin":
        return "windows"
    elif platform.startswith("linux"):
        return "linux"
    else:
        return "other"

def getch():
    if(operating_system == "windows"):
        return msvcrt.getch()
    elif (operating_system == "linux"):
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch
    else:
        return sys.stdin.read(1)  # Default behavior for other OSes

operating_system = get_os()
print(f"Operating system detected: {operating_system}")

if operating_system == "windows":
    print("Running on Windows")
    import msvcrt
    try:
        robie_head = serial.Serial(windowsHeadArduino)
        robie_body = serial.Serial(windowsBodyArduino)
    except Exception as e:
        print("Unable to open Arduino serial ports" )
        traceback.print_exc()
        sys.exit(1)
        
elif operating_system == "linux":
    print("Running on Linux")
    import tty
    import termios
    try:
        robie_head = serial.Serial(linuxHeadArduino)
        robie_body = serial.Serial(linuxBodyArduino)
        #robie_head = DummySerial(linuxHeadArduino)
        #robie_body = DummySerial(linuxBodyArduino)
    except Exception as e:
        print("Unable to open Arduino serial ports")
        traceback.print_exc()
        sys.exit(1)
        
else:
    print("Running on an unknown or unsupported OS")
    # Code for other OSes (e.g., macOS)

# Initialize MediaPipe Face Detection
mp_face_detection = mp.solutions.face_detection
# Model selection: 0 for short-range (within 2 meters), 1 for long-range (up to 5 meters)
face_detection = mp_face_detection.FaceDetection(model_selection=0, min_detection_confidence=0.5)

# Open the default camera (0 is usually the integrated Pi Cam or first USB webcam)
cap = cv2.VideoCapture(0)
# Set buffer size to 1 frame
cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

motion_time_last = time.monotonic()
do_face_det_motions = True
pose = 5

# period between make motions when a face is centered
motion_time_period = 4.0

scan_left = False
scanlr_cnt = 0
scanlr_cnt_max = 40
scan_up = False
scanud_cnt = 0
scanud_cnt_max = 20

loop_delay = 500   # milliseconds

h_mid = 0
w_mid = 0
h_show = 0
w_show = 0

# seems like a delay is needed before interacting with Robie
time.sleep(2.0)

# position face toward center using C command
robie_head.reset_input_buffer()
print("C")
robie_head.write("C".encode('ascii'))
time.sleep(5.0)  # wait for Robie to center the face

print("Starting face detection. Press 'q' in the window to quit...")

while cap.isOpened():
    robie_head.reset_input_buffer()

    # Flush the buffer: grab all waiting frames without decoding them
    # Most webcams buffer between 2 to 5 frames
    for _ in range(5):
        cap.grab()
        
    # Retrieve the latest frame
    success, frame = cap.retrieve()
    if not success:
        print("Ignoring empty camera frame.")
        time.sleep(loop_delay / 1000)  # Convert milliseconds to seconds
        continue

    # Convert the BGR image to RGB for MediaPipe processing
    image_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = face_detection.process(image_rgb)

    # Get image dimensions to scale the normalized coordinates
    height, width, _ = frame.shape
    h_mid = height / 2
    w_mid = width / 2
    h_disp = int(height * 2.0)
    w_disp = int(width * 2.0)

    # tracking for the largest face detected
    face = {'x': 0, 'y': 0, 'a': 0}

    # Check if any faces were detected
    if results.detections:
        for detection in results.detections:
            # Extract the relative bounding box data
            bbox = detection.location_data.relative_bounding_box
            
            # Convert normalized 0.0-1.0 coordinates into actual pixel integers
            xmin = int(bbox.xmin * width)
            ymin = int(bbox.ymin * height)
            w = int(bbox.width * width)
            h = int(bbox.height * height)

            # Update the largest face detected
            a = w * h
            if a > face['a']:
                face['a'] = a
                face['x'] = xmin + w // 2  # Center x-coordinate of the face
                face['y'] = ymin + h // 2  # Center y-coordinate of the face
            
            # Print the bounding box locations to the console
            print(f"Face Found! X: {xmin}, Y: {ymin}, Width: {w}, Height: {h}")

            # Draw a bounding box rectangle on the screen
            cv2.rectangle(frame, (xmin, ymin), (xmin + w, ymin + h), (0, 255, 0), 2)


    dimensions = (w_disp, h_disp)
    # Resize the raw image
    scaled_image = cv2.resize(frame, dimensions, interpolation=cv2.INTER_LINEAR)
    cv2.imshow("scaled_image", scaled_image)

    a = face['a']
    x = face['x']
    y = face['y']

    react = ""
    if a > 600 :
        if x > w_mid * 1.3 :
            react+="R"
        elif x < w_mid * 0.7 :
            react+="L"
        else :
            react+=""

        if y > h_mid * 1.3 :
            react+="D"
        elif y < h_mid * 0.7 :
            react+="U"
        else :
            react+=""

    # process face status 
    # L = move head right, R = move head left
    # U = move head up, D = move head down
    # LU, LD, RU RD face detected move head to center it
    # CC = dont move head the face is centered
    # "" = face is not detected, scan for a face
    if react!="" :
        if react!="CC" :
            print(react)
            robie_head.write(react.encode('ascii')) 
            do_face_det_motions = True
            motion_time_last = time.monotonic()
        else : 
            det_str = ""
            #print("CC",end="\r")
            if do_face_det_motions :
                det_str += f"WE{pose}"
                robie_body.write("WE".encode('ascii'))
                robie_body.write(str(pose).encode('ascii'))
                pose = pose +1
                if pose == 9 : pose=5 #poses 5,6,7,8
            print(det_str)
            # set timer to make motions every 5 seconds when face is centered
            if (time.monotonic() - motion_time_last) >  motion_time_period:
                do_face_det_motions = True
                motion_time_last = time.monotonic()
            else :
                do_face_det_motions = False
    else :
        # No face detected move head to look for a face
        do_face_det_motions = True
        motion_time_last = time.monotonic()
        scan_str = ""
        if scanlr_cnt < scanlr_cnt_max :
            # scan Right and Left
            scanlr_cnt+=1
            if scan_left : 
                scan_str +="L"
                if scanlr_cnt==scanlr_cnt_max : scan_left=False
            else : 
                scan_str +="R"
                if scanlr_cnt==scanlr_cnt_max : scan_left=True
        if scanlr_cnt==scanlr_cnt_max :
            scanlr_cnt = 0  
            if scanud_cnt < scanud_cnt_max:
                # scan Up and Down
                scanud_cnt+=1
                if scan_up:
                    scan_str +="U"
                    if scanud_cnt==scanud_cnt_max : scan_up=False
                else:
                    scan_str +="D"
                    if scanud_cnt==scanud_cnt_max : scan_up=True
            if scanud_cnt == scanud_cnt_max :
                scanud_cnt = 0

        print(scan_str)
        robie_head.write(scan_str.encode('ascii'))

    # if (operating_system=="windows"):
    #     c = getch()
    #     if msvcrt.kbhit():
    #         c = msvcrt.getch()
    #         print(c)
    #         robie_serial.write(c)       
    # elif(operating_system=="linux"):
    #     if sys.stdin in select([sys.stdin], [], [], 0)[0]:
    #         c = getch()
    #         print(c)
    #         robie_serial.write(c.encode('ascii'))
            
    if cv2.waitKey(loop_delay) & 0xff == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
face_detection.close()
