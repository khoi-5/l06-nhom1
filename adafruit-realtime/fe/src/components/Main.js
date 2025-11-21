import { useState, useEffect } from "react";
import { faUser, faCloud, faSun } from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import Humidity from "./Humidity";
import Temperature from "./Temperature";
import Light from "./Light";
import TemperatureChart from "./TemperatureChart";
import HumidityChart from "./HumidityChart";
import LightChart from "./LightChart";
function Main() {
  const [now, setNow] = useState(new Date());
  // mot fetch du lieu
  const users = {
    avt: faUser,
    name: "L06-NHOM1",
  };

  useEffect(() => {
    const interval = setInterval(() => setNow(new Date()), 1000);
    return () => clearInterval(interval);
  }, []);
  const month = now.toLocaleString("en-US", { month: "short" }); // Sep
  const date = now.getDate(); // 13
  const fullTime = now.toLocaleTimeString("en-US", {
    hour: "2-digit",
    minute: "2-digit",
    hour12: true,
  }); // ví dụ: "11:45 PM"

  const [time, ampm] = fullTime.split(" "); // time = "11:45", ampm = "PM"

  return (
    <div className="col-span-11  border rounded-xl shadow-sm bg-[#FAFAFA] px-5 py-10 mt-2">
      <div className="grid grid-cols-2 bg-white rounded-md shadow-md p-4 mb-8">
        <div className="flex flex-col gap-10 ">
          <div>
            <div className="flex items-center space-x-1 text-sm ">
              <span>{month}</span>
              <div className="border border-[#0C0824] rounded-md px-1">
                {date}
              </div>
            </div>

            {/* Time */}
            <div className="flex items-baseline space-x-1 mt-1">
              <span className="text-4xl font-semibold">{time}</span>
              <span className="text-sm ">{ampm}</span>
            </div>
          </div>
          <div className="flex  items-center gap-4">
            <div>
              <FontAwesomeIcon icon={users.avt} className="text-5xl" />
            </div>
            <div>
              <div className="text-2xl font-semibold">Hi {users.name} !</div>
              <div className="text-[#C4C4C4]">Control your house from here</div>
            </div>
          </div>
        </div>

        {/* col-2 */}
        <div className="flex flex-col justify-center items-end  pr-10 relative gap-4">
          <div>
            <FontAwesomeIcon
              icon={faSun}
              className=" text-6xl text-[#FFE07D]"
            />
          </div>
          <div>
            <FontAwesomeIcon
              icon={faCloud}
              className=" text-6xl text-[#ECF4FF] absolute bottom-14 right-14  "
            />
          </div>
          <span className="font-semibold">Ho Chi Minh</span>
        </div>
      </div>
      <div className="flex flex-col gap-5">
        <div>Frequently Used</div>
        <div className="grid grid-cols-3 gap-5">
          <Temperature/>
          <Humidity />
          <Light />
        </div>
      </div>
      <div className="mt-10">
        <TemperatureChart />
      </div>
      <div className="mt-10">
        <HumidityChart/>
      </div>
      <div className="mt-10">
        <LightChart/>
      </div>
      
    </div>
  );
}

export default Main;
