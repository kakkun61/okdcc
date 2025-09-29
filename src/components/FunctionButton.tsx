"use client";

import * as Dcc from "../lib/dcc";

export interface FunctionButtonProps {
  number: Dcc.FunctionNumber;
  label?: string;
  onClick?: () => void;
  className?: string;
}

export const FunctionButton = ({
  number,
  label,
  onClick,
  ...props
}: FunctionButtonProps) => {
  return (
    <button onClick={onClick} {...props}>
      {label ?? number}
      <style jsx>{`
        button {
          text-align: center;
        }
      `}</style>
    </button>
  );
};

export const FrontLightFunctionButton = (props: {
  onClick?: () => void;
  className?: string;
}) => {
  return (
    <FunctionButton
      number={Dcc.frontLightFunctionNumber}
      label="FL"
      {...props}
    />
  );
};
