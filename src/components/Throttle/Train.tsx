"use client";

import { TrainName } from "@/lib/dcc";

export interface TrainProps {
  name: TrainName;
}

export const Train = ({ name }: TrainProps) => {
  return (
    <div>
      {name}
      <style jsx>{`
        div {
          font-size: var(--font-size-xlarge);
          padding: var(--gap-medium);
        }
      `}</style>
    </div>
  );
};
