import { ButtonGroup } from "@/components/ButtonGroup";
import { Meta, StoryObj } from "@storybook/nextjs-vite";
import { fn } from "storybook/internal/test";

const meta = {
  component: ButtonGroup,
  tags: ["autodocs"],
  parameters: {
    layout: "centered",
  },
} satisfies Meta<typeof ButtonGroup>;

export default meta;

type Story = StoryObj<typeof meta>;

export const Primary: Story = {
  args: {
    buttonProps: [
      { label: "Button 1", onClick: fn() },
      { label: "Button 2", onClick: fn() },
      { label: "Button 3", onClick: fn() },
    ],
  },
};
