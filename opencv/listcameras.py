import cv2

def list_cameras(max_devices=10):
    available_cams = []
    for index in range(max_devices):
        cap = cv2.VideoCapture(index)
        if cap.read()[0]:
            print(f"Camera found at index {index}")
            available_cams.append(index)
        cap.release()
    if not available_cams:
        print("No cameras found.")
    return available_cams

list_cameras()
