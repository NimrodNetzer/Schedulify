"use client";

import { motion } from "framer-motion";
import { Download, Github } from "lucide-react";

const GITHUB_URL = "https://github.com/Schedulify-org/Schedulify";

const stats = [
  { value: "11", label: "session types" },
  { value: "A / B / Summer", label: "semesters" },
  { value: "100%", label: "conflict-free" },
];

export default function Hero() {
  return (
    <section className="pt-32 pb-20 px-6 bg-[var(--color-background)]">
      <div className="max-w-6xl mx-auto flex flex-col lg:flex-row items-center gap-16">
        {/* Left column */}
        <motion.div
          className="flex-1 text-center lg:text-left"
          initial={{ opacity: 0, y: 24 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5, ease: "easeOut" }}
        >
          {/* Badge */}
          <span className="inline-flex items-center gap-1.5 bg-blue-50 text-[var(--color-primary)] text-xs font-medium px-3 py-1 rounded-full border border-blue-100 mb-6">
            Free &amp; Open Source
          </span>

          <h1 className="text-4xl sm:text-5xl lg:text-[3.25rem] font-bold text-[var(--color-text-main)] leading-tight mb-5">
            Every valid schedule,{" "}
            <span className="text-[var(--color-primary)]">generated instantly.</span>
          </h1>

          <p className="text-lg text-[var(--color-text-muted)] leading-relaxed mb-8 max-w-xl mx-auto lg:mx-0">
            Upload your course list, block the times you can&apos;t attend, and
            Schedulify computes every conflict-free combination in seconds. Then
            filter results in plain English with SchedBot AI.
          </p>

          <div className="flex flex-col sm:flex-row items-center justify-center lg:justify-start gap-3 mb-8">
            <a
              href="#download"
              className="inline-flex items-center gap-2 bg-[var(--color-charcoal)] text-white px-6 py-3 rounded-lg text-sm font-medium hover:bg-[var(--color-charcoal-hover)] transition-colors w-full sm:w-auto justify-center"
            >
              <Download className="w-4 h-4" />
              Download Free
            </a>
            <a
              href={GITHUB_URL}
              target="_blank"
              rel="noopener noreferrer"
              className="inline-flex items-center gap-2 border border-[var(--color-border-strong)] text-[var(--color-text-main)] px-6 py-3 rounded-lg text-sm font-medium hover:bg-[var(--color-surface)] transition-colors w-full sm:w-auto justify-center"
            >
              <Github className="w-4 h-4" />
              View on GitHub
            </a>
          </div>

          {/* Stats bar */}
          <motion.div
            className="inline-flex flex-wrap justify-center lg:justify-start gap-6"
            initial={{ opacity: 0, y: 8 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.5, ease: "easeOut", delay: 0.25 }}
          >
            {stats.map((s, i) => (
              <div key={i} className="flex flex-col items-center lg:items-start">
                <span className="text-xl font-bold text-[var(--color-text-main)]">
                  {s.value}
                </span>
                <span className="text-xs text-[var(--color-text-faint)]">
                  {s.label}
                </span>
              </div>
            ))}
          </motion.div>

          <p className="mt-5 text-xs text-[var(--color-text-faint)]">
            Available for Windows and macOS
          </p>
        </motion.div>

        {/* Right column — mock schedule UI */}
        <motion.div
          className="flex-1 w-full max-w-lg lg:max-w-none"
          initial={{ opacity: 0, x: 24 }}
          animate={{ opacity: 1, x: 0 }}
          transition={{ duration: 0.5, ease: "easeOut", delay: 0.15 }}
        >
          <div className="rounded-xl border border-[var(--color-border)] shadow-xl overflow-hidden bg-[var(--color-surface)]">
            {/* Window chrome */}
            <div className="bg-[var(--color-charcoal)] px-4 py-3 flex items-center gap-2">
              <div className="w-3 h-3 rounded-full bg-red-400" />
              <div className="w-3 h-3 rounded-full bg-yellow-400" />
              <div className="w-3 h-3 rounded-full bg-green-400" />
              <span className="ml-3 text-[#9ca3af] text-xs font-medium">
                Schedulify — Schedule 1 of 42
              </span>
            </div>

            {/* Schedule grid */}
            <ScheduleMockup />
          </div>
        </motion.div>
      </div>
    </section>
  );
}

function ScheduleMockup() {
  const days = ["Sun", "Mon", "Tue", "Wed", "Thu"];
  const hours = ["8:00", "10:00", "12:00", "14:00", "16:00", "18:00"];

  const blocks: {
    day: number;
    hour: number;
    span: number;
    label: string;
    type: string;
  }[] = [
    { day: 0, hour: 0, span: 2, label: "Calculus\nLecture", type: "lecture" },
    { day: 1, hour: 1, span: 2, label: "Data Structures\nLecture", type: "lecture" },
    { day: 1, hour: 4, span: 1, label: "DS Tutorial", type: "tutorial" },
    { day: 2, hour: 0, span: 1, label: "Calculus\nTutorial", type: "tutorial" },
    { day: 2, hour: 2, span: 1, label: "Linear Algebra\nLecture", type: "lecture" },
    { day: 3, hour: 1, span: 2, label: "Algorithms\nLecture", type: "lecture" },
    { day: 3, hour: 4, span: 1, label: "Algo Lab", type: "lab" },
    { day: 4, hour: 0, span: 2, label: "Linear Algebra\nTutorial", type: "tutorial" },
  ];

  const colors: Record<string, string> = {
    lecture: "bg-blue-100 border-blue-300 text-blue-800",
    tutorial: "bg-green-100 border-green-300 text-green-800",
    lab: "bg-purple-100 border-purple-300 text-purple-800",
  };

  return (
    <div className="p-4 overflow-x-auto">
      <div
        className="grid min-w-[380px]"
        style={{
          gridTemplateColumns: `3rem repeat(${days.length}, 1fr)`,
          gridTemplateRows: `2rem repeat(${hours.length}, 3rem)`,
          gap: "2px",
        }}
      >
        {/* Header row */}
        <div />
        {days.map((d) => (
          <div
            key={d}
            className="flex items-center justify-center text-xs font-semibold text-[var(--color-text-muted)]"
          >
            {d}
          </div>
        ))}

        {/* Hour labels + empty cells */}
        {hours.map((h, hi) => (
          <>
            <div
              key={`h-${hi}`}
              className="flex items-center justify-end pr-2 text-[10px] text-[var(--color-text-faint)]"
            >
              {h}
            </div>
            {days.map((_, di) => (
              <div
                key={`cell-${hi}-${di}`}
                className="rounded border border-[var(--color-border)] bg-[#f9fcff]"
              />
            ))}
          </>
        ))}

        {/* Session blocks */}
        {blocks.map((b, i) => (
          <div
            key={i}
            className={`rounded border text-[9px] font-medium p-1 leading-tight flex items-center justify-center text-center whitespace-pre-line ${colors[b.type]}`}
            style={{
              gridColumn: b.day + 2,
              gridRow: `${b.hour + 2} / span ${b.span}`,
            }}
          >
            {b.label}
          </div>
        ))}
      </div>
    </div>
  );
}
