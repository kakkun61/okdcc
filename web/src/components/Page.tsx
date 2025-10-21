import { usePathname } from "next/navigation";

import { AppBar } from "./AppBar";

export type PageProps = {
  children: React.ReactNode;
};

export const Page = ({ children }: PageProps) => {
  const pathname = usePathname();
  return (
    <>
      <AppBar backHref={pathname === "/" ? undefined : "/"} />
      <main>{children}</main>
    </>
  );
};
