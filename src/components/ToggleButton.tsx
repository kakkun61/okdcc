import React from "react";

import { buttonStyle, toggleButtonStyle } from "./lib";

export interface ToggleButtonProps {
  state?: "default" | "disabled" | "selected";
  action?: "hover" | "active" | "focus" | null;
  style?: string;
  children: React.ReactNode;
  onClick?: () => void;
  onChangeState?: (state: "default" | "selected") => void;
}

export const ToggleButton = ({
  state = "default",
  action,
  style = "",
  children,
  onClick,
  onChangeState,
}: ToggleButtonProps) => {
  const [currentState, setCurrentState] = React.useState<
    "default" | "selected"
  >(state === "selected" ? state : "default");
  return (
    <button
      type="button"
      className={`toggle ${currentState === "default" ? "" : currentState} ${action ?? ""}`}
      disabled={state === "disabled"}
      onClick={() => {
        const newState = currentState === "default" ? "selected" : "default";
        setCurrentState(newState);
        onChangeState?.(newState);
        onClick?.();
      }}
    >
      {children}
      <style jsx>{buttonStyle}</style>
      <style jsx>{toggleButtonStyle}</style>
      <style jsx>{`button { ${style} }`}</style>
    </button>
  );
};
