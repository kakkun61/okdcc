"use client";

export interface SpeedMeterProps {
  // range 0-1
  speed: number;
  style?: string;
}

export const SpeedMeter = ({ speed, style = "" }: SpeedMeterProps) => {
  const outlineRadius = 45;
  const outlineStartX = (100 - 45) / 2;
  const outlineHeight = 45 * (1 + Math.sqrt(3) / 2);
  const outlineVerticalPadding = (100 - outlineHeight) / 2;
  const outlineStartY = 100 - outlineVerticalPadding;
  const outlineEndX = (100 - 45) / 2 + 45;
  const outlineEndY = 100 - outlineVerticalPadding;
  const coreCapRadius = 12;
  const coreCapCenterX = 50;
  const coreCapCenterY = outlineVerticalPadding + outlineRadius;
  const handLength = 35;
  const handWidth = 4;
  const handTipWidth = 0.5;
  const mainMarkLength = 8;
  const mainMarkWidth = 2;
  const subMarkLength = 4;
  const subMarkWidth = 1;
  const markStartAngle = -120;
  const markEndAngle = 120;
  const mainMarkStep = 30;
  const mainMarkCount = (markEndAngle - markStartAngle) / mainMarkStep + 1;
  const subMarkStep = mainMarkStep / 5;
  const subMarkCount = (mainMarkCount - 1) * 4;
  return (
    <div className="speed-meter">
      <svg
        xmlns="http://www.w3.org/2000/svg"
        viewBox="0 0 100 100"
        preserveAspectRatio="xMidYMid meet"
        role="img"
      >
        <title>Speed Meter</title>
        <defs>
          <polygon
            id="main-mark"
            points={`
              ${50 - mainMarkWidth / 2}, ${outlineVerticalPadding}
              ${50 + mainMarkWidth / 2}, ${outlineVerticalPadding}
              ${50 + mainMarkWidth / 2}, ${outlineVerticalPadding + mainMarkLength}
              ${50 - mainMarkWidth / 2}, ${outlineVerticalPadding + mainMarkLength}
          `}
          />
          <polygon
            id="sub-mark"
            points={`
              ${50 - subMarkWidth / 2}, ${outlineVerticalPadding}
              ${50 + subMarkWidth / 2}, ${outlineVerticalPadding}
              ${50 + subMarkWidth / 2}, ${outlineVerticalPadding + subMarkLength}
              ${50 - subMarkWidth / 2}, ${outlineVerticalPadding + subMarkLength}
          `}
          />
        </defs>
        <path
          d={`
            M ${outlineStartX} ${outlineStartY}
            A ${outlineRadius} ${outlineRadius} 0 1 1 ${outlineEndX} ${outlineEndY}
            L ${outlineStartX} ${outlineStartY}
          `}
          stroke="black"
          fill="white"
          strokeWidth="2"
          strokeLinecap="round"
        />
        {Array.from(
          { length: mainMarkCount },
          (_, i) => markStartAngle + i * mainMarkStep,
        ).map((angle) => (
          <use
            key={angle}
            href="#main-mark"
            transform={`rotate(${angle} 50 ${coreCapCenterY})`}
          />
        ))}
        {Array.from(
          { length: subMarkCount },
          (_, i) => markStartAngle + (1 + i + Math.floor(i / 4)) * subMarkStep,
        ).map((angle) => (
          <use
            key={angle}
            href="#sub-mark"
            transform={`rotate(${angle} 50 ${coreCapCenterY})`}
          />
        ))}
        <polygon
          id="hand"
          points={`
            ${50 + handTipWidth / 2}, ${coreCapCenterY - handLength}
            ${50 - handTipWidth / 2}, ${coreCapCenterY - handLength}
            ${50 - handWidth / 2}, ${coreCapCenterY}
            ${50 + handWidth / 2}, ${coreCapCenterY}
        `}
        />
        <circle
          cx={coreCapCenterX}
          cy={coreCapCenterY}
          r={coreCapRadius}
          stroke="black"
          fill="lightgray"
          strokeWidth="2"
        />
      </svg>
      <style jsx>{`
        .speed-meter {
          ${style}
        }
        #hand {
          transform-origin: 50% ${coreCapCenterY}px;
          transform: rotate(${markStartAngle + (markEndAngle - markStartAngle) * speed}deg);
          transition: transform 0.3s ease-in-out;
        }
      `}</style>
    </div>
  );
};
