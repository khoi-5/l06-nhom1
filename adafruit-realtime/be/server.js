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

/* ---------- ENV ---------- */
const {
  AIO_USERNAME,
  AIO_KEY,
  FEED_TEMP  = 'temperature',
  FEED_HUMID = 'humidity',
  FEED_LIGHT = 'light',

  FEED_LED        = 'led',
  FEED_LIGHT_CTRL = 'light_control',
  FEED_HT_CTRL    = 'humidity_temperature_control',

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

  // Sub tất cả feed của user
  const topic = `${AIO_USERNAME}/feeds/+`;
  mqttClient.subscribe(topic, (err) => {
    if (err) {
      console.error('Subscribe error:', topic, err.message);
    } else {
      console.log('Subscribed:', topic);
    }
  });
});

// chỉ đẩy 3 feed sensor sang FE
mqttClient.on('message', (topic, payload) => {
  try {
    const valueStr = payload.toString().trim();
    const feed = topic.split('/').pop(); // ví dụ 'temperature'

    console.log('[MQTT] recv:', { topic, feed, valueStr });

    if (![FEED_TEMP, FEED_HUMID, FEED_LIGHT].includes(feed)) {
      // control feed thì FE không cần
      return;
    }

    const num = Number(valueStr);
    if (!Number.isFinite(num)) {
      console.warn('Invalid numeric value from feed', feed, 'payload =', valueStr);
      return;
    }

    const data = { at: Date.now(), feed, value: num };
    io.emit('telemetry', data);
  } catch (e) {
    console.error('Handle message error:', e.message);
  }
});

mqttClient.on('reconnect', () => console.log('MQTT reconnecting...'));
mqttClient.on('close', () => console.log('MQTT connection closed'));
mqttClient.on('error', (err) => console.error('MQTT error:', err.message));

/* ---------- Helper publish ---------- */
function publishControl(feedKey, desired, label) {
  if (!feedKey) {
    console.warn(`No feed key configured for ${label}`);
    return;
  }
  const topic = `${AIO_USERNAME}/feeds/${feedKey}`;
  const msg = typeof desired === 'string' ? desired : (desired ? 'ON' : 'OFF');

  mqttClient.publish(topic, msg, { qos: 0 }, (err) => {
    if (err) console.error(`Publish ${label} err:`, err.message);
    else console.log(`${label} command published:`, msg);
  });
}

/* ---------- WS: nhận lệnh từ FE ---------- */
io.on('connection', (socket) => {
  console.log('WS client connected');

  // LED
  socket.on('set-led', (desired) => {
    publishControl(FEED_LED, desired, 'LED');
  });

  // LIGHT control
  socket.on('set-light', (desired) => {
    publishControl(FEED_LIGHT_CTRL, desired, 'LIGHT_CTRL');
  });

  // Humidity + Temperature control
  socket.on('set-env', (desired) => {
    publishControl(FEED_HT_CTRL, desired, 'HT_CTRL');
  });

  socket.on('disconnect', () => {
    console.log('WS client disconnected');
  });
});

/* ---------- START ---------- */
httpServer.listen(Number(PORT), () => {
  console.log(`BE running: http://localhost:${PORT}`);
});
