// src/components/Config.jsx
import React, { useEffect, useState } from "react";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faLightbulb,
  faSliders,
  faWifi,
} from "@fortawesome/free-solid-svg-icons";
import { getSocket } from "../socket";

const socket = getSocket();

function Config() {
  // ===== LED state =====
  const [led1, setLed1] = useState(false);
  const [led2, setLed2] = useState(false);
  const [neoColor, setNeoColor] = useState({ r: 255, g: 255, b: 255 });

  // ===== Chu kỳ CoreIOT (ms) =====
  const [cycleMs, setCycleMs] = useState(10000);
  const [currentCycleMs, setCurrentCycleMs] = useState(10000);

  // ===== WiFi config =====
  const [ssid, setSsid] = useState("");
  const [password, setPassword] = useState("");
  const [wifiMsg, setWifiMsg] = useState("");

  const [error, setError] = useState("");

  // Nếu muốn load chu kỳ hiện tại từ BE (ví dụ BE trả qua REST hoặc socket),
  // tạm thời mình để cứng 10s, bạn có thể bổ sung sau.
  useEffect(() => {
    setCurrentCycleMs(cycleMs);
  }, []);

  const cycleOptions = [5000, 10000, 15000, 30000, 60000];
  const formatSeconds = (ms) => (ms ? Math.round(ms / 1000) : "-");

  /* ================== HANDLERS ================== */

  // Toggle LED1: FE đổi state local + gửi lệnh lên BE
  const handleToggleLed1 = () => {
    setLed1((prev) => {
      const next = !prev;
      socket.emit("config-led1", next); // BE publish lên feed `led`
      return next;
    });
  };

  // Khi kéo slider R/G/B
  const handleNeoSliderChange = (component) => (e) => {
    const value = Math.min(255, Math.max(0, Number(e.target.value) || 0));
    const newColor = { ...neoColor, [component]: value };
    setNeoColor(newColor);

    socket.emit("config-neo", {
      r: newColor.r,
      g: newColor.g,
      b: newColor.b,
      on: true,
    });
    setLed2(true);
  };

  // Tắt LED2
  const handleTurnOffNeo = () => {
    socket.emit("config-neo", { r: 0, g: 0, b: 0, on: false });
    setLed2(false);
  };

  // Lưu chu kỳ CoreIOT
  const handleSaveCycle = () => {
    const ms = Number(cycleMs);
    if (!Number.isFinite(ms) || ms < 5000) {
      setError("Chu kỳ phải ≥ 5.000 ms");
      return;
    }
    setError("");
    setCurrentCycleMs(ms);
    socket.emit("config-cycle", ms); // BE publish lên feed `chu-ky`
  };

  // Gửi cấu hình WiFi
  const handleConnectWifi = () => {
    if (!ssid.trim()) {
      setWifiMsg("Vui lòng nhập SSID.");
      return;
    }
    setWifiMsg("");
    setError("");

    socket.emit("config-wifi", { ssid, password }); // BE publish `wifi-id-password`
    setWifiMsg("Đã gửi cấu hình Wi-Fi lên thiết bị (qua Adafruit).");
  };

  // Ví dụ: bật gửi temp+humid+light (0/1) – bạn có thể gắn vào 1 toggle riêng
  const enableAllSensors = () => {
    socket.emit("config-sensor-enable", { temp: 1, humid: 1, light: 1 });
  };

  return (
    <div className="col-span-11 border rounded-md shadow-sm bg-[#FAFAFA] px-5 py-8">
      <div className="flex items-center justify-between mb-6">
        <h2 className="text-lg font-semibold">Device configuration</h2>
        <span className="text-xs text-gray-500">
          Backend: <code>Socket.IO</code>
        </span>
      </div>

      {error && (
        <div className="mb-4 rounded-md bg-red-50 border border-red-200 px-3 py-2 text-sm text-red-700">
          {error}
        </div>
      )}

      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {/* ===== Card 1: LED & NeoPixel ===== */}
        <div className="bg-white rounded-xl shadow p-4 flex flex-col gap-4">
          <div className="flex items-center gap-2">
            <div className="w-9 h-9 rounded-xl flex items-center justify-center bg-[#F3F4FF]">
              <FontAwesomeIcon
                icon={faLightbulb}
                className={led1 || led2 ? "text-yellow-400" : "text-gray-400"}
              />
            </div>
            <div className="font-semibold">LED & NeoPixel</div>
          </div>

          {/* LED1 */}
          <div className="flex items-center justify-between">
            <div className="text-sm text-gray-600">
              <div className="font-medium">LED 1</div>
              <div className="text-xs text-gray-400">
                Trạng thái:{" "}
                <span className="font-semibold">{led1 ? "ON" : "OFF"}</span>
              </div>
            </div>
            <button
              onClick={handleToggleLed1}
              className={`text-xs px-3 py-1 rounded-full font-semibold ${
                led1
                  ? "bg-emerald-100 text-emerald-700"
                  : "bg-gray-100 text-gray-700"
              }`}
            >
              Toggle
            </button>
          </div>

          <hr className="my-2" />

          {/* LED2 (NeoPixel) */}
          <div className="flex items-center justify-between mb-1">
            <div className="text-sm text-gray-600">
              <div className="font-medium">LED 2 (NeoPixel)</div>
              <div className="text-xs text-gray-400">
                Trạng thái:{" "}
                <span className="font-semibold">{led2 ? "ON" : "OFF"}</span>
              </div>
            </div>
            <div
              className="w-10 h-5 rounded-full border"
              style={{
                backgroundColor: `rgb(${neoColor.r},${neoColor.g},${neoColor.b})`,
              }}
            />
          </div>

          <div className="space-y-2 text-xs">
            <div className="flex items-center gap-2">
              <span className="w-6 text-red-500">R</span>
              <input
                type="range"
                min="0"
                max="255"
                value={neoColor.r}
                onChange={handleNeoSliderChange("r")}
                className="flex-1"
              />
              <span className="w-8 text-right">{neoColor.r}</span>
            </div>
            <div className="flex items-center gap-2">
              <span className="w-6 text-green-500">G</span>
              <input
                type="range"
                min="0"
                max="255"
                value={neoColor.g}
                onChange={handleNeoSliderChange("g")}
                className="flex-1"
              />
              <span className="w-8 text-right">{neoColor.g}</span>
            </div>
            <div className="flex items-center gap-2">
              <span className="w-6 text-blue-500">B</span>
              <input
                type="range"
                min="0"
                max="255"
                value={neoColor.b}
                onChange={handleNeoSliderChange("b")}
                className="flex-1"
              />
              <span className="w-8 text-right">{neoColor.b}</span>
            </div>
          </div>

          <button
            onClick={handleTurnOffNeo}
            className="mt-2 text-xs px-3 py-1 rounded-full bg-gray-100 text-gray-700 self-end"
          >
            Tắt LED 2
          </button>
        </div>

        {/* ===== Card 2: Chu kỳ CoreIOT ===== */}
        <div className="bg-white rounded-xl shadow p-4 flex flex-col gap-4">
          <div className="flex items-center gap-2">
            <div className="w-9 h-9 rounded-xl flex items-center justify-center bg-[#F3F4FF]">
              <FontAwesomeIcon icon={faSliders} className="text-indigo-600" />
            </div>
            <div className="font-semibold">Chu kỳ gửi CoreIOT</div>
          </div>

          <div className="text-sm text-gray-600">
            Đang gửi mỗi:{" "}
            <span className="font-semibold">
              {formatSeconds(currentCycleMs)} giây
            </span>
          </div>

          <label className="text-xs text-gray-500">Chọn chu kỳ mới</label>
          <select
            value={cycleMs}
            onChange={(e) => setCycleMs(Number(e.target.value))}
            className="w-full border rounded-lg px-3 py-2 text-sm focus:outline-none focus:ring-2 focus:ring-indigo-500"
          >
            {cycleOptions.map((ms) => (
              <option key={ms} value={ms}>
                {ms / 1000} giây
              </option>
            ))}
          </select>

          <button
            onClick={handleSaveCycle}
            className="mt-2 text-sm px-4 py-2 rounded-lg bg-indigo-600 text-white hover:bg-indigo-700"
          >
            Cập nhật chu kỳ
          </button>

          <button
            onClick={enableAllSensors}
            className="mt-1 text-xs px-3 py-1 rounded-full bg-gray-100 text-gray-700 self-start"
          >
            Bật gửi tất cả sensor
          </button>
        </div>

        {/* ===== Card 3: WiFi ===== */}
        <div className="bg-white rounded-xl shadow p-4 flex flex-col gap-3">
          <div className="flex items-center gap-2 mb-1">
            <div className="w-9 h-9 rounded-xl flex items-center justify-center bg-[#F3F4FF]">
              <FontAwesomeIcon icon={faWifi} className="text-emerald-500" />
            </div>
            <div className="font-semibold">Cấu hình Wi-Fi</div>
          </div>

          <div className="flex flex-col gap-2 text-sm">
            <input
              type="text"
              placeholder="SSID"
              value={ssid}
              onChange={(e) => setSsid(e.target.value)}
              className="border rounded-lg px-3 py-2 focus:outline-none focus:ring-2 focus:ring-emerald-500"
            />
            <input
              type="password"
              placeholder="Mật khẩu (có thể để trống)"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              className="border rounded-lg px-3 py-2 focus:outline-none focus:ring-2 focus:ring-emerald-500"
            />
          </div>

          <button
            onClick={handleConnectWifi}
            className="mt-1 text-sm px-4 py-2 rounded-lg bg-emerald-500 text-white hover:bg-emerald-600"
          >
            Gửi cấu hình Wi-Fi
          </button>

          {wifiMsg && (
            <div className="mt-1 text-xs text-emerald-700 bg-emerald-50 border border-emerald-100 rounded-md px-2 py-1">
              {wifiMsg}
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

export default Config;
