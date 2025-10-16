import type { Metadata } from "next";
import "@/components/default.css";

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
      <body>{children}</body>
    </html>
  );
}
