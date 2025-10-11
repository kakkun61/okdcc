import "../variables.css";

export interface ButtonProps {
  id: string;
  state?: "default" | "hover" | "active" | "disabled";
  active?: boolean;
  style?: string;
  onClick?: () => void;
  children: React.ReactNode;
}

export const Button = ({
  state = "default",
  active,
  style = "",
  children,
  onClick,
}: ButtonProps) => {
  return (
    <button
      type="button"
      className={state === "default" ? "" : state}
      disabled={state === "disabled"}
      onClick={onClick}
    >
      {children}
      <style jsx>{`
        button {
          display: inline-block;
          min-width: 10mm;
          margin-inline-end: var(--button-group-gap);
          padding: var(--button-padding-vertical) var(--button-padding-horizontal);
          border-style: none;
          border-radius: 0;
          border-inline-end-width: 0;
          font-size: var(--button-font-size);
          font-weight: var(--button-font-weight);
          text-align: center;
          line-height: var(--button-line-height);
          cursor: pointer;
          background-color: ${active ? "var(--button-background-color-active)" : "var(--button-background-color)"};
          white-space: normal;
          flex: /* grow */ 1 /* shrink */ 1 /* basis */ 50%;
          ${style}
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
      `}</style>
    </button>
  );
};
