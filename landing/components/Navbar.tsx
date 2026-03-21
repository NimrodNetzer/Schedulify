"use client";

import { useState, useEffect } from "react";
import { Calendar, Menu, X } from "lucide-react";

const GITHUB_URL = "https://github.com/Schedulify-org/Schedulify";

export default function Navbar() {
  const [scrolled, setScrolled] = useState(false);
  const [open, setOpen] = useState(false);

  useEffect(() => {
    const handler = () => setScrolled(window.scrollY > 8);
    window.addEventListener("scroll", handler);
    return () => window.removeEventListener("scroll", handler);
  }, []);

  return (
    <header
      className={`fixed top-0 inset-x-0 z-50 transition-shadow duration-200 bg-[var(--color-surface)] border-b border-[var(--color-border)] ${
        scrolled ? "shadow-sm" : ""
      }`}
    >
      <div className="max-w-6xl mx-auto px-6 h-16 flex items-center justify-between">
        {/* Logo */}
        <div className="flex items-center gap-2">
          <Calendar className="w-5 h-5 text-[var(--color-primary)]" />
          <span className="text-[var(--color-text-main)] font-semibold text-lg tracking-tight">
            Schedulify
          </span>
        </div>

        {/* Desktop nav */}
        <nav className="hidden md:flex items-center gap-6 text-sm text-[var(--color-text-muted)]">
          <a href="#features" className="hover:text-[var(--color-text-main)] transition-colors">Features</a>
          <a href="#how-it-works" className="hover:text-[var(--color-text-main)] transition-colors">How it works</a>
          <a href="#tech" className="hover:text-[var(--color-text-main)] transition-colors">Tech</a>
        </nav>

        {/* CTA */}
        <div className="hidden md:flex items-center gap-3">
          <a
            href={GITHUB_URL}
            target="_blank"
            rel="noopener noreferrer"
            className="text-sm text-[var(--color-text-muted)] hover:text-[var(--color-text-main)] transition-colors"
          >
            GitHub
          </a>
          <a
            href="#download"
            className="bg-[var(--color-charcoal)] text-white text-sm px-4 py-2 rounded-lg hover:bg-[var(--color-charcoal-hover)] transition-colors"
          >
            Download
          </a>
        </div>

        {/* Mobile hamburger */}
        <button
          className="md:hidden p-1 text-[var(--color-text-muted)]"
          onClick={() => setOpen(!open)}
          aria-label="Toggle menu"
        >
          {open ? <X className="w-5 h-5" /> : <Menu className="w-5 h-5" />}
        </button>
      </div>

      {/* Mobile menu */}
      {open && (
        <div className="md:hidden border-t border-[var(--color-border)] bg-[var(--color-surface)] px-6 py-4 flex flex-col gap-4 text-sm">
          <a href="#features" onClick={() => setOpen(false)} className="text-[var(--color-text-muted)]">Features</a>
          <a href="#how-it-works" onClick={() => setOpen(false)} className="text-[var(--color-text-muted)]">How it works</a>
          <a href="#tech" onClick={() => setOpen(false)} className="text-[var(--color-text-muted)]">Tech</a>
          <a href={GITHUB_URL} target="_blank" rel="noopener noreferrer" className="text-[var(--color-text-muted)]">GitHub</a>
          <a
            href="#download"
            onClick={() => setOpen(false)}
            className="bg-[var(--color-charcoal)] text-white px-4 py-2 rounded-lg text-center"
          >
            Download
          </a>
        </div>
      )}
    </header>
  );
}
