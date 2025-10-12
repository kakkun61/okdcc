import { buttonStyle } from "./lib";

export interface ButtonProps {
  state?: "default" | "disabled";
  action?: "hover" | "active" | "focus" | null;
  style?: string;
  children: React.ReactNode;
  onClick?: () => void;
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
      className={`${state === "default" ? "" : state} ${action ?? ""}`}
      disabled={state === "disabled"}
      onClick={onClick}
    >
      {children}
      <style jsx>{buttonStyle}</style>
      <style jsx>{`button { ${style} }`}</style>
    </button>
  );
};
