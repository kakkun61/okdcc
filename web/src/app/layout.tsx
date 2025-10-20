import type { Metadata } from "next";

import "@/components/default.css";
import { AppBar } from "@/components/AppBar";

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
        <AppBar />
        <main>
          {children}
        </main>
      </body>
    </html>
  );
}
