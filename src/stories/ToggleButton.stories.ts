import type { Meta, StoryObj } from "@storybook/nextjs-vite";

import { fn } from "storybook/test";

import { ToggleButton } from "../components/ToggleButton";

const meta = {
  component: ToggleButton,
  parameters: {
    layout: "centered",
  },
  tags: ["autodocs"],
  args: { onChangeState: fn() },
  argTypes: {
    action: {
      control: "radio",
      options: ["hover", "active", "focus", "null"],
      mapping: { hover: "hover", active: "active", focus: "focus", null: null },
    },
    children: {
      table: {
        disable: true,
      },
    },
  },
} satisfies Meta<typeof ToggleButton>;

export default meta;
type Story = StoryObj<typeof meta>;

export const Default: Story = {
  args: {
    children: "Button",
  },
};

export const Hover: Story = {
  args: {
    children: "Button",
    action: "hover",
  },
};

export const Active: Story = {
  args: {
    children: "Button",
    action: "active",
  },
};

export const Disabled: Story = {
  args: {
    children: "Button",
    state: "disabled",
  },
};
