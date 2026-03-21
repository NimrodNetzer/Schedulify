import { Calendar } from "lucide-react";

const GITHUB_URL = "https://github.com/Schedulify-org/Schedulify";

export default function Footer() {
  return (
    <footer className="bg-[var(--color-surface)] border-t border-[var(--color-border)] py-8 px-6">
      <div className="max-w-6xl mx-auto flex flex-col sm:flex-row items-center justify-between gap-4 text-sm text-[var(--color-text-faint)]">
        <div className="flex items-center gap-2">
          <Calendar className="w-4 h-4 text-[var(--color-primary)]" />
          <span>© {new Date().getFullYear()} Schedulify. All rights reserved.</span>
        </div>
        <a
          href={GITHUB_URL}
          target="_blank"
          rel="noopener noreferrer"
          className="hover:text-[var(--color-text-muted)] transition-colors"
        >
          GitHub
        </a>
      </div>
    </footer>
  );
}
