# ESP32-CAM Blue Color Detection System

## Real-Time Object Detection Using Computer Vision and IoT

### Project Overview
This system combines an ESP32-CAM microcontroller with Python-based computer vision to detect blue objects and control an LED remotely. The system operates in three stages:

1. **Image Capture**: ESP32-CAM captures JPEG frames
2. **Wireless Transmission**: Frames stream via WiFi to Python client
3. **Color Processing**: OpenCV analyzes frames for blue objects
4. **Feedback Control**: Python triggers ESP32-CAM's LED based on detection


---

## How It Works

### I. ESP32-CAM Operation (`esp32_cam.ino`)

#### 1. Camera Initialization
```cpp
camera_config_t config = {
  .pixel_format = PIXFORMAT_JPEG,
  .frame_size = FRAMESIZE_QVGA, // 320x240 resolution
  .jpeg_quality = 10, // Lower = better quality
  .fb_count = 1 // Single frame buffer
};
esp_camera_init(&config);
```
- Uses OV2640 camera sensor at 20MHz clock speed
- Captures QVGA (320x240) resolution for optimal performance
- JPEG compression reduces bandwidth usage

#### 2. WiFi Connection
- Implements STA (Station) mode to connect to existing WiFi
- Automatic restart if connection fails
- IP address displayed via serial monitor

#### 3. HTTP Server
Two endpoints:
- `/picture`: Returns JPEG image buffer
```cpp
camera_fb_t *fb = esp_camera_fb_get();
httpd_resp_send(req, (const char *)fb->buf, fb->len);
```
- `/led`: Controls GPIO4 (Built-in LED)
```cpp
digitalWrite(LED_PIN, (strcmp(state, "on") == 0) ? HIGH : LOW);
```

#### 4. Key Hardware Specifications
| Parameter             | Value              |
|-----------------------|--------------------|
| Image Resolution      | 320x240 (QVGA)     |
| Frame Rate            | 5-10 FPS           |
| WiFi Standard         | 802.11 b/g/n       |
| LED Control Latency   | < 100ms            |

---

### II. Python Color Detection (`blue_detection.py`)

#### 1. Image Acquisition
```python
response = requests.get(stream_url, stream=True)
img = cv2.imdecode(np.frombuffer(response.content, np.uint8), cv2.IMREAD_COLOR)
```
- Fetches JPEG frames via HTTP GET requests
- Decodes using OpenCV's optimized image parser

#### 2. HSV Color Space Conversion
```python
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
```
- Why HSV? Better for color segmentation than RGB
- Hue (0-179): Color type (Blue = 90-130)
- Saturation (0-255): Color intensity
- Value (0-255): Brightness

#### 3. Color Masking
```python
mask = cv2.inRange(hsv, lower_blue, upper_blue)
```
- Creates binary mask:
  - 1 (White): Pixels in blue range
  - 0 (Black): Other pixels

#### 4. Noise Reduction
```python
mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
```
- Morphological Operations:
  - Erosion: Removes small white noise
  - Dilation: Fills holes in detected regions

#### 5. Contour Detection
```python
contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
```
- Finds object boundaries in the mask
- Filters contours by area (`min_area=500` pixels)

#### 6. LED Control Logic
```python
requests.get(led_url + ("on" if detected else "off"))
```
- State machine prevents redundant HTTP calls
- Debouncing logic avoids rapid on/off toggling

---

## Technical Details: Color Detection

### A. HSV Threshold Ranges
| Color | Hue Range | Saturation | Value   |
|-------|-----------|------------|---------|
| Blue  | 90-130    | 50-255     | 50-255  |

**How to Adjust:**
- **Light Objects**: Increase saturation lower bound
- **Dark Environments**: Decrease value lower bound
- **Different Colors**: Use HSV color picker tools

### B. Performance Optimization
- **ESP32-CAM:**
```cpp
config.jpeg_quality = 10;
config.frame_size = FRAMESIZE_QVGA;
```
- **Python:**
```python
response = requests.get(..., timeout=2)
```

### C. Detection Accuracy Factors
- **Lighting Consistency**: Avoid shadows/glare
- **Camera Focus**: Ensure clear image capture
- **White Balance**:
```cpp
sensor_t *s = esp_camera_sensor_get();
s->set_whitebal(s, 1); // 1 = auto white balance
```

---

## Advanced Configuration

### 1. Real-Time HSV Calibration
```python
cv2.namedWindow('Thresholds')
cv2.createTrackbar('H Min', 'Thresholds', 90, 179, lambda x: None)
cv2.createTrackbar('H Max', 'Thresholds', 130, 179, lambda x: None)
# Add S/V trackbars...

# In main loop:
h_min = cv2.getTrackbarPos('H Min', 'Thresholds')
lower_blue = np.array([h_min, s_min, v_min])
```

### 2. Multi-Color Detection
```python
# Detect red
lower_red = np.array([0, 50, 50])
upper_red = np.array([10, 255, 255])
red_mask = cv2.inRange(hsv, lower_red, upper_red)

# Combine masks
total_mask = blue_mask | red_mask
```

### 3. Object Tracking
```python
M = cv2.moments(contour)
cx = int(M["m10"] / M["m00"])
cy = int(M["m01"] / M["m00"])
cv2.circle(img, (cx, cy), 5, (0,255,0), -1)
```

---

## Troubleshooting Guide

| Issue                | Solution                           |
|---------------------|------------------------------------|
| No Camera Feed      | Check 5V power supply stability     |
| Intermittent Detection | Increase min_area threshold     |
| High Latency        | Reduce image resolution to CIF      |
| False Positives     | Tighten HSV bounds                  |
| WiFi Drops          | Use shorter password (WPA2)         |

---
