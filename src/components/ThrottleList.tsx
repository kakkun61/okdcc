"use client";

import { Throttle, ThrottleProps } from "./Throttle";

export interface ThrottleListProps {
  throttlePropsList: ThrottleProps[];
  style?: string;
}

export const ThrottleList = ({
  throttlePropsList,
  style,
}: ThrottleListProps) => {
  return (
    <div className="throttle-list">
      {throttlePropsList.map((props) => (
        <Throttle key={props.trainName} {...props} />
      ))}
      <style jsx>{`
        .throttle-list {
          display: flex;
          flex-direction: row;
          gap: var(--gap-large);
          height: 100%;
          overflow-y: hidden;
        }
      `}</style>
      <style jsx>{`.throttle { ${style} }`}</style>
    </div>
  );
};
