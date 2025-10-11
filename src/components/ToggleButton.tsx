import React from "react";
import "./variables.css";

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
  const [userState, setUserState] = React.useState<"default" | "active">(state === "active" ? "active" : "default");
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
      <style jsx>{`
        button {
          display: inline-block;
          padding: var(--button-padding-vertical) var(--button-padding-horizontal);
          border-style: none;
          border-radius: var(--button-border-radius);
          font-size: var(--button-font-size);
          font-weight: var(--button-font-weight);
          text-align: center;
          line-height: var(--button-line-height);
          cursor: pointer;
          color: var(--button-color);
          background-color: var(--button-background-color);
          text-align: center;
          ${style}
        }

        /* :hover など疑似属性が優先されるようにそれらはクラス属性 .hover .active .disabled の後に書く */
        button.hover:not(:disabled) {
          background-color: var(--button-background-color-hover);
        }

        button.active:not(:disabled) {
          background-color: var(--button-background-color-active);
        }

        button:hover:not(:disabled) {
          background-color: var(--button-background-color-hover);
        }

        button:active:not(:disabled) {
          background-color: var(--button-background-color-active);
        }

        button:disabled {
          color: var(--button-color-disabled);
          cursor: default;
        }
      `}</style>
    </button>
  );
};
