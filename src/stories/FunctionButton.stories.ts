import type { Meta, StoryObj } from "@storybook/nextjs-vite";

import { FunctionButton } from "./FunctionButton";

const meta = {
  component: FunctionButton,
} satisfies Meta<typeof FunctionButton>;

export default meta;

type Story = StoryObj<typeof meta>;

export const Primary: Story = {
  args: {},
};
