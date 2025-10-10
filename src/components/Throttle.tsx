"use client";

import "./material-symbols.css";
import "./variables.css";
import "material-symbols/rounded.css";
import { Button } from "./Button";
import { Button as ButtonGroupButton } from "./ButtonGroup/Button";
import { ButtonGroup } from "./ButtonGroup";
import { SpeedMeter } from "./SpeedMeter";
import { Train } from "./Throttle/Train";
import * as Dcc from "@/lib/dcc";
import Image from "next/image";

export interface ThrottleProps {
  trainName: Dcc.TrainName;
  functionInfos: Dcc.FunctionInfo[];
  onClickSpeedUp?: () => void;
  onClickSpeedDown?: () => void;
  onClickForward?: () => void;
  onClickBackward?: () => void;
  onClickEmergencyStop?: () => void;
  onClickFunctionButton?: (functionInfo: Dcc.FunctionInfo) => void;
}

export const Throttle = ({
  functionInfos,
  trainName,
  onClickSpeedUp,
  onClickSpeedDown,
  onClickForward,
  onClickBackward,
  onClickEmergencyStop,
  onClickFunctionButton,
}: ThrottleProps) => {
  const functionButtonImageSize = 50;
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
            <ButtonGroupButton onClick={onClickForward}>
              <span className="material-symbols-rounded">arrow_upward</span>
            </ButtonGroupButton>
            <ButtonGroupButton onClick={onClickBackward}>
              <span className="material-symbols-rounded">arrow_downward</span>
            </ButtonGroupButton>
          </ButtonGroup>
          <ButtonGroup style="flex: 1 1 auto;">
            <ButtonGroupButton onClick={onClickSpeedUp}>
              <span className="material-symbols-rounded">add</span>
            </ButtonGroupButton>
            <ButtonGroupButton onClick={onClickSpeedDown}>
              <span className="material-symbols-rounded">remove</span>
            </ButtonGroupButton>
          </ButtonGroup>
        </div>
        <div className="emergency-stop">
          <Button
            onClick={onClickEmergencyStop}
            style="
            display: flex;
            gap: var(--gap-medium);
            flex-wrap: wrap;
            width: 100%;
            overflow: hidden;
            text-overflow: ellipsis;
            align-items: center;
            justify-content: center;
            --button-background-color: hsl(from red h 100% 95%);
            --button-background-color-hover: hsl(
              from var(--button-background-color) h s calc(l - 10)
            );
            --button-background-color-active: hsl(
              from var(--button-background-color) h s calc(l - 20)
            );
          "
          >
            <span className="material-symbols-rounded">emergency</span>
            <span>Emergency Stop</span>
          </Button>
        </div>
      </div>
      <div className="function-buttons">
        {functionInfos.map((info) => (
          <button
            role="button"
            className="function-button"
            onClick={() => onClickFunctionButton?.(info)}
          >
            <div className="function-button-image">
              {info.image && (
                <Image
                  src={info.image}
                  alt={info.label}
                  width={functionButtonImageSize}
                  height={functionButtonImageSize}
                />
              )}
            </div>
            <div className="function-button-label">{info.label}</div>
          </button>
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
          margin-top: var(--gap-medium);
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
          grid-template-rows: auto;
          row-gap: var(--gap-medium);
          column-gap: var(--gap-medium);
        }

        .function-button {
          display: flex;
          flex-direction: column;
          padding: var(--gap-medium);
          border-style: none;
          text-align: center;
          white-space: nowrap;
          cursor: pointer;
        }

        .function-button-image {
          height: ${functionButtonImageSize}px;
          flex: 0 0 ${functionButtonImageSize}px;
        }

        .function-button-label {
          margin-top: var(--gap-small);
          flex: 0 0 auto;
          overflow: hidden;
          text-overflow: ellipsis;
        }
      `}</style>
    </div>
  );
};
