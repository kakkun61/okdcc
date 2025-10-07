"use client";

export interface SpeedMeterProps {
  style?: string;
}

export const SpeedMeter = ({ style = "" }: SpeedMeterProps) => {
  return (
    <div className="speed-meter">
      <img src="https://placehold.co/120x120" />
      <style jsx>{`
        .speed-meter {
          ${style}
        }
      `}</style>
    </div>
  );
};
