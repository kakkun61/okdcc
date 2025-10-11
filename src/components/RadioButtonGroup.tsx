import React from "react";
import "./variables.css";

import { Button } from "./RadioButtonGroup/Button";

export interface RadioButtonGroupProps {
  style?: string;
  initialActivated?: string;
  onActivate?: (which: string) => void;
  children: React.ReactNode;
}

const updateButtonProps = (
  element: React.ReactNode,
  update: (
    props: React.ComponentProps<typeof Button>,
  ) => Partial<React.ComponentProps<typeof Button>>,
): React.ReactNode => {
  // ReactElement string number bigint Iterable<ReactNode> ReactPortal boolean null undefined Promise<AwaitedReactNode>
  if (element === null) {
    return element;
  }
  // ReactElement string number bigint Iterable<ReactNode> ReactPortal boolean undefined Promise<AwaitedReactNode>
  switch (typeof element) {
    case "string":
    case "number":
    case "boolean":
    case "bigint":
    case "undefined":
      return element;
    case "object":
      break;
    default:
      throw new Error(`Unexpected element type: ${typeof element}`);
  }
  // ReactElement Iterable<ReactNode> ReactPortal Promise<AwaitedReactNode>
  if (Symbol.iterator in Object(element)) {
    if (Array.isArray(element)) {
      return element.map((child) => updateButtonProps(child, update));
    }
    // Map とか Set とかのときはどう形チェックしてどう map するの？
    return element;
  }
  if (React.isValidElement(element)) {
    if (element.type === React.Fragment) {
      return React.Children.map(
        (element.props as React.FragmentProps).children,
        (child) => updateButtonProps(child, update),
      );
    }
    if (element.type === Button) {
      const props = element.props as React.ComponentProps<typeof Button>;
      return React.cloneElement(element, update(props));
    }
    return element;
  }
  // Promise<AwaitedReactNode>
  return element;
};

export const RadioButtonGroup = ({
  style = "",
  initialActivated,
  onActivate,
  children,
}: RadioButtonGroupProps) => {
  const [selected, setSelected] = React.useState<string | null>(
    initialActivated ?? null,
  );
  const children_ = updateButtonProps(children, (props) => ({
    active: props.id === selected,
    onClick: () => {
      props.onClick?.();
      setSelected(props.id);
      onActivate?.(props.id);
    },
  }));
  return (
    <span className="button-group">
      {children_}
      <style jsx>{`
        .button-group {
          display: flex;
          min-width: fit-content;
          white-space: nowrap;
          ${style}
        }
      `}</style>
    </span>
  );
};
