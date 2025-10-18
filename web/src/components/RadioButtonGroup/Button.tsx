import { buttonGroupButtonStyle, buttonStyle, toggleButtonStyle } from "../lib";

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
      className={`toggle ${state === "default" ? "" : state} ${action ?? ""}`}
      disabled={state === "disabled"}
      onClick={onClick}
    >
      {children}
      <style jsx>{buttonStyle}</style>
      <style jsx>{toggleButtonStyle}</style>
      <style jsx>{buttonGroupButtonStyle}</style>
      <style jsx>{`button { ${style} }`}</style>
    </button>
  );
};
