// src/components/Humidity.jsx
import React, { useState, useEffect } from "react";
import { getSocket } from "../socket";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faDroplet } from "@fortawesome/free-solid-svg-icons";

const Humidity = () => {
  const [humidity, setHumidity] = useState(null);
  const [isOn, setIsOn] = useState(true); // bật mặc định giống Light
  const socket = getSocket();

  useEffect(() => {
    const handler = ({ feed, value }) => {
      if (
        feed === "humidity" &&
        typeof value === "number" &&
        Number.isFinite(value)
      ) {
        setHumidity(value);
      }
    };

    socket.on("telemetry", handler);
    return () => socket.off("telemetry", handler);
  }, [socket]);

  // Gạt công tắc -> gửi lệnh control humidity (set-humid)
  const handleToggle = () => {
    setIsOn((prev) => {
      const next = !prev;
      socket.emit("set-humid", next ? "ON" : "OFF");
      return next;
    });
  };

  const showValue = humidity !== null ? humidity.toFixed(0) : "--";

  return (
    <div className="col-span-1 bg-white rounded-xl shadow p-4 flex flex-col">
      {/* Hàng trên: icon giọt nước + switch */}
      <div className="w-full flex items-center justify-between mb-4">
        <div className="bg-[#F7F6FF] p-4 rounded-2xl">
          <FontAwesomeIcon
            icon={faDroplet}
            // 🔥 ĐỔI MÀU THEO isOn, giống Light
            className={isOn ? "text-sky-500" : "text-gray-300"}
            size="2x"
          />
        </div>

        {/* Nút switch */}
        <button
          onClick={handleToggle}
          className={`relative w-11 h-6 rounded-full transition-colors duration-200
            ${isOn ? "bg-blue-900" : "bg-gray-300"}`}
        >
          <span
            className={`absolute top-[3px] left-[3px] w-4 h-4 rounded-full bg-white shadow
              transition-transform duration-200
              ${isOn ? "translate-x-5" : "translate-x-0"}`}
          />
        </button>
      </div>

      {/* Tiêu đề ở giữa */}
      <div className="font-semibold text-center text-[15px] mb-2">
        Humidity Sensor
      </div>

      {/* Giá trị: 68 % */}
      <div className="flex justify-center items-baseline gap-2">
        <span className="text-3xl font-bold text-slate-900">
          {showValue}
        </span>
        <span className="text-base font-semibold text-gray-500">%</span>
      </div>
    </div>
  );
};

export default Humidity;
