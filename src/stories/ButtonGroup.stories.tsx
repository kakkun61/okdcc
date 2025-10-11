import { Meta, StoryObj } from "@storybook/nextjs-vite";
import { fn } from "storybook/test";

import { ButtonGroup } from "@/components/ButtonGroup";
import { Button } from "@/components/ButtonGroup/Button";

const meta = {
  component: ButtonGroup,
  tags: ["autodocs"],
  parameters: {
    layout: "centered",
  },
  argTypes: {
    children: {
      table: {
        disable: true,
      },
    },
  },
} satisfies Meta<typeof ButtonGroup>;

export default meta;

type Story = StoryObj<typeof meta>;

export const Primary: Story = {
  args: {
    children: (
      <>
        <Button onClick={fn()}>Button 1</Button>
        <Button onClick={fn()}>Button 2</Button>
        <Button onClick={fn()}>Button 3</Button>
      </>
    ),
  },
};
