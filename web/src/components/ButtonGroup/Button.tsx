export interface ButtonProps {
  state?: "default" | "hover" | "active" | "disabled";
  style?: string;
  children: React.ReactNode;
  onClick?: (event: React.MouseEvent<HTMLButtonElement, MouseEvent>) => void;
}

export const Button = ({
  state = "default",
  style = "",
  children,
  onClick,
}: ButtonProps) => {
  return (
    <button
      type="button"
      className={`button button-group-button ${state === "default" ? "" : state}`}
      disabled={state === "disabled"}
      onClick={onClick}
    >
      {children}
      <style jsx>{`button { ${style} }`}</style>
    </button>
  );
};
