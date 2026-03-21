import type { Metadata } from "next";
import { Inter } from "next/font/google";
import "./globals.css";

const inter = Inter({
  variable: "--font-inter",
  subsets: ["latin"],
});

export const metadata: Metadata = {
  title: "Schedulify — AI-Powered Course Schedule Builder",
  description:
    "Automatically generate conflict-free course schedules. Upload your course list, set your constraints, and let Schedulify find every valid schedule — powered by AI.",
  openGraph: {
    title: "Schedulify — AI-Powered Course Schedule Builder",
    description:
      "Automatically generate conflict-free course schedules in seconds.",
    type: "website",
  },
};

export default function RootLayout({
  children,
}: Readonly<{ children: React.ReactNode }>) {
  return (
    <html lang="en" className={`${inter.variable} h-full antialiased`}>
      <body className="min-h-full flex flex-col">{children}</body>
    </html>
  );
}
