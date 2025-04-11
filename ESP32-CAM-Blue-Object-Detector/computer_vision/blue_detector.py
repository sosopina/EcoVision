import cv2
import numpy as np
import requests

class BlueDetector:
    def __init__(self, ip_address="192.168.1.100"):
        self.stream_url = f"http://{ip_address}/picture"
        self.led_url = f"http://{ip_address}/led?state="
        self.lower_blue = np.array([90, 50, 50])
        self.upper_blue = np.array([130, 255, 255])
        self.kernel = np.ones((5,5), np.uint8)
        
    def detect(self):
        while True:
            try:
                response = requests.get(self.stream_url, timeout=2)
                frame = cv2.imdecode(np.frombuffer(response.content, np.uint8), -1)
                
                # Detection pipeline
                hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
                mask = cv2.inRange(hsv, self.lower_blue, self.upper_blue)
                mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, self.kernel)
                
                # Find contours and control LED
                contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
                if any(cv2.contourArea(c) > 500 for c in contours):
                    requests.get(self.led_url + "on")
                    for c in contours:
                        x,y,w,h = cv2.boundingRect(c)
                        cv2.rectangle(frame, (x,y), (x+w,y+h), (255,0,0), 2)
                else:
                    requests.get(self.led_url + "off")
                
                cv2.imshow('Blue Detection', frame)
                if cv2.waitKey(1) == ord('q'): break
                    
            except Exception as e:
                print(f"Error: {e}")
                break

if __name__ == "__main__":
    detector = BlueDetector()
    detector.detect()
    cv2.destroyAllWindows()
