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
cap = cv2.VideoCapture(2) #Robie Cam

# initialize the face recognizer (default face haar cascade)
#face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")
face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt.xml")
#face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt2.xml")
hasCamera = False

try:    
    _, image = cap.read()
    print(cap.read())
    dim = image.shape
    h = dim[0]
    w = dim[1]
    h_mid = h/2
    w_mid = w/2
    hasCamera = True
except:
    print("Unable to open camera")

while True and hasCamera:
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

    #print(dim, face)
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
    # L = move head right, D = move head down
    # C = dont move head it is centered
    # - = head not detected, maybe go to center after time out
    if react!="" :
        if react!="CC" :
            print(react)
            robie_serial.write(react.encode('ascii')) 
            time.sleep(0.5)
        else : 
            print("CC",end="\r")
    #else :
    #    print(".",end="")

    if (operating_system=="windows"):
        c = getch()
        if msvcrt.kbhit():
            c = msvcrt.getch()
            print(c)
            robie_serial.write(c)       
    elif(operating_system=="linux"):
        if sys.stdin in select([sys.stdin], [], [], 0)[0]:
            c = getch()
            print(c)
            robie_serial.write(c.encode('ascii'))
            
    cv2.imshow("image", image)

    if cv2.waitKey(1) == ord("q"):
        break

    #s = robie_serial.readline()
    #print(s)


cap.release()
cv2.destroyAllWindows()
