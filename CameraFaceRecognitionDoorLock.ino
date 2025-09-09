#include "esp_camera.h"
#include <WiFi.h>

// ===== Camera model =====
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// ===== Relay config =====
#define Relay 33            // Use GPIO33 for relay IN
// Most relay modules are Active-LOW (IN=LOW -> ON)
// If your relay is Active-HIGH, swap RELAY_ON / RELAY_OFF.
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// ===== Wi-Fi =====
const char* ssid     = "-----------"; // WiFi SSID
const char* password = "----------";  // WiFi Password

void startCameraServer();

// Set to true from app_httpd.cpp when a face is recognized
volatile bool matchFace = false;
bool activateRelay = false;
unsigned long prevMillis = 0;
const unsigned long interval = 5000UL; // Relay ON duration (ms)

void setup() {
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, RELAY_OFF); // relay off by default

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // ===== Camera config =====
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size   = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count     = 2;
  } else {
    config.frame_size   = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count     = 1;
  }

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    while (true) delay(1000);
  }

  // Adjust sensor settings
  sensor_t* s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }
  s->set_framesize(s, FRAMESIZE_QVGA); // faster FPS

  // Wi-Fi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Start camera web server
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  // On face match, turn relay ON for 'interval' ms
  if (matchFace && !activateRelay) {
    activateRelay = true;
    digitalWrite(Relay, RELAY_ON);
    prevMillis = millis();
  }

  // After timeout, turn relay OFF and reset flags
  if (activateRelay && (millis() - prevMillis > interval)) {
    activateRelay = false;
    matchFace = false;
    digitalWrite(Relay, RELAY_OFF);
  }
}
