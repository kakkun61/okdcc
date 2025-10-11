import React from "react";
import "./variables.css";

import { Button } from "./RadioButtonGroup/Button";
import { updateComponentPropsRecursively } from "./lib";

export interface RadioButtonGroupProps {
  state?: "default" | "disabled";
  style?: string;
  initialActivated?: string;
  children: React.ReactNode;
  onActivate?: (id: string) => void;
}

export const RadioButtonGroup = ({
  state = "default",
  style = "",
  initialActivated,
  children,
  onActivate,
}: RadioButtonGroupProps) => {
  const [selected, setSelected] = React.useState<string | null>(
    initialActivated ?? null,
  );
  const children_ = updateComponentPropsRecursively(
    Button,
    children,
    (props) => ({
      state: state === "disabled" ? "disabled" : props.state,
      active: props.id === selected,
      onClick: () => {
        props.onClick?.();
        setSelected(props.id);
        onActivate?.(props.id);
      },
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
