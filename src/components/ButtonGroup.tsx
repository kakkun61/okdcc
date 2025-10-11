import React from "react";
import "./variables.css";

import { Button } from "./ButtonGroup/Button";
import { updateComponentPropsRecursively } from "./lib";

export interface ButtonGroupProps {
  state?: "default" | "disabled";
  style?: string;
  children: React.ReactNode;
}

export const ButtonGroup = ({
  state = "default",
  style = "",
  children,
}: ButtonGroupProps) => {
  const children_ = updateComponentPropsRecursively(
    Button,
    children,
    (props) => ({
      state: state === "disabled" ? "disabled" : props.state,
    }),
  );
  return (
    <span className="button-group">
      {children_}
      <style jsx>{`
        .button-group {
          display: flex;
          min-width: fit-content;
          white-space: nowrap;
          ${style}
        }
      `}</style>
    </span>
  );
};
