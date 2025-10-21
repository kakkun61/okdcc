export interface ButtonProps {
  state?: "default" | "disabled";
  action?: "hover" | "active" | "focus" | null;
  style?: string;
  children: React.ReactNode;
  onClick?: (event: React.MouseEvent<HTMLButtonElement, MouseEvent>) => void;
  onPointerDown?: (event: React.PointerEvent<HTMLButtonElement>) => void;
  onPointerUp?: (event: React.PointerEvent<HTMLButtonElement>) => void;
}

export const Button = ({
  state = "default",
  action,
  style = "",
  children,
  onClick,
  onPointerDown,
  onPointerUp,
}: ButtonProps) => {
  return (
    <button
      type="button"
      className={`button ${state === "default" ? "" : state} ${action ?? ""}`}
      disabled={state === "disabled"}
      onClick={onClick}
      onPointerDown={onPointerDown}
      onPointerUp={onPointerUp}
    >
      {children}
      <style jsx>{`button { ${style} }`}</style>
    </button>
  );
};
