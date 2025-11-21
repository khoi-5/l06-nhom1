// Helper dùng chung cho FE
import { io } from "socket.io-client";

const SOCKET_URL = "http://localhost:3001";

let socket;

export function getSocket() {
  if (!socket) {
    socket = io(SOCKET_URL, {
      transports: ["websocket"],
    });
  }
  return socket;
}
