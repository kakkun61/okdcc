"use client";

import { TrainName } from "@/lib/dcc";

export interface TrainProps {
  name: TrainName;
}

export const Train = ({ name }: TrainProps) => {
  return (
    <div>
      <img src="https://placehold.co/60x60" />
      {name}
    </div>
  );
};
