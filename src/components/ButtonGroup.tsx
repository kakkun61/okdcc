import "./variables.css";

export interface ButtonGroupButtonProps {
  label: string;
  onClick?: () => void;
}

export interface ButtonGroupProps {
  buttonProps: ButtonGroupButtonProps[];
  style?: string;
}

export const ButtonGroup = ({ buttonProps, style = "" }: ButtonGroupProps) => {
  const buttonWidth = (100 / buttonProps.length) + "%";
  return (
    <span className="button-group">
      {buttonProps.map(({ label, onClick }, index) =>
        (() => {
          const position =
            index === 0
              ? "start"
              : index === buttonProps.length - 1
                ? "end"
                : "mid";
          return (
            <button key={index} className={`${position}`} onClick={onClick}>
              {label}
            </button>
          );
        })(),
      )}
      <style jsx>{`
        .button-group {
          display: inline-block;
          ${style}
        }

        button {
          display: inline-block;
          width: ${buttonWidth};
          padding: var(--button-padding-vertical) var(--button-padding-horizontal);
          border-radius: var(--button-border-radius);
          font-size: var(--button-font-size);
          font-weight: var(--button-font-weight);
          line-height: var(--button-line-height);
          cursor: pointer;
        }

        button.start {
          border-start-end-radius: 0;
          border-end-end-radius: 0;
          border-inline-end-width: 0;
        }

        button.mid {
          border-radius: 0;
          border-inline-end-width: 0;
        }

        button.end {
          border-start-start-radius: 0;
          border-end-start-radius: 0;
        }
      `}</style>
    </span>
  );
};
