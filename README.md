# 🏠 Hệ thống Giám sát & Điều khiển Môi trường Realtime  
*(ESP32 – FreeRTOS – Adafruit IO – MERN Stack)*

Đây là repository chứa toàn bộ mã nguồn cho hệ thống **giám sát và điều khiển môi trường theo thời gian thực**, sử dụng:

- **ESP32 / FreeRTOS** làm thiết bị IoT.
- **Adafruit IO** làm MQTT Broker & Cloud Dashboard.
- **MERN Stack** (MongoDB – Express – React – Node.js) cho hệ thống Web Realtime (Backend + Frontend).

---

## 1. Kiến trúc tổng quan

Dự án gồm 3 phần chính:

1. **IoT Core** – Firmware cho ESP32  
   - Thư mục: `YoloUNO_PlatformIO-RTOS_Project/`  
   - Viết bằng C++ (PlatformIO + FreeRTOS).  
   - Gửi dữ liệu cảm biến (nhiệt độ, độ ẩm, ánh sáng) lên Adafruit IO qua MQTT.  
   - Nhận lệnh điều khiển (LED, enable/disable auto control, …) từ Cloud.

2. **Backend** – Server Realtime (Node.js/Express) *(đang phát triển)*  
   - Thư mục: `adafruit-realtime/be/`  
   - Dự kiến: nhận dữ liệu từ Adafruit IO / MQTT / Webhook, lưu vào DB (MongoDB), cung cấp REST API / WebSocket cho Frontend.

3. **Frontend** – Web Dashboard *(đang phát triển)*  
   - Thư mục: `adafruit-realtime/fe/`  
   - Dự kiến: giao diện realtime hiển thị biểu đồ, lịch sử, và điều khiển thiết bị.

---

## 2. Cấu trúc thư mục

```bash
.
├── YoloUNO_PlatformIO-RTOS_Project/     # Mã nguồn nhúng (ESP32 / PlatformIO)
│   ├── src/                             # coreiot.cpp (logic MQTT + FreeRTOS)
│   └── include/                         # aio.h (các hằng số, secrets WiFi & AIO)
│
├── adafruit-realtime/
│   ├── be/                              # Backend server (Node.js / Express) – planned
│   └── fe/                              # Frontend web app (React/Vue/…) – planned
│
└── .gitignore                          # Bỏ qua file nhạy cảm (API keys, node_modules, …)
