"use client";

import { FunctionButton, FrontLightFunctionButton } from "./FunctionButton";
import * as Dcc from "../lib/dcc";
import "./variables.css";

export interface ThrottleProps {
  maxFunctionNumber?: Dcc.FunctionNumber;
}

export const Throttle = ({
  maxFunctionNumber = Dcc.maxFunctionNumber,
}: ThrottleProps) => {
  return (
    <>
      <div>Address</div>
      <div>Speed</div>
      <div>Direction</div>
      <div className="function-buttons">
        <FrontLightFunctionButton
          className="fl"
          key={Dcc.frontLightFunctionNumber}
        />
        {[...Array(maxFunctionNumber)]
          .map((_, i) => i + 1)
          .map((i) => (
            <FunctionButton number={Dcc.createFunctionNumber(i)} key={i} />
          ))}
        <style jsx>{`
          .function-buttons {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            grid-template-rows: auto;
            row-gap: var(--gap-medium);
            column-gap: var(--gap-medium);
          }
          .function-buttons :global(button) {
            font-size: var(--font-size-xlarge);
          }
          .function-buttons :global(.fl) {
            grid-column: 1 / span 3;
          }
        `}</style>
      </div>
    </>
  );
};
