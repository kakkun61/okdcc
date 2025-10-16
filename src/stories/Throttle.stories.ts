import { fn } from "storybook/internal/test";
import type { Meta, StoryObj } from "@storybook/nextjs-vite";

import "@/components/default.css";
import { Throttle } from "@/components/Throttle";
import * as Dcc from "@/lib/dcc";
import lightImage from "@/assets/light.png";

const meta = {
  component: Throttle,
  tags: ["autodocs"],
  args: {
    onClickSpeedUp: fn(),
    onClickSpeedDown: fn(),
    onClickForward: fn(),
    onClickBackward: fn(),
    onClickEmergencyStop: fn(),
  },
} satisfies Meta<typeof Throttle>;

export default meta;

type Story = StoryObj<typeof meta>;

export const Primary: Story = {
  args: {
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
        label: "Wiper",
        stateType: "toggle",
      },
      {
        number: Dcc.createFunctionNumber(5),
        label: "Coupler",
        stateType: "toggle",
      },
      {
        number: Dcc.createFunctionNumber(6),
        label: "Sound",
        stateType: "toggle",
      },
      {
        number: Dcc.createFunctionNumber(7),
        label: "Smoke",
        stateType: "toggle",
      },
      {
        number: Dcc.createFunctionNumber(8),
        label: "Music",
        stateType: "toggle",
      },
      {
        number: Dcc.createFunctionNumber(9),
        label:
          "Extremely Too Long Long Long Long Long Long Long Long Long Long Name",
        stateType: "toggle",
      },
    ],
  },
};
