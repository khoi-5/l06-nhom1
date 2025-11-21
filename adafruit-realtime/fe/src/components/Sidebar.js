import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faHouse,
  faUser,
  faDesktop,
  faN,
  faRightFromBracket,
} from "@fortawesome/free-solid-svg-icons";
import { useState } from "react";

function Sidebar({ show, setShow }) {
  const [activeIcon, setActiveIcon] = useState('home');
  const icons = [
    { id: "home", title: faHouse },
    { id: "config", title: faDesktop },
    //{ id: "user", title: faUser },
  ];

  return (
    <div className="col-span-1 flex flex-col justify-start gap-32 pt-4 ">
      <div className="flex justify-around items-center">
        <FontAwesomeIcon
          icon={faN}
          className="text-[#030391] shadow-md  rounded-md p-3 text-center"
        />
      </div>

      <div className="flex flex-col  items-center justify-around gap-10 mt-5 text-[#C4C4C4]">
        {icons.map((icon) => {
          return (
            <div key={icon.id}>
              <FontAwesomeIcon
                icon={icon.title}
                className={`
      cursor-pointer transition-all duration-300 ease-in-out 
      text-xl
      ${
        activeIcon === icon.id
          ? "bg-[#F7F6FF] text-[#030391] p-3 rounded-xl shadow-md "
          : " hover:text-[#030391]  "
      }
    `}
                onClick={() => {
                  setActiveIcon(icon.id);
                  setShow(icon.id);
                }}
              />
            </div>
          );
        })}
      </div>
      <div className="flex justify-around items-center text-[#C4C4C4] ">
        <FontAwesomeIcon
          icon={faRightFromBracket}
          className="hover:text-[#030391] "
        />
      </div>
    </div>
  );
}

export default Sidebar;
