"use client";

import * as Dcc from "@/lib/dcc";

export interface FunctionButtonProps {
  info: Dcc.FunctionInfo;
  onClick: (num: Dcc.FunctionNumber) => void;
  style?: string;
}

export const FunctionButton = ({
  info,
  onClick,
  style = "",
}: FunctionButtonProps) => {
  return (
    <button onClick={() => onClick(info.number)} className="button">
      {info.label}
      <style jsx>{`
        .button {
          border-style: none;
          text-align: center;
          overflow: hidden;
          text-overflow: ellipsis;
          white-space: nowrap;
          ${style}
        }
      `}</style>
    </button>
  );
};
