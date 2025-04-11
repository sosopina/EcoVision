#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"
#include "camera_config.h"  // Pin definitions

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

httpd_handle_t server = NULL;
#define LED_PIN 4  // Built-in flash LED

void startCameraServer();

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  // Camera init (config in camera_config.h)
  if(esp_camera_init(&camera_config) != ESP_OK) {
    Serial.println("Camera init failed!");
    while(1);
  }

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) delay(500);
  
  startCameraServer();
  Serial.print("Stream URL: http://");
  Serial.println(WiFi.localIP());
}

void loop() { delay(10000); }

// HTTP Handlers (same as previous code)
// ... [Include all HTTP handlers from earlier] ...
