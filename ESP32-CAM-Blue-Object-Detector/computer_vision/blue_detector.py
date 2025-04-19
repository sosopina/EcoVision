import cv2
import requests
import numpy as np
import time  # Added for cooldown

# Replace with your ESP32-CAM's IP address
stream_url = "http://192.168.0.10/picture"
led_url_base = "http://192.168.0.10/led?state="

# Blue color range in HSV (adjust these values as needed)
lower_blue = np.array([90, 50, 50])
upper_blue = np.array([130, 255, 255])

# Detection settings
min_area = 1000       # Minimum contour area to consider as detection
led_cooldown = 0.5    # Minimum seconds between LED state changes

# State tracking
prev_state = None
last_led_change = 0   # Time of last LED state change

while True:
    try:
        # Fetch the image from ESP32-CAM
        response = requests.get(stream_url, stream=True)
        if response.status_code == 200:
            # Process image
            img_array = np.asarray(bytearray(response.content), dtype=np.uint8)
            frame = cv2.imdecode(img_array, cv2.IMREAD_COLOR)
            
            # Flip frame if needed (depends on camera orientation)
            # frame = cv2.flip(frame, 1)
            
            # Convert to HSV color space
            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            
            # Create mask for blue color
            mask = cv2.inRange(hsv, lower_blue, upper_blue)
            
            # Remove noise
            mask = cv2.erode(mask, None, iterations=2)
            mask = cv2.dilate(mask, None, iterations=2)
            
            # Find contours
            contours, _ = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            detected = False
            if len(contours) > 0:
                largest_contour = max(contours, key=cv2.contourArea)
                if cv2.contourArea(largest_contour) > min_area:
                    detected = True
                    x, y, w, h = cv2.boundingRect(largest_contour)
                    cv2.rectangle(frame, (x, y), (x+w, y+h), (255, 0, 0), 2)

            # LED Control with Cooldown
            current_time = time.time()
            if detected:
                if prev_state != "on" and (current_time - last_led_change) > led_cooldown:
                    try:
                        requests.get(led_url_base + "on", timeout=1)
                        prev_state = "on"
                        last_led_change = current_time
                    except:
                        pass
            else:
                if prev_state != "off" and (current_time - last_led_change) > led_cooldown:
                    try:
                        requests.get(led_url_base + "off", timeout=1)
                        prev_state = "off"
                        last_led_change = current_time
                    except:
                        pass

            # Display
            cv2.imshow('Blue Detection', frame)
            cv2.imshow('Mask', mask)
            
            if cv2.waitKey(1) == ord('q'):
                break
        else:
            print("Failed to fetch image")
            break
    except Exception as e:
        print(f"Error: {e}")
        break

cv2.destroyAllWindows()
