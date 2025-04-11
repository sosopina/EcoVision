import cv2
import requests
import numpy as np

stream_url = "http://***.***.*.***/picture"
led_url = "http://***.***.*.***/led?state="

# HSV Range for Blue
lower_blue = np.array([90, 50, 50])
upper_blue = np.array([130, 255, 255])

kernel = np.ones((5,5), np.uint8)
min_area = 500

def main():
    prev_state = None
    while True:
        try:
            response = requests.get(stream_url, timeout=2)
            if response.status_code == 200:
                img = cv2.imdecode(np.frombuffer(response.content, np.uint8), cv2.IMREAD_COLOR)
                hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
                mask = cv2.inRange(hsv, lower_blue, upper_blue)
                mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
                
                contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
                detected = False
                
                for cnt in contours:
                    if cv2.contourArea(cnt) > min_area:
                        x,y,w,h = cv2.boundingRect(cnt)
                        cv2.rectangle(img, (x,y), (x+w,y+h), (255,0,0), 2)
                        detected = True
                        
                if detected != prev_state:
                    requests.get(led_url + ("on" if detected else "off"))
                    prev_state = detected
                
                cv2.imshow('Detection', img)
                if cv2.waitKey(1) == ord('q'):
                    break
                    
        except Exception as e:
            print(f"Error: {e}")
            break

    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
