import cv2
from Model import *


def image_contouring(image):
    edges = cv2.Canny(image, edge_threshold_min, edge_threshold_max)
    return edges


window_pos = (450, 200)
window_size = 164
edge_threshold_min = 100
edge_threshold_max = 200

font = cv2.FONT_HERSHEY_SIMPLEX
fontScale = 1
fontColor = (0, 0, 255)
thickness = 3
lineType = 2

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    cv2.rectangle(frame, window_pos, (window_pos[0] + window_size, window_pos[1] + window_size), (0, 0, 255), 2)
    cropped_image = frame[window_pos[1]+2:window_pos[1] + window_size-2, window_pos[0]+2:window_pos[0] + window_size-2]
    edge_image = image_contouring(cropped_image)
    filename = "image.jpg"
    cv2.imwrite(filename, edge_image)
    img = cv2.imread('image.jpg')
    percent, val = predict_value(img)
    cv2.putText(frame, str(percent)+ '%'+str(val), (400,400), font, fontScale, fontColor, thickness, lineType)
    cv2.imshow('frame', frame)
    if cv2.waitKey(1) == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()