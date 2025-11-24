// server.js
import 'dotenv/config';
import express from 'express';
import cors from 'cors';
import { createServer } from 'http';
import { Server as SocketIOServer } from 'socket.io';
import mqtt from 'mqtt';
import path from 'path';
import { fileURLToPath } from 'url';

/* ---------- PATH ---------- */
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

/* ---------- ENV & FEED KEYS ---------- */
const {
  AIO_USERNAME,
  AIO_KEY,

  // SENSOR
  FEED_HTL      = 'humidity-temperature-light',          // JSON sensor

  // CONTROL
  FEED_HTL_CTRL = 'humidity-temperature-light-control',  // JSON {temperature,humidity,light}
  FEED_LED      = 'led',                                 // "ON"/"OFF"
  FEED_LED_NEO  = 'led-neo',                             // JSON {r,g,b,on}
  FEED_WIFI     = 'wifi-id-password',                    // JSON {ssid,password}
  FEED_CYCLE    = 'chu-ky',                              // "10" (giây)

  PORT      = 3001,
  MQTT_URL  = 'mqtt://io.adafruit.com',
  MQTT_PORT = 1883,
} = process.env;

if (!AIO_USERNAME || !AIO_KEY) {
  console.error('[ENV ERROR] AIO_USERNAME/AIO_KEY chưa được cấu hình trong .env');
}

/* ---------- HTTP + WS ---------- */
const app = express();
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

const httpServer = createServer(app);
const io = new SocketIOServer(httpServer, {
  cors: { origin: '*' },
});

/* ---------- MQTT (Adafruit IO) ---------- */
const mqttClient = mqtt.connect(MQTT_URL, {
  port: Number(MQTT_PORT),
  username: AIO_USERNAME,
  password: AIO_KEY,
  reconnectPeriod: 2000,
});

mqttClient.on('connect', () => {
  console.log(`MQTT connected -> ${MQTT_URL}:${MQTT_PORT}`);

  // SUB toàn bộ để đọc sensor JSON
  const topic = `${AIO_USERNAME}/feeds/+`;
  mqttClient.subscribe(topic, (err) => {
    if (err) console.error('Subscribe error:', topic, err.message);
    else console.log('Subscribed:', topic);
  });
});

/* ===== ESP → AIO → BE → FE (sensor) ===== */
mqttClient.on('message', (topic, payload) => {
  try {
    const valueStr = payload.toString().trim();
    const feed = topic.split('/').pop(); // ví dụ 'humidity-temperature-light'

    console.log('[MQTT] recv:', { topic, feed, valueStr });

    // chỉ quan tâm feed sensor JSON
    if (feed !== FEED_HTL) return;

    let parsed;
    try {
      parsed = JSON.parse(valueStr);
    } catch (e) {
      console.warn('Invalid JSON from HTL feed:', valueStr);
      return;
    }

    const { temperature, humidity, light } = parsed;
    const at = Date.now();

    const tempNum  = Number(temperature);
    const humidNum = Number(humidity);
    const lightNum = Number(light);

    // emit 3 event giống như ngày xưa 3 feed lẻ
    if (Number.isFinite(tempNum)) {
      io.emit('telemetry', { at, feed: 'temperature', value: tempNum });
    }
    if (Number.isFinite(humidNum)) {
      io.emit('telemetry', { at, feed: 'humidity', value: humidNum });
    }
    if (Number.isFinite(lightNum)) {
      io.emit('telemetry', { at, feed: 'light', value: lightNum });
    }
  } catch (e) {
    console.error('Handle message error:', e.message);
  }
});

mqttClient.on('reconnect', () => console.log('MQTT reconnecting...'));
mqttClient.on('close', () => console.log('MQTT connection closed'));
mqttClient.on('error', (err) => console.error('MQTT error:', err.message));

/* ---------- Helper publish ---------- */
function publishString(feedKey, value, label = '') {
  if (!feedKey) return;
  const topic = `${AIO_USERNAME}/feeds/${feedKey}`;
  mqttClient.publish(topic, String(value), { qos: 0 }, (err) => {
    if (err) console.error(`Publish ${label || feedKey} error:`, err.message);
    else console.log(`Published ${label || feedKey}:`, value);
  });
}

function publishJson(feedKey, obj, label = '') {
  publishString(feedKey, JSON.stringify(obj), label);
}

/* ---------- SENSOR CONTROL STATE (HTL_CTRL) ---------- */
// 1 = bật (ESP gửi giá trị thật), 0 = tắt (ESP gửi 0)
let sensorState = {
  temperature: 1,
  humidity: 1,
  light: 1,
};

function publishSensorControl(label = 'HTL_CTRL') {
  publishJson(
    FEED_HTL_CTRL,
    {
      temperature: sensorState.temperature,
      humidity: sensorState.humidity,
      light: sensorState.light,
    },
    label
  );
}

/* ===== FE → BE → AIO → ESP (control) ===== */
io.on('connection', (socket) => {
  console.log('WS client connected');

  // 1) LED 1 (bật/tắt) - dùng trong Config.jsx
  socket.on('config-led1', (payload) => {
    const on = (typeof payload === 'string')
      ? (payload === 'ON' || payload === '1')
      : !!payload;
    publishString(FEED_LED, on ? 'ON' : 'OFF', 'LED1');
  });

  // 2) LED Neo: { r, g, b, on } - dùng trong Config.jsx
  socket.on('config-neo', (data) => {
    if (!data) return;
    const r = Number(data.r) || 0;
    const g = Number(data.g) || 0;
    const b = Number(data.b) || 0;
    const on = data.on !== false; // default: true
    publishJson(
      FEED_LED_NEO,
      { r, g, b, on: on ? 1 : 0 },
      'LED_NEO',
    );
  });

  // 3) Chu kỳ gửi: FE dùng ms → gửi lên AIO dạng "giây"
  socket.on('config-cycle', (msOrSec) => {
    const num = Number(msOrSec);
    if (!Number.isFinite(num) || num <= 0) return;
    const seconds = num >= 1000 ? Math.round(num / 1000) : num;
    publishString(FEED_CYCLE, seconds, 'CYCLE');
  });

  // 4) WiFi: { ssid, password }
  socket.on('config-wifi', ({ ssid, password }) => {
    if (!ssid) return;
    publishJson(
      FEED_WIFI,
      { ssid, password: password || '' },
      'WIFI_ID_PASSWORD',
    );
  });

  // 5a) Bật/tắt nhiều sensor cùng lúc (Config.jsx dùng)
  socket.on('config-sensor-enable', (cfg) => {
    if (!cfg) return;
    sensorState.temperature = cfg.temp  ? 1 : 0;
    sensorState.humidity    = cfg.humid ? 1 : 0;
    sensorState.light       = cfg.light ? 1 : 0;
    publishSensorControl('HTL_CTRL (config-sensor-enable)');
  });

  // 5b) Từng card riêng lẻ:
  //    Humidity.jsx → socket.emit("set-humid", "ON"/"OFF")
  socket.on('set-humid', (payload) => {
    const on = (typeof payload === 'string')
      ? (payload === 'ON' || payload === '1')
      : !!payload;
    sensorState.humidity = on ? 1 : 0;
    publishSensorControl('HTL_CTRL (set-humid)');
  });

  //    Temperature.jsx → socket.emit("set-temp", "ON"/"OFF")
  socket.on('set-temp', (payload) => {
    const on = (typeof payload === 'string')
      ? (payload === 'ON' || payload === '1')
      : !!payload;
    sensorState.temperature = on ? 1 : 0;
    publishSensorControl('HTL_CTRL (set-temp)');
  });

  //    Light.jsx → socket.emit("set-light", "ON"/"OFF")
  socket.on('set-light', (payload) => {
    const on = (typeof payload === 'string')
      ? (payload === 'ON' || payload === '1')
      : !!payload;
    sensorState.light = on ? 1 : 0;
    publishSensorControl('HTL_CTRL (set-light)');
  });

  socket.on('disconnect', () => {
    console.log('WS client disconnected');
  });
});

/* ---------- START ---------- */
httpServer.listen(Number(PORT), () => {
  console.log(`BE running: http://localhost:${PORT}`);
});
