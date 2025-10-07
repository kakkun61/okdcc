import "./variables.css";

export interface ButtonProps {
  label: string;
  style?: string;
  onClick?: () => void;
}

export const Button = ({ label, onClick, style }: ButtonProps) => {
  return (
    <button type="button" onClick={onClick}>
      {label}
      <style jsx>{`
        button {
          display: inline-block;
          padding: var(--button-padding-vertical) var(--button-padding-horizontal);
          border-radius: var(--button-border-radius);
          font-size: var(--button-font-size);
          font-weight: var(--button-font-weight);
          line-height: var(--button-line-height);
          cursor: pointer;
          ${style}
        }
      `}</style>
    </button>
  );
};
