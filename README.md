# Hệ thống quan trắc IOT sử dụng ESP32

## Công nghệ sử dụng
### Phần cứng
-ESP32
-YOLO-uno

### Database
- Adafruit IO

### Backend
- Node.js
- Express.js
- MongoDB

### Frontend
- React.js
- Tailwing

- 
## Cài Đặt và Chạy

### Yêu Cầu Hệ Thống
- Node.js (v14 trở lên)
- Adafruit IO
- npm

### Cài Đặt Backend
```bash
cd backend
npm install
```

### Cài Đặt Frontend
```bash
cd frontend
npm install
```

### Cấu Hình Môi Trường
1. Tạo file `.env` trong thư mục backend:
```
AIO_USERNAME= Ten_nguoi_dung_Adafruit_IO

AIO_KEY=Khoa_API_Adafruit_IO

FEED_HTL      = 'humidity-temperature-light',         


FEED_HTL_CTRL = 'humidity-temperature-light-control', 
FEED_LED      = 'led',                            
FEED_LED_NEO  = 'led-neo',                           
FEED_WIFI     = 'wifi-id-password',                  
FEED_CYCLE    = 'chu-ky',                             

PORT      = 3001,
MQTT_URL  = 'mqtt://io.adafruit.com',
MQTT_PORT = 1883

```

2. Tạo file `aio.h` trong thư mục YoloUNO_PlatformIO-RTOS_Project\include\:
```
#define MY_AIO_USERNAME  "Ten_nguoi_dung_Adafruit_IO"     
#define MY_AIO_KEY       "Khoa_API_Adafruit_IO"  
```

3. Tạo các feed trên Adafruit:
```
humidity-temperature-light         
humidity-temperature-light-control
led
led-neo
wifi-id-password
chu-ky
```
### Chạy Ứng Dụng
1. chạy YOLOUNO
2. Chạy backend:
```bash
cd backend
npm run dev
```
3. Chạy frontend:
```bash
cd frontend
npm start
