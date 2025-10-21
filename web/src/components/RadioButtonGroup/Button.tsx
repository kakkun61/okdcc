export interface ButtonProps {
  id: string;
  state?: "default" | "disabled" | "selected";
  action?: "hover" | "active" | "focus" | null;
  style?: string;
  onClick?: (event: React.MouseEvent<HTMLButtonElement, MouseEvent>) => void;
  children: React.ReactNode;
}

export const Button = ({
  state = "default",
  action,
  style = "",
  children,
  onClick,
}: ButtonProps) => {
  return (
    <button
      type="button"
      className={`button toggle-button button-group-button ${state === "default" ? "" : state} ${action ?? ""}`}
      disabled={state === "disabled"}
      onClick={onClick}
    >
      {children}
      <style jsx>{`button { ${style} }`}</style>
    </button>
  );
};
