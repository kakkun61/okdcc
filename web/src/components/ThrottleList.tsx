"use client";

import { useEffect, useRef, useState } from "react";
import { Throttle, ThrottleProps } from "./Throttle";

export interface ThrottleListProps {
  throttlePropsList: ThrottleProps[];
  style?: string;
}

export const ThrottleList = ({
  throttlePropsList,
  style,
}: ThrottleListProps) => {
  const throttleListRef = useRef<HTMLDivElement>(null);
  const lastThrottleRef = useRef<HTMLDivElement>(null);
  const [spacerWidth, setSpacerWidth] = useState(0);

  useEffect(() => {
    const calculateSpacerWidth = () => {
      if (!throttleListRef.current || !lastThrottleRef.current) return 0;

      const listWidth = throttleListRef.current.clientWidth;
      const throttleWidth = lastThrottleRef.current.offsetWidth;
      const gap = parseFloat(
        getComputedStyle(throttleListRef.current).getPropertyValue("gap") ||
          "0",
      );

      return Math.max(0, listWidth - throttleWidth - gap);
    };

    setSpacerWidth(calculateSpacerWidth());

    const resizeObserver = new ResizeObserver(() => {
      setSpacerWidth(calculateSpacerWidth());
    });
    if (throttleListRef.current) {
      resizeObserver.observe(throttleListRef.current);
    }
    if (lastThrottleRef.current) {
      resizeObserver.observe(lastThrottleRef.current);
    }

    return () => {
      resizeObserver.disconnect();
    };
  }, [throttlePropsList.length]);

  return (
    <div className="throttle-list" ref={throttleListRef}>
      {throttlePropsList.map((props, index) => (
        <Throttle
          key={index}
          ref={index === throttlePropsList.length - 1 ? lastThrottleRef : null}
          style={`scroll-snap-align: start;${props.style}`}
          {...props}
        />
      ))}
      <div
        className="spacer"
        style={{ width: `${spacerWidth}px`, height: "100%", flex: "0 0 auto" }}
      />
      <style jsx>{`
        .throttle-list {
          display: flex;
          flex-direction: row;
          gap: var(--gap-large);
          height: 100%;
          overflow-x: scroll;
          overflow-y: hidden;
          scroll-snap-type: x mandatory;
        }
      `}</style>
      <style jsx>{`.throttle { ${style} }`}</style>
    </div>
  );
};
