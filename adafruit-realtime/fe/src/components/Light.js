import { useEffect, useState } from "react";
import { getSocket } from "../socket";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faSun } from "@fortawesome/free-solid-svg-icons";

function Light() {
  const [isOn, setIsOn] = useState(false);
  const [lux, setLux] = useState(null);

  const socket = getSocket();

  useEffect(() => {
    const handler = ({ feed, value }) => {
      if (feed === "light" && typeof value === "number" && Number.isFinite(value)) {
        setLux(value);
      }
    };

    socket.on("telemetry", handler);
    return () => socket.off("telemetry", handler);
  }, [socket]);

  const handleToggle = () => {
    setIsOn((prev) => {
      const next = !prev;
      socket.emit("set-light", next ? "ON" : "OFF");
      return next;
    });
  };

  const showValue = lux !== null ? lux.toFixed(0) : "--";

  return (
    <div className="col-span-1 bg-white rounded-xl shadow p-4 flex flex-col">
      {/* HÀNG TRÊN: icon trái, switch phải – canh đều theo chiều ngang */}
      <div className="w-full flex items-center justify-between mb-4">
        {/* Icon mặt trời */}
        <div className="bg-[#F7F6FF] p-3 rounded-2xl">
          <FontAwesomeIcon
            icon={faSun}
            className={isOn ? "text-yellow-400" : "text-gray-300"}
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
        Light Sensor
      </div>

      {/* Dòng LIGHT : 101 LUX */}
      <div className="flex justify-center items-baseline gap-2">
        <span className="text-3xl font-bold text-slate-900">
          {showValue}
        </span>
        <span className="text-sm font-semibold text-gray-500 uppercase">
          lux
        </span>
      </div>
    </div>
  );
}

export default Light;
