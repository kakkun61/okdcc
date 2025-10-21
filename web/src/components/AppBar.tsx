"use client";

import "material-symbols/rounded.css";
import { UrlObject } from "url";

import "./variables.css";
import "./default.css";
import { Button } from "./AppBar/Button";

export type AppBarProps = {
  backHref?: string | UrlObject;
  style?: string;
  children?: React.ReactNode;
};

export const AppBar = ({ backHref, style = "", children }: AppBarProps) => {
  return (
    <header>
      {backHref === undefined ? (
        <></>
      ) : (
        <Button href={backHref}>
          <span className="material-symbols-rounded">arrow_back</span>
        </Button>
      )}
      <span className="padding" />
      {children}
      <style jsx>{`
        header {
          display: flex;
          align-items: center;
          height: var(--app-bar-height);
          width: 100%;
          padding: var(--app-bar-padding);
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
