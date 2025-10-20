"use client";

import "material-symbols/rounded.css";

import "./variables.css";

export type AppBarProps = {
  style?: string;
};

export const AppBar = ({ style = "" }: AppBarProps) => {
  return (
    <header>
      <span className="material-symbols-rounded">arrow_back</span>
      App Bar
      <span className="padding" />
      <style jsx>{`
        header {
          display: flex;
          align-items: center;
          height: var(--app-bar-height);
          width: 100%;
          flex: 0 0 var(--app-bar-height);
        }
        .padding {
          flex: 1 1 0;
        }
      `}</style>
      <style jsx>{`
        header {
          ${style}
        }
      `}</style>
    </header>
  );
};
