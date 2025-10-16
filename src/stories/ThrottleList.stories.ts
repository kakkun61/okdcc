import { Meta, StoryObj } from "@storybook/nextjs-vite";

import { ThrottleList } from "@/components/ThrottleList";
import { ThrottleProps } from "@/components/Throttle";
import * as Dcc from "@/lib/dcc";
import lightImage from "@/assets/light.png";

const meta = {
  component: ThrottleList,
  tags: ["autodocs"],
} satisfies Meta<typeof ThrottleList>;

export default meta;

type Story = StoryObj<typeof meta>;

const throttleProps: ThrottleProps = {
  trainName: Dcc.createTrainName("DD-51"),
  functionInfos: [
    {
      number: Dcc.createFunctionNumber(1),
      label: "Light",
      image: lightImage,
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(2),
      label: "Horn",
      stateType: "momentary",
    },
    {
      number: Dcc.createFunctionNumber(3),
      label: "Bell",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(4),
      label: "Squeal",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(5),
      label: "Coupler",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(6),
      label: "Mute",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(7),
      label: "Smoke",
      stateType: "toggle",
    },
    {
      number: Dcc.createFunctionNumber(8),
      label: "Generator",
      stateType: "toggle",
    },
  ],
};

export const Default: Story = {
  args: {
    throttlePropsList: [
      throttleProps,
      { ...throttleProps, trainName: Dcc.createTrainName("C57") },
      { ...throttleProps, trainName: Dcc.createTrainName("JRW 221") },
    ],
  },
};
