import { buttonGroupButtonStyle, buttonStyle } from "../lib";

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
      <style jsx>{buttonStyle}</style>
      <style jsx>{buttonGroupButtonStyle}</style>
      <style jsx>{`
        button {
          background-color: ${active ? "var(--button-background-color-active)" : "var(--button-background-color)"};
          ${style}
        }
      `}</style>
    </button>
  );
};
