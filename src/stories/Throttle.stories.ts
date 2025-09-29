import type { Meta, StoryObj } from "@storybook/nextjs-vite";

import { Throttle } from "../components/Throttle";

const meta = {
  component: Throttle,
} satisfies Meta<typeof Throttle>;

export default meta;

type Story = StoryObj<typeof meta>;

export const Primary: Story = {
  args: {},
};
