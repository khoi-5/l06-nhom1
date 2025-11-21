import Main from "./components/Main";
import Sidebar from "./components/Sidebar";
import { useState } from "react";
import Config from "./components/Config";
import "reactjs-popup/dist/index.css";
function App() {
  const [show, setShow] = useState("home");
  return (
    <div className="grid grid-cols-12 min-h-[800px] modal">
      <Sidebar show={show} setShow={setShow} />
      {show === "home" ? <Main /> : show === "config" ? <Config /> : <></>}
      {/* {show === "home" ? <Service /> : <></>} */}
    </div>
  );
}

export default App;
