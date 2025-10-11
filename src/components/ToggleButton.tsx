import React from "react";

import { buttonStyle } from "./lib";

export interface ToggleButtonProps {
  state?: "default" | "hover" | "active" | "disabled";
  style?: string;
  children: React.ReactNode;
  onClick?: () => void;
  onChangeState?: (state: "default" | "active") => void;
}

export const ToggleButton = ({
  state = "default",
  style = "",
  children,
  onClick,
  onChangeState,
}: ToggleButtonProps) => {
  const [userState, setUserState] = React.useState<"default" | "active">(
    state === "active" ? "active" : "default",
  );
  return (
    <button
      type="button"
      className={`${state === "default" ? "" : state} ${userState === "default" ? "" : userState}`}
      disabled={state === "disabled"}
      onClick={() => {
        const newState = userState === "default" ? "active" : "default";
        setUserState(newState);
        onChangeState?.(newState);
        onClick?.();
      }}
    >
      {children}
      <style jsx>{buttonStyle}</style>
      <style jsx>{`button { ${style} }`}</style>
    </button>
  );
};
