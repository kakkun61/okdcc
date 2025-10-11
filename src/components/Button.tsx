import "./variables.css";

export interface ButtonProps {
  state?: "default" | "hover" | "active" | "disabled";
  style?: string;
  onClick?: () => void;
  children: React.ReactNode;
}

export const Button = ({ state = "default", style = "", onClick, children }: ButtonProps) => {
  return (
    <button type="button" className={state === "default"?"":state} disabled={state === "disabled"} onClick={onClick}>
      {children}
      <style jsx>{`
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
          ${style}
        }

        /* :hover など疑似属性が優先されるようにそれらはクラス属性 .hover .active .disabled の後に書く */
        button.hover:not(:disabled) {
          background-color: var(--button-background-color-hover);
        }

        button.active:not(:disabled) {
          background-color: var(--button-background-color-active);
        }

        button.disabled {
          color: var(--button-text-color-disabled);
          cursor: default;
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
      `}</style>
    </button>
  );
};
