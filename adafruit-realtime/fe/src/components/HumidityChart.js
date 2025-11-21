// src/components/HumidityChart.jsx
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

const HumidityChart = () => {
  const [data, setData] = useState([]);
  const socket = getSocket();

  useEffect(() => {
    // handler nhận realtime từ BE
    const handler = ({ at, feed, value }) => {
      if (feed !== "humidity") return;
      if (typeof value !== "number" || !Number.isFinite(value)) return;

      const time = new Date(at).toLocaleTimeString();

      setData((prev) => {
        // nếu cùng time với điểm cuối thì update luôn
        if (prev.length && prev[prev.length - 1].time === time) {
          const next = [...prev];
          next[next.length - 1] = { ...next[next.length - 1], humidity: value };
          return next;
        }

        // thêm điểm mới, giới hạn MAX_POINTS
        const next = [...prev, { time, humidity: value }];
        if (next.length > MAX_POINTS) next.shift();
        return next;
      });
    };

    socket.on("telemetry", handler);
    return () => socket.off("telemetry", handler);
  }, [socket]);

  return (
    <div className="w-full h-64 bg-white p-4 rounded shadow mt-6">
      <h2 className="text-lg font-semibold mb-4">Humidity Chart</h2>
      <ResponsiveContainer width="100%" height="100%">
        <LineChart data={data}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="time" />
          <YAxis unit="%" />
          <Tooltip />
          <Line
            type="monotone"
            dataKey="humidity"
            stroke="#00b894"
            strokeWidth={2}
            dot={false}
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};

export default HumidityChart;
