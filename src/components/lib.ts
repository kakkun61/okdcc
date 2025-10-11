import React from "react";
import { StaticImageData } from "next/image";
import * as dcc from "@/lib/dcc";

export const updateComponentPropsRecursively = (
  component:
    | keyof React.JSX.IntrinsicElements
    | React.JSXElementConstructor<any>,
  element: React.ReactNode,
  update: (
    props: React.ComponentProps<typeof component>,
  ) => Partial<React.ComponentProps<typeof component>>,
): React.ReactNode => {
  if (element === null) {
    return element;
  }
  switch (typeof element) {
    case "string":
    case "number":
    case "boolean":
    case "bigint":
    case "undefined":
      return element;
    case "object":
      break;
  }
  if (Symbol.iterator in element) {
    if (Array.isArray(element)) {
      return element.map((child) =>
        updateComponentPropsRecursively(component, child, update),
      );
    }
    // 未知の反復処理プロトコル準拠オブジェクトのときはどう map するの？
    return element;
  }
  if (React.isValidElement(element)) {
    if (element.type === React.Fragment) {
      return React.Children.map(
        (element.props as React.FragmentProps).children,
        (child) => updateComponentPropsRecursively(component, child, update),
      );
    }
    if (element.type === component) {
      const props = element.props as React.ComponentProps<typeof component>;
      return React.cloneElement(element, update(props));
    }
    return element;
  }
  if (element instanceof Promise) {
    return element;
  }
};

export type FunctionInfo = dcc.FunctionInfo<StaticImageData>;
