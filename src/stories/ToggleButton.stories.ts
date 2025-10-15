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
  name: "Default + Hover",
  args: {
    children: "Button",
    action: "hover",
  },
};

export const Active: Story = {
  name: "Default + Active",
  args: {
    children: "Button",
    action: "active",
  },
};

export const Selected: Story = {
  name: "Selected",
  args: {
    children: "Button",
    state: "selected",
  },
};

export const HoverSelected: Story = {
  name: "Selected + Hover",
  args: {
    children: "Button",
    state: "selected",
    action: "hover",
  },
};

export const ActiveSelected: Story = {
  name: "Selected + Active",
  args: {
    children: "Button",
    state: "selected",
    action: "active",
  },
};

export const Disabled: Story = {
  args: {
    children: "Button",
    state: "disabled",
  },
};
