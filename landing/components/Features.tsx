"use client";

import { motion } from "framer-motion";
import {
  Zap,
  MessageSquare,
  Upload,
  Clock,
  FileOutput,
  CalendarDays,
} from "lucide-react";

const features = [
  {
    icon: Zap,
    title: "Conflict-Free Generation",
    description:
      "Automatically generates every valid schedule combination with zero time conflicts — no manual checking required.",
  },
  {
    icon: MessageSquare,
    title: "SchedBot AI Filtering",
    description:
      "Chat with SchedBot, powered by Claude AI. Ask for schedules with no morning classes, maximum free days, or any preference you can describe.",
  },
  {
    icon: Upload,
    title: "Flexible File Input",
    description:
      "Upload your course list as Excel or TXT. Supports all major session types: lectures, tutorials, labs, and more.",
  },
  {
    icon: Clock,
    title: "Block Unavailable Times",
    description:
      "Mark time slots you cannot attend — the generator respects your constraints before it even starts computing.",
  },
  {
    icon: FileOutput,
    title: "Export Anywhere",
    description:
      "Save your schedule as a PNG, export to CSV, or print it directly from the app. Your schedule, your format.",
  },
  {
    icon: CalendarDays,
    title: "Multi-Semester Support",
    description:
      "Manage Semester A, Semester B, Summer, and Yearly courses all in one session, with clean filtering between them.",
  },
];

export default function Features() {
  return (
    <section id="features" className="py-20 px-6 bg-[var(--color-surface)]">
      <div className="max-w-6xl mx-auto">
        <div className="text-center mb-14">
          <h2 className="text-3xl font-bold text-[var(--color-text-main)] mb-3">
            Everything you need to build the right schedule
          </h2>
          <p className="text-[var(--color-text-muted)] text-lg max-w-xl mx-auto">
            No more spreadsheets. No more conflicts.
          </p>
        </div>

        <div className="grid sm:grid-cols-2 lg:grid-cols-3 gap-6">
          {features.map((f, i) => {
            const Icon = f.icon;
            return (
              <motion.div
                key={f.title}
                initial={{ opacity: 0, y: 20 }}
                whileInView={{ opacity: 1, y: 0 }}
                viewport={{ once: true }}
                transition={{ duration: 0.4, delay: i * 0.06 }}
                className="bg-[var(--color-background)] border border-[var(--color-border)] rounded-xl p-6 hover:shadow-md transition-shadow"
              >
                <div className="w-10 h-10 rounded-lg bg-blue-50 flex items-center justify-center mb-4">
                  <Icon className="w-5 h-5 text-[var(--color-primary)]" />
                </div>
                <h3 className="font-semibold text-[var(--color-text-main)] mb-2">
                  {f.title}
                </h3>
                <p className="text-sm text-[var(--color-text-muted)] leading-relaxed">
                  {f.description}
                </p>
              </motion.div>
            );
          })}
        </div>
      </div>
    </section>
  );
}
