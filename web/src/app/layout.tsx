import type { Metadata } from "next";

import "@/components/default.css";
import { AppContext } from "@/components/AppContext";

export const metadata: Metadata = {
  title: "OKDCC Web Throttle",
  description: "Throttle control for model railway DCC",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body>
        <AppContext>{children}</AppContext>
      </body>
    </html>
  );
}
