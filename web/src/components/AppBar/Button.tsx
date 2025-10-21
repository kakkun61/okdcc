import Link from "next/link";
import { UrlObject } from "url";

export type ButtonProps = {
  href: string | UrlObject;
  children?: React.ReactNode;
};

export const Button = ({ href, children }: ButtonProps) => {
  return (
    <span className="app-bar-button">
      <div className="background" />
      <Link href={href}>{children}</Link>
    </span>
  );
};
