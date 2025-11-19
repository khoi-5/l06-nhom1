// src/main.jsx
import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App.jsx";

ReactDOM.createRoot(document.getElementById("root")).render(
  // BỎ StrictMode để tránh mount/unmount 2 lần trong dev
  // <React.StrictMode>
    <App />
  // </React.StrictMode>
);
