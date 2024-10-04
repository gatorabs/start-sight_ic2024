import cv2
import numpy as np
import time
import serial


serial_port = 'COM5'
baud_rate = 115200
ser = serial.Serial(serial_port, baud_rate)

cap = cv2.VideoCapture(0)

p_time = 0
fps = 0

while True:
    ret, frame = cap.read()

    if not ret:
        break

    c_time = time.time()
    time_elap = c_time - p_time
    p_time = c_time
    if time_elap > 0:
        fps = 1.0 / time_elap

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = cv2.medianBlur(gray, 5)

    circle = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, dp=1.2,
                              minDist=30, param1=50, param2=30, minRadius=15,
                              maxRadius=20)

    if circle is not None:
        circle = np.round(circle[0, :]).astype("int")
        x, y, r = circle[0]  # 1 círculo detectado
        cv2.circle(frame, (x, y), r, (0, 255, 0), 4)
        cv2.circle(frame, (x, y), 5, (0, 128, 255), -1)
        cv2.putText(frame, f'FPS: {fps:.2f}', (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
        print(f'Círculo detectado: centro= ({x}, {y}), raio= {r}')


        ser.write(b'2')
    else:

        ser.write(b'1')

    cv2.imshow("Esferas Real Time", frame)

    if cv2.waitKey(1) & 0xFF == ord('s'):
        break

cap.release()
cv2.destroyAllWindows()
ser.close()
