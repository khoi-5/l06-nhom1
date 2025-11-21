// src/components/LightChart.jsx
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
import { getSocket } from "../socket"; // chỉnh lại path nếu khác

const MAX_POINTS = 60;

const LightChart = () => {
  const [data, setData] = useState([]);
  const socket = getSocket();

  useEffect(() => {
    const handler = ({ at, feed, value }) => {
      if (feed !== "light") return;
      if (typeof value !== "number" || !Number.isFinite(value)) return;

      const time = new Date(at).toLocaleTimeString();

      setData((prev) => {
        if (prev.length && prev[prev.length - 1].time === time) {
          const next = [...prev];
          next[next.length - 1] = { ...next[next.length - 1], lux: value };
          return next;
        }

        const next = [...prev, { time, lux: value }];
        if (next.length > MAX_POINTS) next.shift();
        return next;
      });
    };

    socket.on("telemetry", handler);
    return () => socket.off("telemetry", handler);
  }, [socket]);

  return (
    <div className="w-full h-64 bg-white p-4 rounded shadow">
      <h2 className="text-lg font-semibold mb-4">Light Chart</h2>
      <ResponsiveContainer width="100%" height="100%">
        <LineChart data={data}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="time" />
          <YAxis unit=" lux" />
          <Tooltip />
          <Line
            type="monotone"
            dataKey="lux"
            stroke="#f59e0b"
            strokeWidth={2}
            dot={false}
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};

export default LightChart;
