import type { Meta, StoryObj } from "@storybook/nextjs-vite";

import { fn } from "storybook/test";

import { Button } from "../components/Button";

const meta = {
  component: Button,
  parameters: {
    layout: "centered",
  },
  tags: ["autodocs"],
  args: { onClick: fn() },
  argTypes: {
    children: {
      table: {
        disable: true,
      },
    },
  },
} satisfies Meta<typeof Button>;

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
    state: "hover",
  },
};

export const Active: Story = {
  args: {
    children: "Button",
    state: "active",
  },
};

export const Disabled: Story = {
  args: {
    children: "Button",
    state: "disabled",
  },
};
