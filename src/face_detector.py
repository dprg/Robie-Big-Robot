import cv2
import mediapipe as mp

# Initialize MediaPipe Face Detection
mp_face_detection = mp.solutions.face_detection
# Model selection: 0 for short-range (within 2 meters), 1 for long-range (up to 5 meters)
face_detection = mp_face_detection.FaceDetection(model_selection=0, min_detection_confidence=0.5)

# Open the default camera (0 is usually the integrated Pi Cam or first USB webcam)
cap = cv2.VideoCapture(0)

print("Starting face detection. Press 'q' in the window to quit...")

while cap.isOpened():
    success, frame = cap.read()
    if not success:
        print("Ignoring empty camera frame.")
        continue

    # Convert the BGR image to RGB for MediaPipe processing
    image_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = face_detection.process(image_rgb)

    # Get image dimensions to scale the normalized coordinates
    height, width, _ = frame.shape

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
            
            # Print the bounding box locations to the console
            print(f"Face Found! X: {xmin}, Y: {ymin}, Width: {w}, Height: {h}")

            # Draw a bounding box rectangle on the screen
            cv2.rectangle(frame, (xmin, ymin), (xmin + w, ymin + h), (0, 255, 0), 2)

    # Display the live video stream with overlay
    cv2.imshow('Raspberry Pi Face Detection', frame)

    # Break loop cleanly if 'q' key is pressed
    if cv2.waitKey(5) & 0xFF == ord('q'):
        break

# Clean up resources
cap.release()
cv2.destroyAllWindows()
face_detection.close()

B
B
B
B
B
B
B
B
B
B
B
B
B
B
A
A
A
A
A
B
B
B
B
B
B
B
B
B
B

