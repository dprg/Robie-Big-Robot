import cv2

# create a new cam object
cap = cv2.VideoCapture(1)
# initialize the face recognizer (default face haar cascade)
#face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")
face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt.xml")
#face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt2.xml")

_, image = cap.read()
dim = image.shape
h = dim[0]
w = dim[1]
h_mid = h/2
w_mid = w/2

while True:
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

    react = []
    if a > 1000 :
        if x > w_mid * 1.1 :
            react.append('R')
        elif x < w_mid * 0.9 :
            react.append('L')
        else :
            react.append('C')

        if y > h_mid * 1.1 :
            react.append('D')
        elif y < h_mid * 0.9 :
            react.append('U')
        else :
            react.append('C')

    # process face status 
    # L = move head right, D = move head down
    # C = dont move head it is centered
    # - = head not detected, maybe go to center after time out
    print(react)

    cv2.imshow("image", image)
    if cv2.waitKey(1) == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
