# 📷 ESP32-CAM Face Recognition with Relay Control

# 📷 ESP32-CAM Face Recognition with Servo Control

## 📝 Overview
This project uses the **ESP32-CAM (AI-Thinker)** to perform **face recognition** and trigger a **servo motor** upon a successful match.  
Typical use case: unlocking a small door latch, opening a lock, or activating a mechanism only when a known face is detected.

---

## ⚙️ Requirements
- ESP32-CAM AI-Thinker  
- Servo Motor (e.g., SG90 / MG90S)  
- Stable 5V / 2A power supply (ESP32-CAM + Servo)  
- Arduino IDE or PlatformIO with ESP32 board support  

---

## 🔌 Wiring
| Component    | ESP32-CAM Pin |
|--------------|---------------|
| Servo Signal | GPIO2         |
| Servo VCC    | 5V (external supply recommended) |
| Servo GND    | GND (shared with ESP32-CAM)      |

⚠️ **Important:**  
- GPIO2 is a boot pin; make sure the servo does not pull it HIGH/LOW during reset.  
- Always use an **external 5V supply** for the servo and connect grounds together.  

---

## 📂 Project Files
- **sketch.ino** → Main Arduino sketch (camera + Wi-Fi + servo logic).  
- **camera_pins.h** → Pin mapping for ESP32-CAM (AI-Thinker).  
- **app_httpd.cpp** → Web server file (from CameraWebServer example) modified to set `matchFace = true` when a face match occurs.  

---

## 🚀 Setup & Usage
1. Open the sketch in Arduino IDE.  
2. Select **ESP32 Wrover Module** and enable **PSRAM**.  
3. Update your Wi-Fi credentials in the code:  
   ```cpp
   const char* ssid = "YOUR_SSID";
   const char* password = "YOUR_PASSWORD";
   ```
4. Upload the code to ESP32-CAM via USB-TTL.  
5. Open Serial Monitor @115200 baud → find your device IP.  
6. Access `http://<IP_ADDRESS>` in a browser.  
7. Enable **Face Recognition** from the web UI.  
8. When a registered face is detected, the servo rotates (e.g., 0° → 90° for 5 seconds).

---

## 🤖 Algorithms
Powered by [**esp-who**](https://github.com/espressif/esp-who) and the **esp-face** library:  
- **MTCNN** → Face Detection  
- **MobileFaceNet** → Feature Extraction  
- **kNN / Euclidean Distance** → Face Matching  

---

## 🔄 Process Flow

A high-level illustration of how the system works:

<img width="1536" height="1024" alt="9a669eb8-5d6b-48dc-a08a-021fb586a868" src="https://github.com/user-attachments/assets/887a0c7d-8fdb-48ef-a96d-71535dcb8076" />

1. ESP32-CAM captures an image.  
2. MTCNN detects faces.  
3. MobileFaceNet extracts embeddings.  
4. kNN compares with stored faces.  
5. Servo motor rotates if a match is found.

---

## ⚠️ Notes
- Relay is connected to **GPIO33**.  
- Most relay modules are **Active-LOW**. Adjust in code if yours is Active-HIGH:  
  ```cpp
  #define RELAY_ON  HIGH
  #define RELAY_OFF LOW
  ```
- Always power ESP32-CAM with a **5V/2A supply** for stable operation.  

---

## 📌 Applications
- Smart door locks  
- Face-activated switches  
- IoT security systems  
- Educational computer vision projects  
