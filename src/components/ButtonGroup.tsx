import React from "react";
import "./variables.css";

export interface ButtonGroupProps {
  style?: string;
  children: React.ReactNode;
}

export const ButtonGroup = ({ style = "", children }: ButtonGroupProps) => {
  return (
    <span className="button-group">
      {children}
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
