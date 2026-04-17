from picamera2 import Picamera2
import cv2
import json
import time

ZONE_FILE = "zone.json"

drawing = False
ix, iy = -1, -1
pending_zone = None
zone_timer_start = None

def mouse_event(event, x, y, flags, param):
    global ix, iy, drawing, pending_zone, zone_timer_start
    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        ix, iy = x, y
    elif event == cv2.EVENT_LBUTTONUP:
        drawing = False
        pending_zone = [ix, iy, x, y]
        zone_timer_start = time.time()

def save_zone(zone):
    with open(ZONE_FILE, "w") as f:
        json.dump(zone, f)
    print(f"Zone saved to {ZONE_FILE}")

picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"format":"RGB888","size":(640,480)}))
picam2.start()

cv2.namedWindow("Draw Zone")
cv2.setMouseCallback("Draw Zone", mouse_event)

while True:
    frame = picam2.capture_array()

    if pending_zone:
        cv2.rectangle(frame, (pending_zone[0], pending_zone[1]),
                             (pending_zone[2], pending_zone[3]), (0,0,255), 2)

    cv2.imshow("Draw Zone", frame)

    if pending_zone and zone_timer_start:
        if time.time() - zone_timer_start > 5:
            save_zone(pending_zone)
            break

    if cv2.waitKey(1) & 0xFF == 27:
        break

cv2.destroyAllWindows()
picam2.stop()
