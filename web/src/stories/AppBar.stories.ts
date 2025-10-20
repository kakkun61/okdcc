import type { Meta, StoryObj } from "@storybook/nextjs-vite";

import { AppBar } from "@/components/AppBar";

const meta = {
  component: AppBar,
  parameters: {
    layout: "centered",
  },
  tags: ["autodocs"],
  args: {
    style: "width: 100mm",
  },
} satisfies Meta<typeof AppBar>;

export default meta;

type Story = StoryObj<typeof meta>;

export const Default: Story = {
  args: {},
};
