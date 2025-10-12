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
  state: initialState = "default",
  action,
  style = "",
  children,
  onClick,
  onChangeState,
}: ToggleButtonProps) => {
  const [state, setState] = React.useState<
    "default" | "selected"
  >(initialState === "selected" ? initialState : "default");
  return (
    <button
      type="button"
      className={`toggle ${state === "default" ? "" : state} ${action ?? ""}`}
      disabled={initialState === "disabled"}
      onClick={() => {
        const newState = state === "default" ? "selected" : "default";
        setState(newState);
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
