import cv2
import os
import time

# Define the ESP32-CAM streaming URL
ESP32_URL = "http://<ESP32_IP>:81/stream"  # Replace <ESP32_IP> with actual IP

# Define the output directory
output_dir = r"Data acquisition\frames"

# Ensure the directory exists
os.makedirs(output_dir, exist_ok=True)

# Open the video stream
cap = cv2.VideoCapture(ESP32_URL)

if not cap.isOpened():
    print("Error: Could not open video stream")
    exit()

frame_count = 0

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Failed to capture frame")
            break

        # Save frame as an image
        frame_path = os.path.join(output_dir, f"frame_{frame_count:04d}.jpg")
        cv2.imwrite(frame_path, frame)
        print(f"Saved {frame_path}")

        frame_count += 1
        time.sleep(0.5)  # Adjust the interval between frame captures

except KeyboardInterrupt:
    print("\nStopping frame capture.")

finally:
    cap.release()
    cv2.destroyAllWindows()
