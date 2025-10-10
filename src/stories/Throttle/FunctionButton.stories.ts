import type { Meta, StoryObj } from "@storybook/nextjs-vite";

import { fn } from "storybook/test";

import { FunctionButton } from "@/components/Throttle/FunctionButton";
import { createFunctionNumber } from "@/lib/dcc";

const meta = {
  component: FunctionButton,
  title: "Throttle/FunctionButton",
  parameters: {
    layout: "centered",
  },
  tags: ["autodocs"],
  args: { onClick: fn() },
} satisfies Meta<typeof FunctionButton>;

export default meta;

type Story = StoryObj<typeof meta>;

export const Primary: Story = {
  args: {
    info: { number: createFunctionNumber(1), label: "Light" },
  },
};
