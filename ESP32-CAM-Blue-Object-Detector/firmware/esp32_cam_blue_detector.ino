#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"

// Select camera model (AI Thinker ESP32-CAM)
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// WiFi credentials
const char *ssid = "Orange-1A86";
const char *password = "8J76322882";

// HTTP server for streaming and control
httpd_handle_t server = NULL;

#define LED_PIN 4   // Define the pin controlling the LED

// Function prototypes
void startServer();

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Initialize LED pin as output and ensure it starts off
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Camera configuration (unchanged)
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;  // Use JPEG for streaming
  config.frame_size = FRAMESIZE_QVGA;      // 320x240
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_DRAM;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x", err);
    return;
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the HTTP server (registers both /picture and /led)
  startServer();

  // Print the ESP32-CAM's IP address
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("/picture' to view the image");
}

void loop() {
  // Everything is handled in the HTTP server.
  delay(10000);
}

// Handler to serve a single picture
static esp_err_t picture_handler(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  httpd_resp_send(req, (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return ESP_OK;
}

// Handler to control the LED on pin 4 via query parameter (state=on/off)
static esp_err_t led_handler(httpd_req_t *req) {
  char* buf;
  size_t buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*) malloc(buf_len);
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      char state[4];
      if (httpd_query_key_value(buf, "state", state, sizeof(state)) == ESP_OK) {
        if (strcmp(state, "on") == 0) {
          digitalWrite(LED_PIN, HIGH);
          httpd_resp_sendstr(req, "LED ON");
        } else if (strcmp(state, "off") == 0) {
          digitalWrite(LED_PIN, LOW);
          httpd_resp_sendstr(req, "LED OFF");
        } else {
          httpd_resp_sendstr(req, "Invalid state");
        }
      }
    }
    free(buf);
  } else {
    httpd_resp_sendstr(req, "No query string");
  }
  return ESP_OK;
}

// Start the HTTP server and register URI handlers
void startServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t picture_uri = {
    .uri = "/picture",
    .method = HTTP_GET,
    .handler = picture_handler,
    .user_ctx = NULL
  };

  httpd_uri_t led_uri = {
    .uri = "/led",
    .method = HTTP_GET,
    .handler = led_handler,
    .user_ctx = NULL
  };

  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &picture_uri);
    httpd_register_uri_handler(server, &led_uri);
  }
}
