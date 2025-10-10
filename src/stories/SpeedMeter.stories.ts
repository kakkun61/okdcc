import type { Meta, StoryObj } from "@storybook/nextjs-vite";

import { SpeedMeter } from "@/components/SpeedMeter";

const meta = {
  component: SpeedMeter,
  title: "Throttle/SpeedMeter",
  parameters: {
    layout: "centered",
  },
  tags: ["autodocs"],
  argTypes: {
    speed: {
      control: { type: "range", min: 0, max: 1, step: 0.01 },
    },
  },
} satisfies Meta<typeof SpeedMeter>;

export default meta;

type Story = StoryObj<typeof meta>;

export const Primary: Story = {
  args: {
    speed: 0,
    style: "width: 200px; height: 200px;",
  },
};
