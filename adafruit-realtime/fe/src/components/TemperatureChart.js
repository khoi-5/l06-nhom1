// src/components/TemperatureChart.jsx
import React, { useEffect, useState } from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
} from "recharts";
import { getSocket } from "../socket"; // chỉnh path nếu khác

const MAX_POINTS = 60; // số điểm tối đa trên chart

const TemperatureChart = () => {
  const [data, setData] = useState([]);
  const socket = getSocket();

  useEffect(() => {
    const handler = ({ at, feed, value }) => {
      // chỉ nhận temperature
      if (feed !== "temperature") return;
      if (typeof value !== "number" || !Number.isFinite(value)) return;

      const time = new Date(at).toLocaleTimeString();

      setData((prev) => {
        // nếu trùng time với điểm cuối thì cập nhật giá trị
        if (prev.length && prev[prev.length - 1].time === time) {
          const next = [...prev];
          next[next.length - 1] = { ...next[next.length - 1], temp: value };
          return next;
        }

        // thêm điểm mới, cắt bớt cho max 60
        const next = [...prev, { time, temp: value }];
        if (next.length > MAX_POINTS) next.shift();
        return next;
      });
    };

    socket.on("telemetry", handler);
    return () => socket.off("telemetry", handler);
  }, [socket]);

  return (
    <div className="w-full h-64 bg-white p-4 rounded shadow">
      <h2 className="text-lg font-semibold mb-4">Temperature Chart</h2>
      <ResponsiveContainer width="100%" height="100%">
        <LineChart data={data}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="time" />
          <YAxis unit="°C" />
          <Tooltip />
          <Line
            type="monotone"
            dataKey="temp"
            stroke="#8884d8"
            strokeWidth={2}
            dot={false}
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};

export default TemperatureChart;
