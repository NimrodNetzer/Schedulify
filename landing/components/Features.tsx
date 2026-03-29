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
      "Computes every valid schedule combination exhaustively — zero time overlaps, guaranteed. No manual checking, no missed options.",
  },
  {
    icon: MessageSquare,
    title: "SchedBot AI Filtering",
    description:
      "Chat with SchedBot, powered by Gemini Flash AI. Ask for schedules with no morning classes, maximum free days, or any preference you can describe in plain English.",
  },
  {
    icon: Upload,
    title: "Flexible File Input",
    description:
      "Import your course list via Excel (.xlsx) or TXT. Supports all 11 session types: Lectures, Tutorials, Labs, Blocks, Departmental, Reinforcement, Guidance, Colloquium, Registration, Thesis, and Project.",
  },
  {
    icon: Clock,
    title: "Block Unavailable Times",
    description:
      "Mark any time slots you cannot attend before generation starts. The algorithm only ever considers schedules that fit your life.",
  },
  {
    icon: FileOutput,
    title: "Print-Ready Export",
    description:
      "Export your final schedule to CSV for spreadsheet workflows, or print it directly from the app with a single click — clean and formatted.",
  },
  {
    icon: CalendarDays,
    title: "Multi-Semester Support",
    description:
      "Handles Semester A, Semester B, Summer, and Yearly courses in one session. Switch between semesters and filter cleanly without re-importing.",
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
            No spreadsheets. No conflicts. No wasted time.
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
