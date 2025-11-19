# 🏠 Hệ thống Giám Sát & Điều Khiển Môi Trường Realtime  
**(ESP32 – FreeRTOS – Adafruit IO – MERN Stack)**

Dự án này xây dựng một hệ thống **IoT giám sát và điều khiển môi trường theo thời gian thực**, gồm:

- **ESP32 / FreeRTOS**: đọc dữ liệu cảm biến & nhận lệnh điều khiển.
- **Adafruit IO (MQTT Broker)**: trung gian truyền dữ liệu giữa thiết bị và server/web.
- **MERN Stack** (MongoDB – Express – React – Node.js): backend + web dashboard realtime.

Hệ thống phù hợp cho các bài toán:
- Giám sát nhiệt độ, độ ẩm, ánh sáng trong phòng, nhà kính, lab.
- Bật/tắt thiết bị (LED, đèn, quạt…) từ xa qua web.
- Lưu lịch sử dữ liệu để phân tích xu hướng.

---

## 1. Kiến trúc tổng quan

Hệ thống gồm 3 phần chính:

1. **IoT Core – Firmware ESP32**
   - Đọc dữ liệu cảm biến.
   - Kết nối WiFi và Adafruit IO bằng MQTT.
   - Publish dữ liệu lên các feed (`temperature`, `humidity`, `light`, …).
   - Nhận lệnh điều khiển từ feed (`led`, `light_control`, `humidity_temperature_control`).

2. **Backend – Server Node.js/Express** *(đang phát triển)*
   - Nhận dữ liệu từ Adafruit IO (REST/MQTT/Webhook).
   - Lưu trữ vào MongoDB.
   - Cung cấp REST API / WebSocket cho frontend.

3. **Frontend – Web Dashboard** *(đang phát triển)*
   - Hiển thị realtime: temperature, humidity, light.
   - Điều khiển thiết bị: bật/tắt LED, enable/disable auto control.
   - Hiển thị biểu đồ & lịch sử dữ liệu.

---

## 2. Cấu trúc thư mục

```bash
.
├── YoloUNO_PlatformIO-RTOS_Project/       # Firmware ESP32 (PlatformIO + FreeRTOS)
│   ├── src/
│   │   └── coreiot.cpp                    # Logic chính: MQTT, FreeRTOS tasks, xử lý cảm biến
│   └── include/
│       └── aio.h                          # Thông tin WiFi & Adafruit IO (tự tạo, KHÔNG commit)
│
├── adafruit-realtime/
│   ├── be/                                # Backend Node.js/Express
│   │   └── .env                           # Biến môi trường backend (tự tạo, KHÔNG commit)
│   └── fe/                                # Frontend Web (React/Vite/…)
│       └── .env                           # Biến môi trường frontend (tự tạo, KHÔNG commit)
│
└── .gitignore                             # Bỏ qua file nhạy cảm & node_modules
