🏠 Hệ thống Giám sát và Điều khiển Môi trường Realtime (ESP32 - Adafruit IO - MERN Stack)

Đây là repository chứa toàn bộ mã nguồn cho hệ thống giám sát và điều khiển môi trường theo thời gian thực, sử dụng chip ESP32/FreeRTOS, dịch vụ Adafruit IO (MQTT Broker), và giao diện người dùng dựa trên Web.

Dự án bao gồm 3 phần chính:

IoT Core (YoloUNO_PlatformIO-RTOS_Project): Mã nguồn C++ cho ESP32.

Backend (adafruit-realtime/be): (Dự kiến) Dịch vụ server Node.js.

Frontend (adafruit-realtime/fe): (Dự kiến) Giao diện người dùng Web.

1. Cấu trúc Dự án

Dự án này được chia thành các thư mục chính sau:

.
├── YoloUNO_PlatformIO-RTOS_Project/ # Mã nguồn nhúng (ESP32/PlatformIO)
│   ├── src/                         # Chứa coreiot.cpp (logic MQTT/FreeRTOS)
│   └── include/                     # Chứa aio.h (các hằng số, secrets)
├── adafruit-realtime/
│   ├── be/                          # Backend Server (Node.js/Express)
│   └── fe/                          # Frontend Web App (React/Vue/etc.)
└── .gitignore                       # File quan trọng để bảo mật khóa API


2. Thiết lập Môi trường Nhúng (IoT Core)

Phần này hướng dẫn cách thiết lập và nạp code cho chip ESP32.

2.1. Yêu cầu Phần cứng & Phần mềm

Phần mềm: Visual Studio Code (VS Code) với Extension PlatformIO.

Phần cứng: ESP32 Development Board, Cảm biến môi trường (DHT22/BME280), Cảm biến ánh sáng (LDR), LED.

2.2. Cấu hình Secrets (Bắt buộc)

Để thiết bị kết nối với Adafruit IO, bạn phải cung cấp thông tin đăng nhập trong file include/aio.h.

⚠️ QUAN TRỌNG: File aio.h đã được thêm vào .gitignore để tránh bị lộ khóa API.

Tạo hoặc chỉnh sửa file YoloUNO_PlatformIO-RTOS_Project/include/aio.h với nội dung sau:

#define MY_WIFI_SSID    "Tên_WiFi_Của_Bạn"
#define MY_WIFI_PASS    "Mật_khẩu_WiFi_Của_Bạn"

#define MY_AIO_USERNAME "Tên_người_dùng_Adafruit_IO" 
#define MY_AIO_KEY      "Khóa_API_Adafruit_IO_của_bạn" 


2.3. Cấu hình Code

Chân LED: Đảm bảo chân LED (LED_PIN) được định nghĩa đúng trong file cấu hình (ví dụ: 2 cho LED on-board của nhiều board ESP32).

Tên Feeds: Tên các feed đã được định nghĩa trong coreiot.cpp: temperature, humidity, light, led, light_control, humidity_temperature_control.

3. Hoạt động của IoT Core (coreiot.cpp)

Logic hoạt động của thiết bị được quản lý bởi các Task FreeRTOS và giao thức MQTT.

3.1. Kết nối & Tái kết nối (Hàm reconnect())

Hàm reconnect() được thiết kế để chống lại lỗi mất kết nối (như lỗi rc=6 do Client ID bị từ chối) bằng cách:

Tạo Client ID Ngẫu nhiên: Sử dụng MAC Address của ESP32 và nối thêm một số ngẫu nhiên (random(0xffff)) để đảm bảo ID luôn độc nhất trong mỗi lần thử kết nối.

Độ trễ: Chờ 10 giây (vTaskDelay(pdMS_TO_TICKS(10000))) sau mỗi lần thất bại để Broker Adafruit IO có thời gian giải phóng Client ID cũ.

Thông tin đăng nhập: Sử dụng MY_AIO_USERNAME và MY_AIO_KEY (từ aio.h) để kết nối.

3.2. Vòng lặp Chính (Hàm coreiot_task())

Tần suất Gửi: Thiết bị gửi dữ liệu (Temperature, Humidity, Light) lên Adafruit IO mỗi 10 giây (PUSHED_WAITING_TIME).

Điều khiển: Thiết bị lắng nghe các feed điều khiển (như led, light_control) và thay đổi trạng thái của các biến điều khiển (g_lightEnable, g_envEnable) hoặc bật/tắt LED vật lý.

Logic Gửi:

Nếu g_envEnable = true, gửi giá trị cảm biến thật.

Nếu g_envEnable = false, gửi giá trị 0.0f để báo hiệu trạng thái Tắt trên Cloud/Giao diện.

4. Bảo mật (File .gitignore)

Để đảm bảo các khóa API không bị đẩy lên GitHub, file .gitignore đã được cấu hình như sau:

/adafruit-realtime/be/.env
/adafruit-realtime/be/node_modules/
/adafruit-realtime/fe/node_modules/
/YoloUNO_PlatformIO-RTOS_Project/include/aio.h


Nếu bạn có bất kỳ file nhạy cảm nào khác, hãy thêm chúng vào file này.
