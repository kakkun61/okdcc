"use client";

import "./material-symbols.css";
import "./variables.css";
import "material-symbols/rounded.css";
import { Button } from "./Button";
import { Button as ButtonGroupButton } from "./ButtonGroup/Button"
import { ButtonGroup } from "./ButtonGroup";
import { FunctionButton } from "./Throttle/FunctionButton";
import { SpeedMeter } from "./Throttle/SpeedMeter";
import { Train } from "./Throttle/Train";
import * as Dcc from "@/lib/dcc";

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
    <div className="throttle">
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
          <ButtonGroup style="flex: 1 1 auto;">
            <ButtonGroupButton onClick={() => {}} style="padding: 0;">
              <span className="material-symbols-rounded">
                arrow_upward
              </span>
            </ButtonGroupButton>
            <ButtonGroupButton onClick={() => {}}>
              <span className="material-symbols-rounded">
                arrow_downward
              </span>
            </ButtonGroupButton>
          </ButtonGroup>
          <ButtonGroup style="flex: 1 1 auto;">
            <ButtonGroupButton onClick={() => {}}>
              <span className="material-symbols-rounded">add</span>
            </ButtonGroupButton>
            <ButtonGroupButton onClick={() => {}}>
              <span className="material-symbols-rounded">remove</span>
            </ButtonGroupButton>
          </ButtonGroup>
        </div>
        <div className="emergency-stop">
          <Button style="
            display: flex;
            gap: var(--gap-medium);
            flex-wrap: wrap;
            width: 100%;
            overflow: hidden;
            text-overflow: ellipsis;
            align-items: center;
            justify-content: center;
          ">
            <span className="material-symbols-rounded">emergency</span>
            <span>Emergency Stop</span>
          </Button>
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
        .throttle {
          max-width: 150mm;
        }

        .meters {
          --meters-height: 40mm;
          --first-column-width: var(--meters-height);
          display: grid;
          grid-template-columns: var(--first-column-width) 1fr;
          grid-template-rows: 1fr auto auto;
          align-items: center;
        }

        .speed-and-direction {
          grid-column: 2;
          margin-top: var(--gap-medium);
          display: flex;
          gap: var(--gap-medium);
          flex-wrap: wrap;
        }

        .emergency-stop {
          grid-column: 2;
          margin-top: var(--gap-medium);
          min-width: 0;
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
