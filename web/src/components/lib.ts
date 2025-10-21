// components/lib.ts はプロジェクトの logic.ts に依存してもよい
// React や Next.js に依存してもよい

import React from "react";
import { StaticImageData } from "next/image";
import css from "styled-jsx/css";
import * as Next from "next/dist/shared/lib/app-router-context.shared-runtime";

import * as Logic from "@/logic";
import lightImage from "@/assets/light.png";

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

const builtinImages = {
  light: lightImage,
};

export type FunctionInfo = {
  label: string;
  number: Logic.FunctionNumber;
  image?: StaticImageData;
  stateType: "momentary" | "toggle";
};

export const makeFunctionInfo = (info: Logic.FunctionInfo): FunctionInfo => ({
  label: info.label,
  number: info.number,
  image:
    info?.image &&
    info.image.type === "builtin" &&
    info.image.id in builtinImages
      ? builtinImages[info.image.id as keyof typeof builtinImages]
      : undefined,
  stateType: info.stateType,
});

export type ThrottleInfo = {
  trainInfo: Logic.TrainName;
  functionInfos: FunctionInfo[];
};

export const mmToPx = (mm: number): number => mm * 3.7795275591;
