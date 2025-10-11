import "../variables.css";

export interface ButtonProps {
  id: string;
  active?: boolean;
  style?: string;
  onClick?: () => void;
  children: React.ReactNode;
}

export const Button = ({
  active,
  onClick,
  style = "",
  children,
}: ButtonProps) => {
  return (
    <button type="button" onClick={onClick}>
      {children}
      <style jsx>{`
        button {
          display: inline-block;
          min-width: 10mm;
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

        button:hover {
          background-color: var(--button-background-color-hover);
        }

        button:active {
          background-color: var(--button-background-color-active);
        }

        button:first-child {
          border-start-start-radius: var(--button-border-radius);
          border-end-start-radius: var(--button-border-radius);
          border-inline-start-width: var(--button-border-width);
        }

        button:last-child {
          border-start-end-radius: var(--button-border-radius);
          border-end-end-radius: var(--button-border-radius);
        }
      `}</style>
    </button>
  );
};
