// src/App.jsx
import React, { useEffect, useMemo, useRef, useState } from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  Tooltip,
  Legend,
  ResponsiveContainer,
} from "recharts";
import { getSocket } from "./socket";

// ---------- Small card ----------
function StatCard({ title, value, unit }) {
  return (
    <div
      style={{
        background: "#fff",
        borderRadius: 16,
        padding: 16,
        boxShadow: "0 8px 20px rgba(0,0,0,.06)",
      }}
    >
      <div style={{ fontSize: 13, color: "#666", marginBottom: 6 }}>{title}</div>
      <div style={{ display: "flex", alignItems: "baseline", gap: 8 }}>
        <div style={{ fontSize: 40, fontWeight: 700 }}>
          {value === null || value === undefined ? "--" : value.toFixed(2)}
        </div>
        <div style={{ color: "#666" }}>{unit}</div>
      </div>
    </div>
  );
}

// ---------- Error boundary cho Chart ----------
class ChartErrorBoundary extends React.Component {
  constructor(props) {
    super(props);
    this.state = { hasError: false };
  }
  static getDerivedStateFromError() {
    return { hasError: true };
  }
  componentDidCatch(error, info) {
    console.error("Chart crashed:", error, info);
  }
  render() {
    if (this.state.hasError) {
      return (
        <div style={{ padding: 16, color: "#b91c1c" }}>
          Chart error – data invalid, but app vẫn chạy.
        </div>
      );
    }
    return this.props.children;
  }
}

export default function App() {
  console.log("Render App at", new Date().toLocaleTimeString());

  const [last, setLast] = useState({
    temperature: null,
    humidity: null,
    light: null,
  });
  const [rows, setRows] = useState([]);
  const maxPoints = 60;

  // trạng thái gửi lệnh (để highlight khi bấm)
  const [ledLastCmd, setLedLastCmd] = useState(null);
  const [ledActive, setLedActive] = useState(null);

  const [lightLastCmd, setLightLastCmd] = useState(null);
  const [lightActive, setLightActive] = useState(null);

  const [envLastCmd, setEnvLastCmd] = useState(null);
  const [envActive, setEnvActive] = useState(null);

  const lastRef = useRef(last);
  lastRef.current = last;

  useEffect(() => {
    const socket = getSocket();

    const handler = ({ at, feed, value }) => {
      console.log("Telemetry from BE:", { at, feed, value, type: typeof value });

      if (!["temperature", "humidity", "light"].includes(feed)) return;
      if (typeof value !== "number" || !Number.isFinite(value)) {
        console.warn("Ignore invalid value:", feed, value);
        return;
      }

      setLast((prev) => ({ ...prev, [feed]: value }));

      const time = new Date(at).toLocaleTimeString();

      setRows((prev) => {
        if (!prev.length || prev[prev.length - 1].time !== time) {
          const base = {
            time,
            temperature: lastRef.current.temperature,
            humidity: lastRef.current.humidity,
            light: lastRef.current.light,
          };
          const next = [...prev, base].slice(-maxPoints);
          next[next.length - 1][feed] = value;
          return next;
        } else {
          const next = [...prev];
          next[next.length - 1] = {
            ...next[next.length - 1],
            [feed]: value,
          };
          return next;
        }
      });
    };

    socket.on("telemetry", handler);
    return () => socket.off("telemetry", handler);
  }, []);

  // data sạch cho chart
  const chartData = useMemo(
    () =>
      rows.map((r) => ({
        time: r.time,
        temperature:
          typeof r.temperature === "number" && Number.isFinite(r.temperature)
            ? r.temperature
            : null,
        humidity:
          typeof r.humidity === "number" && Number.isFinite(r.humidity)
            ? r.humidity
            : null,
        light:
          typeof r.light === "number" && Number.isFinite(r.light)
            ? r.light
            : null,
      })),
    [rows]
  );

  const socket = getSocket();

  // ----- LED handlers -----
  const onLedOn = () => {
    setLedActive("ON");
    socket.emit("set-led", "ON");
    setLedLastCmd("ON");
    setTimeout(() => setLedActive(null), 150);
  };

  const onLedOff = () => {
    setLedActive("OFF");
    socket.emit("set-led", "OFF");
    setLedLastCmd("OFF");
    setTimeout(() => setLedActive(null), 150);
  };

  // ----- LIGHT handlers -----
  const onLightOn = () => {
    setLightActive("ON");
    socket.emit("set-light", "ON");
    setLightLastCmd("ON");
    setTimeout(() => setLightActive(null), 150);
  };

  const onLightOff = () => {
    setLightActive("OFF");
    socket.emit("set-light", "OFF");
    setLightLastCmd("OFF");
    setTimeout(() => setLightActive(null), 150);
  };

  // ----- ENV (humidity + temperature) handlers -----
  const onEnvOn = () => {
    setEnvActive("ON");
    socket.emit("set-env", "ON");
    setEnvLastCmd("ON");
    setTimeout(() => setEnvActive(null), 150);
  };

  const onEnvOff = () => {
    setEnvActive("OFF");
    socket.emit("set-env", "OFF");
    setEnvLastCmd("OFF");
    setTimeout(() => setEnvActive(null), 150);
  };

  return (
    <div style={{ background: "#f6f7fb", minHeight: "100vh", padding: 16 }}>
      <h2 style={{ marginBottom: 12 }}>Adafruit IO – Realtime Dashboard</h2>

      {/* Cards */}
      <div
        style={{
          display: "grid",
          gridTemplateColumns: "repeat(auto-fit, minmax(220px,1fr))",
          gap: 16,
          marginBottom: 16,
        }}
      >
        <StatCard title="Temperature" value={last.temperature} unit="°C" />
        <StatCard title="Humidity" value={last.humidity} unit="%" />
        <StatCard title="Light" value={last.light} unit="lux" />
      </div>

      {/* Controls */}
      <div
        style={{
          background: "#fff",
          borderRadius: 16,
          padding: 16,
          boxShadow: "0 8px 20px rgba(0,0,0,.06)",
          marginBottom: 16,
        }}
      >
        <div style={{ fontSize: 13, color: "#666", marginBottom: 8 }}>
          Controls
        </div>

        {/* LED row */}
        <div style={{ marginBottom: 8 }}>
          <span style={{ fontSize: 13, color: "#555", marginRight: 8 }}>LED:</span>
          <button
            onClick={onLedOn}
            style={{
              background: "#22c55e",
              color: "#fff",
              border: "none",
              borderRadius: 12,
              padding: "10px 16px",
              marginRight: 8,
              cursor: "pointer",
              boxShadow:
                ledActive === "ON" ? "0 0 0 3px #111 inset" : "0 0 0 0 transparent",
            }}
          >
            LED ON
          </button>
          <button
            onClick={onLedOff}
            style={{
              background: "#ef4444",
              color: "#fff",
              border: "none",
              borderRadius: 12,
              padding: "10px 16px",
              cursor: "pointer",
              boxShadow:
                ledActive === "OFF" ? "0 0 0 3px #111 inset" : "0 0 0 0 transparent",
            }}
          >
            LED OFF
          </button>
          <span style={{ marginLeft: 12, fontSize: 12, color: "#666" }}>
            Last: {ledLastCmd ?? "—"}
          </span>
        </div>

        {/* LIGHT row */}
        <div style={{ marginBottom: 8 }}>
          <span style={{ fontSize: 13, color: "#555", marginRight: 8 }}>
            LIGHT CTRL:
          </span>
          <button
            onClick={onLightOn}
            style={{
              background: "#0ea5e9",
              color: "#fff",
              border: "none",
              borderRadius: 12,
              padding: "10px 16px",
              marginRight: 8,
              cursor: "pointer",
              boxShadow:
                lightActive === "ON"
                  ? "0 0 0 3px #111 inset"
                  : "0 0 0 0 transparent",
            }}
          >
            LIGHT ON
          </button>
          <button
            onClick={onLightOff}
            style={{
              background: "#6366f1",
              color: "#fff",
              border: "none",
              borderRadius: 12,
              padding: "10px 16px",
              cursor: "pointer",
              boxShadow:
                lightActive === "OFF"
                  ? "0 0 0 3px #111 inset"
                  : "0 0 0 0 transparent",
            }}
          >
            LIGHT OFF
          </button>
          <span style={{ marginLeft: 12, fontSize: 12, color: "#666" }}>
            Last: {lightLastCmd ?? "—"}
          </span>
        </div>

        {/* ENV row */}
        <div>
          <span style={{ fontSize: 13, color: "#555", marginRight: 8 }}>
            HUM/TEMP CTRL:
          </span>
          <button
            onClick={onEnvOn}
            style={{
              background: "#22c55e",
              color: "#fff",
              border: "none",
              borderRadius: 12,
              padding: "10px 16px",
              marginRight: 8,
              cursor: "pointer",
              boxShadow:
                envActive === "ON"
                  ? "0 0 0 3px #111 inset"
                  : "0 0 0 0 transparent",
            }}
          >
            ON
          </button>
          <button
            onClick={onEnvOff}
            style={{
              background: "#ef4444",
              color: "#fff",
              border: "none",
              borderRadius: 12,
              padding: "10px 16px",
              cursor: "pointer",
              boxShadow:
                envActive === "OFF"
                  ? "0 0 0 3px #111 inset"
                  : "0 0 0 0 transparent",
            }}
          >
            OFF
          </button>
          <span style={{ marginLeft: 12, fontSize: 12, color: "#666" }}>
            Last: {envLastCmd ?? "—"}
          </span>
        </div>
      </div>

      {/* Chart */}
      <div
        style={{
          background: "#fff",
          borderRadius: 16,
          padding: 12,
          boxShadow: "0 8px 20px rgba(0,0,0,.06)",
        }}
      >
        <div style={{ fontSize: 13, color: "#666", marginBottom: 6 }}>
          Realtime (last ~60 ticks)
        </div>
        <div style={{ width: "100%", height: 340 }}>
          <ChartErrorBoundary>
            <ResponsiveContainer>
              <LineChart data={chartData}>
                <XAxis dataKey="time" minTickGap={30} />
                <YAxis />
                <Tooltip />
                <Legend />
                <Line
                  type="monotone"
                  dataKey="temperature"
                  name="Temperature (°C)"
                  dot={false}
                  stroke="#ef4444"
                  strokeWidth={2}
                />
                <Line
                  type="monotone"
                  dataKey="humidity"
                  name="Humidity (%)"
                  dot={false}
                  stroke="#3b82f6"
                  strokeWidth={2}
                />
                <Line
                  type="monotone"
                  dataKey="light"
                  name="Light (lux)"
                  dot={false}
                  stroke="#f59e0b"
                  strokeWidth={2}
                />
              </LineChart>
            </ResponsiveContainer>
          </ChartErrorBoundary>
        </div>
      </div>
    </div>
  );
}
