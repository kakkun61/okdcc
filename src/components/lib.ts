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

// TODO active と selected を分ける
// active は saturation を hover より高く
// selected は saturation を hover より低く
/**
 * ボタンのスタイル
 *
 * @remarks
 *
 * (状態 × ユーザーの操作)→スタイル
 *
 * ```txt
 *        | default disabled selected
 * -------+---------------------------
 * hover  | hover   disabled both
 * active | active  disabled both
 * focus  | focus   both     both
 * ```
 */
export const buttonStyle =
  // :hover など疑似属性が優先される
  // クラス属性が先、疑似属性が後
  css`
    button {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      padding: var(--button-padding-vertical) var(--button-padding-horizontal);
      border-style: none;
      border-radius: var(--button-border-radius);
      font-size: var(--button-font-size);
      font-weight: var(--button-font-weight);
      line-height: var(--button-line-height);
      cursor: pointer;
      color: var(--button-color);
      background-color: var(--button-background-color);
      overflow: clip;
      text-overflow: ellipsis;
    }

    button.hover:not(:disabled) {
      background-color: var(--button-background-color-hover);
    }

    button.active:not(:disabled) {
      background-color: var(--button-background-color-active);
    }

    @media (hover: hover) {
      button:hover:not(:disabled) {
        background-color: var(--button-background-color-hover);
      }
    }

    button:active:not(:disabled) {
      background-color: var(--button-background-color-active);
    }

    button:disabled {
      color: var(--button-color-disabled);
      cursor: default;
    }
  `;

export const toggleButtonStyle = css`
    button.toggle {
      border: solid var(--toggle-button-ring-width) var(--toggle-button-ring-color);
      box-shadow:
        inset 0 0 0 var(--toggle-button-ring-gap) var(--background-color);
    }

    button.toggle.selected {
      border: solid var(--toggle-button-ring-width) var(--toggle-button-ring-color-selected);
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

export const mmToPx = (mm: number): number => mm * 3.7795275591;
