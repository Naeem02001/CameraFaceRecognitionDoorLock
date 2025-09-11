#include "esp_camera.h"
#include <WiFi.h>
#include <Servo.h>   // Servo library

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// ===== Servo Motor =====
#define SERVO_PIN 2    // Using GPIO2 for the servo
Servo myServo;

const char* ssid     = "-----------"; // WiFi SSID
const char* password = "----------";  // WiFi Password

void startCameraServer();

// This variable is changed to true from app_httpd.cpp when a face is recognized
volatile boolean matchFace = false;
boolean activateServo = false;
unsigned long prevMillis = 0;
const unsigned long interval = 5000UL; // Time the servo stays open (ms)

void setup() {
  myServo.attach(SERVO_PIN);
  myServo.write(0);   // Default position (closed)

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Camera configuration
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

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    while (true) delay(1000);
  }

  sensor_t * s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }
  s->set_framesize(s, FRAMESIZE_QVGA);

  // WiFi connection
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
  // If a face is matched and servo is not active
  if (matchFace && !activateServo) {
    activateServo = true;
    myServo.write(90);   // Open servo (90°)
    prevMillis = millis();
  }

  // After the interval, return servo to default position
  if (activateServo && (millis() - prevMillis > interval)) {
    activateServo = false;
    matchFace = false;
    myServo.write(0);    // Close servo (0°)
  }
}
