import "./variables.css";

export interface ButtonProps {
  style?: string;
  onClick?: () => void;
  children: React.ReactNode;
}

export const Button = ({ onClick, style = "", children }: ButtonProps) => {
  return (
    <button type="button" onClick={onClick}>
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
          background-color: var(--button-background-color);
          text-align: center;
          ${style}
        }

        button:hover {
          background-color: var(--button-background-color-hover);
        }

        button:active {
          background-color: var(--button-background-color-active);
        }
      `}</style>
    </button>
  );
};
