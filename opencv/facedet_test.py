import cv2
import serial
import time
from select import select
import sys

windowsPort = "com39"
linuxPort = "/dev/ttyACM0" 

def get_os():
    platform = sys.platform
    if platform == "win32" or platform == "cygwin":
        return "windows"
    elif platform.startswith("linux"):
        return "linux"
    else:
        return "other"

operating_system = get_os()
print(f"Operating system detected: {operating_system}")

if operating_system == "windows":
    print("Running on Windows")
    import msvcrt
    try:
        robie_serial = serial.Serial(windowsPort)
    except:
        print("Unable to open serial port")
        
elif operating_system == "linux":
    print("Running on Linux")
    import tty
    import termios
    try:
        robie_serial = serial.Serial(linuxPort)
    except:
        print("Unable to open serial port")
        
else:
    print("Running on an unknown or unsupported OS")
    # Code for other OSes (e.g., macOS)

def getch():
    if(operating_system != "linux"):
        return msvcrt.getch()
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch

#cap = cv2.VideoCapture(0) # Laptop Cam
cap = cv2.VideoCapture(1) #Robie Cam windows

# initialize the face recognizer (default face haar cascade)
#face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")
face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt.xml")
#face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt2.xml")
hasCamera = False

try:    
    _, image = cap.read()
#    print(cap.read())
    dim = image.shape
    h = dim[0]
    w = dim[1]
    h_mid = h/2
    w_mid = w/2
    hasCamera = True
except:
    print("Unable to open camera")

motion_time_last = time.monotonic()
do_face_det_motions = True

pose = 5
# period between make motions when a face is centered
motion_time_period = 7.0

scan_left = False
scanlr_cnt = 0
scanlr_cnt_max = 20
scan_up = False
scanud_cnt = 0
scanud_cnt_max = 10

loop_delay = 0.5

# seems like a delay is needed before interacting with Robie
time.sleep(2.0)

# position face toward center
# There is no feedback so go to each stop and back off
robie_serial.reset_input_buffer()
for n in range(int(scanud_cnt_max)) :
    print("D")
    robie_serial.write("D".encode('ascii'))
    time.sleep(loop_delay)
robie_serial.reset_input_buffer()
for n in range(int(scanud_cnt_max/3)) :
    print("U")
    robie_serial.write("U".encode('ascii'))
    time.sleep(loop_delay)
robie_serial.reset_input_buffer()
for n in range(int(scanlr_cnt_max)) :
    print("L")
    robie_serial.write("L".encode('ascii'))
    time.sleep(loop_delay)
robie_serial.reset_input_buffer()
for n in range(int(scanlr_cnt_max/2)) :
    print("R")
    robie_serial.write("R".encode('ascii'))
    time.sleep(loop_delay)

while True and hasCamera:
    robie_serial.reset_input_buffer()
    # read the image from the cam
    _, image = cap.read()
    # converting to grayscale
    image_gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    # detect all the faces in the image
    faces = face_cascade.detectMultiScale(image_gray, 1.3, 5)
    # for every face, draw a blue rectangle
    face = {'x': 0, 'y': 0, 'a': 0}
    for x, y, width, height in faces:
        cv2.rectangle(image, (x, y), (x + width, y + height), color=(255, 0, 0), thickness=2)
        a = width*height
        if(a>face['a']): 
            face['a'] = int(a)
            # Adjust xy to align with center of face square
            face['x'] = int(x) + width/2
            face['y'] = int(y) + height/2

    a = face['a']
    x = face['x']
    y = face['y']

    react = ""
    if a > 1000 :
        if x > w_mid * 1.1 :
            react+="R"
        elif x < w_mid * 0.9 :
            react+="L"
        else :
            react+="C"

        if y > h_mid * 1.1 :
            react+="D"
        elif y < h_mid * 0.9 :
            react+="U"
        else :
            react+="C"

    # process face status 
    # L = move head right, R = move head left
    # U = move head up, D = move head down
    # LU, LD, RU RD face detected move head to center it
    # CC = dont move head the face is centered
    # "" = face is not detected, scan for a face
    if react!="" :
        if react!="CC" :
            print(react)
            robie_serial.write(react.encode('ascii')) 
            do_face_det_motions = True
            motion_time_last = time.monotonic()
        else : 
            det_str = "CC"
            # print("CC",end="\r")
            if do_face_det_motions :
                det_str += f"WE{pose}"
                robie_serial.write("WE".encode('ascii'))
                robie_serial.write(str(pose).encode('ascii'))
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
        # No face detected scan for a face
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
        robie_serial.write(scan_str.encode('ascii'))

    # loop rate control
    time.sleep(loop_delay) 

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
            
    cv2.imshow("image", image)

    if cv2.waitKey(1) == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
