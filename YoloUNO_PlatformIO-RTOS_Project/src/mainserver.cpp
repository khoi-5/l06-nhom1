#include "mainserver.h"

// ================== Biến toàn cục riêng của mainserver ==================

WebServer server(80);
bool isAPMode = true;  // true = đang ở AP mode cấu hình WiFi

// ================== HTML MAIN PAGE ==================

String mainPage() {
  float temperature = glob_temperature;
  float humidity    = glob_humidity;
  float light       = glob_light;

  String led1 = digitalRead(LED1_PIN) ? "ON" : "OFF";
  String led2 = digitalRead(LED2_PIN) ? "ON" : "OFF";

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>ESP32 Dashboard</title>
  <style>
    body {
      font-family: "Segoe UI", Arial, sans-serif;
      background: linear-gradient(135deg, #58a6ff, #8ec5ff);
      margin: 0;
      padding: 0;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh;
      color: #333;
    }

    .glass {
      background: rgba(255, 255, 255, 0.25);
      backdrop-filter: blur(14px) saturate(180%);
      -webkit-backdrop-filter: blur(14px) saturate(180%);
      border-radius: 22px;
      border: 1px solid rgba(255, 255, 255, 0.35);
      padding: 35px;
      width: 90%;
      max-width: 430px;
      text-align: center;
      box-shadow: 0 8px 32px rgba(0,0,0,0.2);
      animation: fadeIn 0.6s ease-in-out;
    }

    @keyframes fadeIn {
      from { opacity: 0; transform: translateY(15px); }
      to   { opacity: 1; transform: translateY(0); }
    }

    h1 {
      color: #ffffff;
      font-size: 2.1em;
      font-weight: 700;
      margin-bottom: 25px;
      text-shadow: 0 0 12px rgba(255,255,255,0.8);
    }

    .sensorRow {
      display: flex;
      justify-content: space-between;
      background: rgba(255,255,255,0.35);
      padding: 14px 18px;
      margin: 10px 0;
      border-radius: 14px;
      font-size: 1.15em;
      font-weight: 600;
      color: #003d80;
      box-shadow: inset 0 0 8px rgba(0,0,0,0.1);
      animation: pop 0.4s ease;
    }

    @keyframes pop {
      from { transform: scale(0.95); opacity: 0.5; }
      to { transform: scale(1); opacity: 1; }
    }

    button {
      margin: 12px 8px;
      background: linear-gradient(135deg, #007bff, #00c6ff);
      color: #fff;
      border: none;
      border-radius: 25px;
      padding: 12px 28px;
      font-size: 1.05em;
      cursor: pointer;
      transition: 0.25s;
      font-weight: 600;
      box-shadow: 0 4px 12px rgba(0,0,0,0.2);
    }

    button:hover {
      transform: scale(1.09);
      box-shadow: 0 6px 18px rgba(0,0,0,0.25);
    }

    #settings {
      background: rgba(255,255,255,0.75);
      color: #005eff;
    }

    #settings:hover {
      background: rgba(255,255,255,0.9);
    }
  </style>
</head>
<body>

  <!-- Popup chỉnh màu LED 2 -->
  <div id="led2Modal" style="
      position: fixed; inset: 0;
      background: rgba(0,0,0,0.4);
      display: none;
      align-items: center;
      justify-content: center;
      z-index: 999;
    ">
    <div style="
        background: #ffffff;
        border-radius: 16px;
        padding: 20px;
        min-width: 260px;
        box-shadow: 0 8px 24px rgba(0,0,0,0.3);
        color: #333;
      ">
      <h3 style="margin-top:0; text-align:center;">Chỉnh màu LED 2</h3>

      <div style="margin-bottom:10px;">
        <label>Đỏ (0–255): <span id="rLabel">255</span></label>
        <input id="rVal" type="range" min="0" max="255" value="255" style="width:100%;"
               oninput="updateLed2Live()">
      </div>

      <div style="margin-bottom:10px;">
        <label>Xanh lá (0–255): <span id="gLabel">255</span></label>
        <input id="gVal" type="range" min="0" max="255" value="255" style="width:100%;"
               oninput="updateLed2Live()">
      </div>

      <div style="margin-bottom:10px;">
        <label>Xanh dương (0–255): <span id="bLabel">255</span></label>
        <input id="bVal" type="range" min="0" max="255" value="255" style="width:100%;"
               oninput="updateLed2Live()">
      </div>

      <!-- ô preview màu -->
      <div id="colorPreview" style="
          margin-top:12px;
          height:32px;
          border-radius:16px;
          border:1px solid #ccc;
          background: rgb(255,255,255);
        ">
      </div>

      <button style="margin-top:16px; width:100%; background:#ccc; color:#333;"
              onclick="closeLed2Dialog()">Đóng</button>

      <button style="margin-top:10px; width:100%; background:#ff4d4f;"
              onclick="turnOffLed2()">Tắt LED 2</button>
    </div>
  </div>

  <div class="glass">
    <h1>📡 ESP32 Realtime Dashboard</h1>

    <div class="sensorRow">🌡️ Nhiệt độ: <span id="temp">--</span> °C</div>
    <div class="sensorRow">💧 Độ ẩm: <span id="hum">--</span> %</div>
    <div class="sensorRow">🌞 Ánh sáng: <span id="light">--</span> lx</div>

    <button onclick="toggleLED(1)">💡 LED 1: <span id="l1">OFF</span></button>
    <button onclick="openLed2Dialog()">💡 LED 2: <span id="l2">OFF</span></button>

    <button id="settings" onclick="window.location='/settings'">⚙️ Cài đặt</button>
  </div>

  <script>
    function toggleLED(id) {
      fetch(`/toggle?led=${id}`)
        .then(res => res.json())
        .then(data => {
          document.getElementById('l1').innerText = data.led1;
          document.getElementById('l2').innerText = data.led2;
        });
    }

    function openLed2Dialog() {
      document.getElementById('led2Modal').style.display = 'flex';
      updateLed2Preview();
    }

    function closeLed2Dialog() {
      document.getElementById('led2Modal').style.display = 'none';
    }

    function updateLed2Preview() {
      const r = document.getElementById('rVal').value || 0;
      const g = document.getElementById('gVal').value || 0;
      const b = document.getElementById('bVal').value || 0;

      document.getElementById('rLabel').innerText = r;
      document.getElementById('gLabel').innerText = g;
      document.getElementById('bLabel').innerText = b;

      document.getElementById('colorPreview').style.background =
        `rgb(${r}, ${g}, ${b})`;
    }

    function updateLed2Live() {
      updateLed2Preview();

      const r = document.getElementById('rVal').value || 0;
      const g = document.getElementById('gVal').value || 0;
      const b = document.getElementById('bVal').value || 0;

      fetch(`/toggle?led=2&r=${r}&g=${g}&b=${b}`)
        .then(res => res.json())
        .then(data => {
          document.getElementById('l1').innerText = data.led1;
          document.getElementById('l2').innerText = data.led2;
        });
    }

    function turnOffLed2() {
      fetch(`/toggle?led=2&off=1`)
        .then(res => res.json())
        .then(data => {
          document.getElementById('l1').innerText = data.led1;
          document.getElementById('l2').innerText = data.led2;
          closeLed2Dialog();
        });
    }

    setInterval(() => {
      fetch('/sensors')
        .then(res => res.json())
        .then(data => {
          document.getElementById('temp').innerText = data.temp;
          document.getElementById('hum').innerText = data.hum;
          document.getElementById('light').innerText = data.light;
        });
    }, 1500);
  </script>

</body>
</html>
)rawliteral";

  // (hiện giờ mình không chèn sẵn led1/led2 vào HTML, vì JS sẽ cập nhật)
  return html;
}

// ==================== TRANG CÀI ĐẶT (HOÀN CHỈNH) ====================
String settingsPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Cài đặt Wi-Fi & Chu kỳ</title>
  <style>
    body {font-family: "Segoe UI",Arial,sans-serif;background: linear-gradient(135deg,#667eea,#764ba2);margin:0;padding:20px;min-height:100vh;display:flex;align-items:center;justify-content:center;color:#fff;}
    .container {width:100%;max-width:520px;background:rgba(255,255,255,0.25);border-radius:24px;padding:32px;box-shadow:0 12px 40px rgba(0,0,0,0.3);backdrop-filter:blur(16px);}
    h1 {font-size:2.3em;margin-bottom:30px;text-align:center;}
    .columns {display:flex;gap:20px;flex-wrap:wrap;margin-bottom:25px;}
    .column {flex:1;min-width:280px;background:rgba(255,255,255,0.22);padding:22px;border-radius:18px;box-shadow:0 4px 15px rgba(0,0,0,0.2);}
    h2 {margin:0 0 20px 0;font-size:1.45em;text-align:center;}
    input, select {width:100%;padding:14px;margin:11px 0;border:none;border-radius:12px;font-size:1.1em;box-sizing:border-box;box-shadow:inset 0 2px 8px rgba(0,0,0,0.15);}
    .btn {width:100%;padding:15px;margin-top:15px;border:none;border-radius:30px;font-size:1.15em;font-weight:600;cursor:pointer;background:linear-gradient(135deg,#00f2c3,#009c7d);color:#002d22;transition:all 0.3s;box-shadow:0 6px 20px rgba(0,0,0,0.25);}
    .btn:hover {transform:translateY(-3px);box-shadow:0 10px 28px rgba(0,0,0,0.35);}
    .back-btn {background:rgba(255,255,255,0.85);color:#5533cc;margin-top:15px;}
    #msg {margin-top:20px;font-weight:600;min-height:28px;text-align:center;}
  </style>
</head>
<body>
  <div class="container">
    <h1>⚙️ Cài đặt Wi-Fi & CoreIOT</h1>

    <div class="columns">
      <!-- Cột Wi-Fi -->
      <div class="column">
        <h2>Wi-Fi</h2>
        <input type="text" id="ssid" placeholder="Tên Wi-Fi (SSID)" required />
        <input type="password" id="pass" placeholder="Mật khẩu Wi-Fi" />
        <button class="btn" onclick="connectWiFi()">Kết nối Wi-Fi</button>
      </div>

      <!-- Cột Chu kỳ gửi -->
      <div class="column">
        <h2>Chu kỳ gửi lên CoreIOT</h2>
        <select id="cycle">
          <option value="5000">5 giây</option>
          <option value="10000">10 giây (mặc định)</option>
          <option value="15000">15 giây</option>
          <option value="30000">30 giây</option>
          <option value="60000">60 giây</option>
        </select>

        <button class="btn" onclick="saveCycle()">Cập nhật chu kỳ</button>

        <div style="margin-top:15px;font-size:1.1em;">
          Đang gửi mỗi: <b id="currentCycle">10</b> giây
        </div>
      </div>
    </div>

    <button class="btn back-btn" onclick="window.location='/'">← Quay lại Dashboard</button>
    <div id="msg"></div>
  </div>

  <script>
    // Tải chu kỳ hiện tại khi mở trang
    fetch('/getcycle')
      .then(r => r.text())
      .then(ms => {
        const msVal = parseInt(ms) || 10000;
        const sec = Math.round(msVal / 1000);
        document.getElementById('cycle').value = msVal;
        document.getElementById('currentCycle').innerText = sec;
      });

    function connectWiFi() {
      const ssid = document.getElementById('ssid').value.trim();
      const pass = document.getElementById('pass').value;
      if (!ssid) return alert("Vui lòng nhập SSID!");

      fetch(`/connect?ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}`)
        .then(r => r.text())
        .then(msg => {
          document.getElementById('msg').innerHTML = `<span style="color:#0f0">✓ ${msg}</span>`;
        });
    }

    function saveCycle() {
      const ms = document.getElementById('cycle').value;
      fetch(`/setcycle?ms=${ms}`)
        .then(r => r.text())
        .then(msg => {
          document.getElementById('msg').innerHTML = `<span style="color:#0f0">✓ ${msg}</span>`;
          document.getElementById('currentCycle').innerText = Math.round(ms/1000);
          setTimeout(() => document.getElementById('msg').innerHTML = '', 5000);
        });
    }
  </script>
</body>
</html>
  )rawliteral";
}

// ================== WiFi & Server ==================

void startAP() {
  isAPMode = true;
  isWifiConnected = false;

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), password.c_str());
  IPAddress ip = WiFi.softAPIP();

  Serial.print("[WiFi] AP mode. SSID: ");
  Serial.print(ssid);
  Serial.print("  IP: ");
  Serial.println(ip);
  Serial.print(" ");
}

void connectToWiFi() {
  if (wifi_ssid.isEmpty()) {
    Serial.println("[WiFi] Chưa có wifi_ssid, giữ AP mode.");
    Serial.print(" ");
    startAP();
    return;
  }

  isAPMode        = false;
  isWifiConnected = false;

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

  Serial.print("[WiFi] Đang kết nối tới ");
  Serial.println(wifi_ssid);
  Serial.print(" ");

  unsigned long start = millis();
  unsigned long timeout = 15000;  // 15s

  while (WiFi.status() != WL_CONNECTED && millis() - start < timeout) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WiFi] Đã kết nối, IP: ");
    Serial.println(WiFi.localIP());
    Serial.print(" ");
    isWifiConnected = true;

    if (xBinarySemaphoreInternet != NULL) {
      xSemaphoreGive(xBinarySemaphoreInternet);
    }
  } else {
    Serial.println("[WiFi] Kết nối thất bại, quay lại AP mode.");
    Serial.print(" ");
    startAP();
  }
}

void setupServer() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BOOT_PIN, INPUT_PULLUP);

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  // Trang chính
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", mainPage());
  });

  // Trang settings
  server.on("/settings", HTTP_GET, []() {
    server.send(200, "text/html", settingsPage());
  });

  // API trả về cảm biến
  server.on("/sensors", HTTP_GET, []() {
    String json = "{";
    json += "\"temp\":" + String(glob_temperature, 1) + ",";
    json += "\"hum\":"  + String(glob_humidity, 1) + ",";
    json += "\"light\":" + String(glob_light, 0);
    json += "}";
    server.send(200, "application/json", json);
  });

  // API toggle LED + chỉnh màu (hiện tại chỉ bật/tắt digital cho đơn giản)
  server.on("/toggle", HTTP_GET, []() {
    int led = server.hasArg("led") ? server.arg("led").toInt() : 0;

    if (led == 1) {
      bool state = !digitalRead(LED1_PIN);
      digitalWrite(LED1_PIN, state);
    } else if (led == 2) {
      if (server.hasArg("off") && server.arg("off") == "1") {
        digitalWrite(LED2_PIN, LOW);
      } else {
        // Ở đây bạn có thể gọi hàm set màu NeoPixel nếu muốn.
        // Tạm thời: chỉ bật LED2 digital.
        digitalWrite(LED2_PIN, HIGH);
      }
    }

    String json = "{";
    json += "\"led1\":\"";
    json += digitalRead(LED1_PIN) ? "ON" : "OFF";
    json += "\",\"led2\":\"";
    json += digitalRead(LED2_PIN) ? "ON" : "OFF";
    json += "\"}";
    server.send(200, "application/json", json);
  });

  // API lấy chu kỳ (ms) – ánh xạ từ chu_ky (giây) trong global.cpp
  server.on("/getcycle", HTTP_GET, []() {
    int ms = chu_ky * 1000;   // global chu_ky đang để 10 (giây), mình convert sang ms
    server.send(200, "text/plain", String(ms));
  });

  // API set chu kỳ (ms) – lưu lại theo giây vào chu_ky
  server.on("/setcycle", HTTP_GET, []() {
    if (server.hasArg("ms")) {
      int ms = server.arg("ms").toInt();
      if (ms < 1000) ms = 1000;
      chu_ky = ms / 1000;   // lưu dạng giây để không phá logic code khác

      server.send(200, "text/plain",
                  "Đã cập nhật chu kỳ thành " + String(ms) + " ms");
    } else {
      server.send(400, "text/plain", "Thiếu tham số ms");
    }
  });

  // API kết nối WiFi từ trang settings
  server.on("/connect", HTTP_GET, []() {
    if (!server.hasArg("ssid")) {
      server.send(400, "text/plain", "Thiếu tham số ssid");
      return;
    }

    wifi_ssid     = server.arg("ssid");
    wifi_password = server.hasArg("pass") ? server.arg("pass") : "";

    Serial.print("[WiFi] SSID mới (GET): ");
    Serial.println(wifi_ssid);

    connectToWiFi();

    String msg;
    if (isWifiConnected) {
      msg = "Đã kết nối tới " + wifi_ssid;
    } else {
      msg = "Kết nối thất bại, vẫn ở AP mode";
    }
    server.send(200, "text/plain", msg);
  });

  // Giữ /wifi POST nếu sau này bạn còn dùng
  server.on("/wifi", HTTP_POST, []() {
    if (server.hasArg("ssid") && server.hasArg("pass")) {
      wifi_ssid     = server.arg("ssid");
      wifi_password = server.arg("pass");

      Serial.print("[WiFi] SSID mới (POST): ");
      Serial.println(wifi_ssid);
      Serial.print(" ");

      server.send(200, "text/html",
                  "<html><body><h3>Đã nhận cấu hình Wi-Fi.</h3>"
                  "<p>ESP32 đang thử kết nối...</p>"
                  "<p><a href='/'>Về Dashboard</a></p></body></html>");

      connectToWiFi();
    } else {
      server.send(400, "text/plain", "Thiếu tham số ssid/pass");
    }
  });

  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("[HTTP] WebServer đã khởi động ");
  Serial.print(" ");
}

// ================== FreeRTOS Task ==================

void main_server_task(void *pvParameters) {
  (void) pvParameters;

  // Ban đầu cho chạy AP để cấu hình
  startAP();
  setupServer();

  for (;;) {
    server.handleClient();

    // Nhấn giữ BOOT 3s để reset về AP mode
    static unsigned long pressedAt = 0;
    if (digitalRead(BOOT_PIN) == LOW) {
      if (pressedAt == 0) pressedAt = millis();
      if (millis() - pressedAt > 3000) {
        Serial.println("[BTN] Giữ BOOT 3s -> Reset về AP mode");
        WiFi.disconnect(true);
        startAP();
        pressedAt = 0;
      }
    } else {
      pressedAt = 0;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
