// src/components/Temperature.jsx
import React, { useState, useEffect } from "react";
import { getSocket } from "../socket";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faTemperatureHalf } from "@fortawesome/free-solid-svg-icons";

const Temperature = () => {
  const [temp, setTemp] = useState(null);
  const [isOn, setIsOn] = useState(true); // bật mặc định giống Light/Humidity
  const socket = getSocket();

  // Nhận dữ liệu nhiệt độ từ BE
  useEffect(() => {
    const handler = ({ feed, value }) => {
      if (
        feed === "temperature" &&
        typeof value === "number" &&
        Number.isFinite(value)
      ) {
        setTemp(value);
      }
    };

    socket.on("telemetry", handler);
    return () => socket.off("telemetry", handler);
  }, [socket]);

  // Gạt công tắc -> gửi lệnh control temperature (set-temp)
  const handleToggle = () => {
    setIsOn((prev) => {
      const next = !prev;
      // gửi lên BE, BE publish ra feed temperature-control
      socket.emit("set-temp", next ? "ON" : "OFF");
      return next;
    });
  };

  const showValue = temp !== null ? temp.toFixed(1) : "--";

  return (
    <div className="col-span-1 bg-white rounded-xl shadow p-4 flex flex-col">
      {/* Hàng trên: icon + switch giống Light */}
      <div className="w-full flex items-center justify-between mb-4">
        {/* Icon nhiệt kế */}
        <div className="bg-[#F7F6FF] p-4 rounded-2xl">
          <FontAwesomeIcon
            icon={faTemperatureHalf}
            className={isOn ? "text-red-500" : "text-gray-300"}
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
        Temperature Sensor
      </div>

      {/* Giá trị: 25.3 °C */}
      <div className="flex justify-center items-baseline gap-2">
        <span className="text-3xl font-bold text-slate-900">
          {showValue}
        </span>
        <span className="text-base font-semibold text-gray-500">°C</span>
      </div>
    </div>
  );
};

export default Temperature;
