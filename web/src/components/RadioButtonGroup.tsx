import React from "react";
import "./variables.css";

import { Button, ButtonProps } from "./RadioButtonGroup/Button";
import { updateComponentPropsRecursively } from "./lib";

export interface RadioButtonGroupProps {
  state?: "default" | "disabled";
  style?: string;
  selected?: string;
  children: React.ReactNode;
  onActivate?: (
    event: React.MouseEvent<HTMLButtonElement, MouseEvent>,
    id: string,
  ) => void;
}

export const RadioButtonGroup = ({
  state = "default",
  style = "",
  selected: initialSelected,
  children,
  onActivate,
}: RadioButtonGroupProps) => {
  const [selected, setSelected] = React.useState<string | null>(
    initialSelected ?? null,
  );
  const children_ = updateComponentPropsRecursively(
    Button,
    children,
    (props: ButtonProps): Partial<ButtonProps> => ({
      state:
        state === "disabled"
          ? "disabled"
          : props.id === selected
            ? "selected"
            : props.state,
      onClick:
        props.id === selected
          ? undefined
          : (event) => {
              props.onClick?.(event);
              setSelected(props.id);
              onActivate?.(event, props.id);
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
