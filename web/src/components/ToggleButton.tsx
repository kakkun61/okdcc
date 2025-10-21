import React from "react";

export interface ToggleButtonProps {
  state?: "default" | "disabled" | "selected";
  action?: "hover" | "active" | "focus" | null;
  style?: string;
  children: React.ReactNode;
  onClick?: (event: React.MouseEvent<HTMLButtonElement, MouseEvent>) => void;
  onToggle?: (
    event: React.MouseEvent<HTMLButtonElement, MouseEvent>,
    state: "default" | "selected",
  ) => void;
}

export const ToggleButton = ({
  state: initialState = "default",
  action,
  style = "",
  children,
  onClick,
  onToggle: onToggle,
}: ToggleButtonProps) => {
  const [state, setState] = React.useState<"default" | "selected">(
    initialState === "selected" ? initialState : "default",
  );
  return (
    <button
      type="button"
      className={`button toggle-button ${state === "default" ? "" : state} ${action ?? ""}`}
      disabled={initialState === "disabled"}
      onClick={(event) => {
        const newState = state === "default" ? "selected" : "default";
        setState(newState);
        onToggle?.(event, newState);
        onClick?.(event);
      }}
    >
      {children}
      <style jsx>{`button { ${style} }`}</style>
    </button>
  );
};
