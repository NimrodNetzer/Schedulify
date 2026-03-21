"use client";

import { motion } from "framer-motion";

const techs = [
  { label: "C++17", color: "bg-blue-50 border-blue-200 text-blue-700" },
  { label: "Qt 6", color: "bg-green-50 border-green-200 text-green-700" },
  { label: "SQLite", color: "bg-slate-50 border-slate-200 text-slate-700" },
  { label: "Claude API", color: "bg-orange-50 border-orange-200 text-orange-700" },
  { label: "OpenXLSX", color: "bg-emerald-50 border-emerald-200 text-emerald-700" },
  { label: "CMake", color: "bg-red-50 border-red-200 text-red-700" },
];

export default function TechStack() {
  return (
    <section id="tech" className="py-16 px-6 bg-[var(--color-surface)] border-t border-[var(--color-border)]">
      <div className="max-w-6xl mx-auto text-center">
        <p className="text-sm font-medium text-[var(--color-text-faint)] uppercase tracking-widest mb-8">
          Built with
        </p>
        <motion.div
          className="flex flex-wrap justify-center gap-3"
          initial={{ opacity: 0 }}
          whileInView={{ opacity: 1 }}
          viewport={{ once: true }}
          transition={{ duration: 0.5 }}
        >
          {techs.map((t) => (
            <span
              key={t.label}
              className={`inline-flex items-center px-4 py-2 rounded-full border text-sm font-medium ${t.color}`}
            >
              {t.label}
            </span>
          ))}
        </motion.div>
      </div>
    </section>
  );
}
