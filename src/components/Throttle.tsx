"use client";

import { FunctionButton } from "./Throttle/FunctionButton";
import * as Dcc from "@/lib/dcc";
import "./variables.css";
import { Train } from "./Throttle/Train";
import { SpeedMeter } from "./Throttle/SpeedMeter";
import { ButtonGroup } from "./ButtonGroup";
import { Button } from "./Button";

export interface ThrottleProps {
  trainName: Dcc.TrainName;
  functionInfos: Dcc.FunctionInfo[];
  onClickSpeedUp?: () => void;
  onClickSpeedDown?: () => void;
  onClickForward?: () => void;
  onClickBackward?: () => void;
  onClickEmergencyStop?: () => void;
}

export const Throttle = ({ functionInfos, trainName }: ThrottleProps) => {
  return (
    <div>
      <div className="meters">
        <SpeedMeter
          speed={0}
          style={`
          grid-row: 1 / 4;
          height: var(--meters-height);
        `}
        />
        <Train name={trainName} />
        <div className="speed-and-direction">
          <ButtonGroup buttonProps={[{ label: "↑" }, { label: "↓" }]} style="width: calc(50% - var(--gap-medium) / 2);" />
          <span style={{ display: "inline-block", width: "var(--gap-medium)" }}></span>
          <ButtonGroup buttonProps={[{ label: "+" }, { label: "-" }]} style="width: calc(50% - var(--gap-medium) / 2);" />
        </div>
        <div className="emergency-stop">
          <Button label="Emergency Stop" style="width: 100%;" />
        </div>
      </div>
      <div className="function-buttons">
        {functionInfos.map((i) => (
          <FunctionButton
            info={i}
            key={i.number}
            onClick={(_) => {}}
            style={`
              font-size: var(--font-size-xlarge);
            `}
          />
        ))}
      </div>
      <style jsx>{`
        .meters {
          --meters-height: 6cm;
          --second-column-width: 8cm;
          display: grid;
          height: var(--meters-height);
          grid-template-columns: var(--meters-height) var(--second-column-width) 1fr;
          grid-template-rows: 1fr auto auto;
        }

        .speed-and-direction {
          grid-column: 2;
          padding: var(--gap-medium);
        }

        .emergency-stop {
          grid-column: 2;
          padding: var(--gap-medium);
        }

        .function-buttons {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
          grid-template-rows: auto;
          row-gap: var(--gap-medium);
          column-gap: var(--gap-medium);
        }
      `}</style>
    </div>
  );
};
