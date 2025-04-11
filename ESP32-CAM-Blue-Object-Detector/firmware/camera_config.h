// AI-Thinker ESP32-CAM pin mapping
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27
#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34


camera_config_t camera_config = {
  .pixel_format = PIXFORMAT_JPEG,
  .frame_size = FRAMESIZE_QVGA,
  .jpeg_quality = 12,
  .fb_count = 1,
  .ledc_channel = LEDC_CHANNEL_0,
  .ledc_timer = LEDC_TIMER_0,
  .pin_d0 = Y2_GPIO_NUM,
  .pin_d1 = Y3_GPIO_NUM,
};
