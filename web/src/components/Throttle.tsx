"use client";

import Image from "next/image";
import "material-symbols/rounded.css";

import "./material-symbols.css";
import "./variables.css";
import { Button } from "./Button";
import { Button as ButtonGroupButton } from "./ButtonGroup/Button";
import { ButtonGroup } from "./ButtonGroup";
import { SpeedMeter } from "./SpeedMeter";
import * as Logic from "@/logic";
import { RadioButtonGroup } from "./RadioButtonGroup";
import { Button as RadioButtonGroupButton } from "./RadioButtonGroup/Button";
import { FunctionInfo, mmToPx } from "./lib";
import { ToggleButton } from "./ToggleButton";

export interface ThrottleProps {
  trainName: Logic.TrainName;
  functionInfos: FunctionInfo[];
  ref?: React.Ref<HTMLDivElement>;
  style?: string;
  onClickSpeedUp?: (
    event: React.MouseEvent<HTMLButtonElement, MouseEvent>,
  ) => void;
  onClickSpeedDown?: (
    event: React.MouseEvent<HTMLButtonElement, MouseEvent>,
  ) => void;
  onClickForward?: (
    event: React.MouseEvent<HTMLButtonElement, MouseEvent>,
  ) => void;
  onClickBackward?: (
    event: React.MouseEvent<HTMLButtonElement, MouseEvent>,
  ) => void;
  onClickEmergencyStop?: (
    event: React.MouseEvent<HTMLButtonElement, MouseEvent>,
  ) => void;
  onClickFunctionButton?: (
    event: React.MouseEvent<HTMLButtonElement, MouseEvent>,
    functionInfo: FunctionInfo,
  ) => void;
}

export const Throttle = ({
  functionInfos,
  trainName,
  ref,
  style,
  onClickSpeedUp,
  onClickSpeedDown,
  onClickForward,
  onClickBackward,
  onClickEmergencyStop,
  onClickFunctionButton,
}: ThrottleProps) => {
  return (
    <div className="throttle" ref={ref}>
      <div className="meters">
        <SpeedMeter
          speed={0}
          style={`
            grid-row: 1 / 4;
            height: var(--meters-height);
          `}
        />
        <div className="train-name">{trainName}</div>
        <div className="speed-and-direction">
          <RadioButtonGroup selected="forward" style="flex: 1 1 auto;">
            <RadioButtonGroupButton
              key="forward"
              id="forward"
              onClick={onClickForward}
            >
              <span className="material-symbols-rounded">arrow_upward</span>
            </RadioButtonGroupButton>
            <RadioButtonGroupButton
              key="backward"
              id="backward"
              onClick={onClickBackward}
            >
              <span className="material-symbols-rounded">arrow_downward</span>
            </RadioButtonGroupButton>
          </RadioButtonGroup>
          <ButtonGroup style="flex: 1 1 auto;">
            <ButtonGroupButton key="speed-up" onClick={onClickSpeedUp}>
              <span className="material-symbols-rounded">add</span>
            </ButtonGroupButton>
            <ButtonGroupButton key="speed-down" onClick={onClickSpeedDown}>
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
        {functionInfos.map((info) => {
          const imageDiv = (() => {
            const functionButtonImageSize = 15;
            return (
              <div>
                {info.image && (
                  <Image
                    src={info.image}
                    alt="" // 直下にラベルがあるので要らない
                    width={mmToPx(functionButtonImageSize)}
                    height={mmToPx(functionButtonImageSize)}
                    priority={false}
                  />
                )}
              </div>
            );
          })();
          const style = "height: 30mm; flex-direction: column;";
          if (info.stateType === "toggle") {
            return (
              <ToggleButton key={info.number} style={style}>
                {imageDiv}
                <div className="function-button-label">{info.label}</div>
              </ToggleButton>
            );
          }
          if (info.stateType === "momentary") {
            return (
              <Button
                key={info.number}
                onClick={(event) => onClickFunctionButton?.(event, info)}
                style={style}
              >
                {imageDiv}
                <div className="function-button-label">{info.label}</div>
              </Button>
            );
          }
        })}
      </div>
      <style jsx>{`
        .throttle {
          display: flex;
          flex-direction: column;
          max-width: var(--throttle-max-width);
          min-width: min(100vw, var(--throttle-min-width));
          height: 100%;
          overflow: hidden;
        }

        .meters {
          --meters-height: 40mm;
          --first-column-width: var(--meters-height);
          flex: 0 0 auto;
          display: grid;
          grid-template-columns: var(--first-column-width) 1fr;
          grid-template-rows: 1fr auto auto;
          align-items: center;
        }

        .train-name {
          padding: var(--gap-medium);
          font-size: var(--font-size-xlarge);
          font-weight: var(--font-weight-bold);
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
          flex: 0 1 auto;
          margin-top: var(--gap-medium);
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
          row-gap: var(--gap-medium);
          column-gap: var(--gap-medium);
          overflow-y: auto;
          overscroll-behavior-y: none;
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

        .function-button-label {
          flex: 0 0 auto;
          overflow: hidden;
          text-overflow: ellipsis;
        }
      `}</style>
      <style jsx>{`.throttle { ${style} }`}</style>
    </div>
  );
};
