import React from "react";
import { StaticImageData } from "next/image";
import css from "styled-jsx/css";

import * as dcc from "@/lib/dcc";
import "./variables.css";

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

export const buttonStyle = css`
  button {
    display: inline-block;
    padding: var(--button-padding-vertical) var(--button-padding-horizontal);
    border-style: none;
    border-radius: var(--button-border-radius);
    font-size: var(--button-font-size);
    font-weight: var(--button-font-weight);
    text-align: center;
    line-height: var(--button-line-height);
    cursor: pointer;
    color: var(--button-color);
    background-color: var(--button-background-color);
    text-align: center;
  }

  /* :hover など疑似属性が優先されるようにそれらはクラス属性 .hover .active .disabled の後に書く */
  button.hover:not(:disabled) {
    background-color: var(--button-background-color-hover);
  }

  button.active:not(:disabled) {
    background-color: var(--button-background-color-active);
  }

  button:hover:not(:disabled) {
    background-color: var(--button-background-color-hover);
  }

  button:active:not(:disabled) {
    background-color: var(--button-background-color-active);
  }

  button:disabled {
    color: var(--button-color-disabled);
    cursor: default;
  }
`;

export const buttonGroupButtonStyle = css`
  button {
    min-width: 10mm;
    margin-inline-end: var(--button-group-gap);
    padding: var(--button-padding-vertical) var(--button-padding-horizontal);
    border-style: none;
    border-radius: 0;
    flex: /* grow */ 1 /* shrink */ 1 /* basis */ 50%;
  }

  button:first-child {
    border-start-start-radius: var(--button-border-radius);
    border-end-start-radius: var(--button-border-radius);
    border-inline-start-width: var(--button-border-width);
  }

  button:last-child {
    margin-inline-end: 0;
    border-start-end-radius: var(--button-border-radius);
    border-end-end-radius: var(--button-border-radius);
  }

  button:first-child {
    border-start-start-radius: var(--button-border-radius);
    border-end-start-radius: var(--button-border-radius);
    border-inline-start-width: var(--button-border-width);
  }

  button:last-child {
    margin-inline-end: 0;
    border-start-end-radius: var(--button-border-radius);
    border-end-end-radius: var(--button-border-radius);
  }
`;
