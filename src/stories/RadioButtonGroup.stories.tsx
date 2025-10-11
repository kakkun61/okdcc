import { Meta, StoryObj } from "@storybook/nextjs-vite";
import { fn } from "storybook/test";

import { RadioButtonGroup } from "@/components/RadioButtonGroup";
import { Button } from "@/components/RadioButtonGroup/Button";

const meta = {
  component: RadioButtonGroup,
  tags: ["autodocs"],
  parameters: {
    layout: "centered",
  },
  argTypes: {
    initialActivated: {
      table: {
        disable: true,
      },
    },
    children: {
      table: {
        disable: true,
      },
    },
  },
  args: {
    onActivate: fn(),
  },
} satisfies Meta<typeof RadioButtonGroup>;

export default meta;

type Story = StoryObj<typeof meta>;

const children = (
  <>
    <Button id="1" onClick={fn()}>
      Button 1
    </Button>
    <Button id="2" onClick={fn()}>
      Button 2
    </Button>
    <Button id="3" onClick={fn()}>
      Button 3
    </Button>
  </>
);

export const Default: Story = {
  args: {
    initialActivated: "1",
    children: children,
  },
};

export const Disabled: Story = {
  args: {
    state: "disabled",
    children: children,
  },
};

export const Mixed: Story = {
  args: {
    initialActivated: "1",
    children: (
      <>
        <Button id="1" onClick={fn()}>
          Button 1
        </Button>
        <Button id="2" state="disabled" onClick={fn()}>
          Button 2
        </Button>
        <Button id="3" onClick={fn()}>
          Button 3
        </Button>
      </>
    ),
  },
};
