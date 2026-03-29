"use client";

import { motion } from "framer-motion";
import { Upload, SlidersHorizontal, CheckCircle } from "lucide-react";

const steps = [
  {
    num: "01",
    Icon: Upload,
    title: "Upload Your Course List",
    description:
      "Drop in your Excel (.xlsx) or TXT course file. Schedulify parses all session types automatically and remembers previously uploaded files for quick re-use.",
  },
  {
    num: "02",
    Icon: SlidersHorizontal,
    title: "Select Courses & Block Times",
    description:
      "Pick the courses you need, choose your semester, and mark any time slots you cannot attend. The generator only considers schedules that fit your constraints.",
  },
  {
    num: "03",
    Icon: CheckCircle,
    title: "Browse, Filter & Export",
    description:
      "Every valid schedule appears instantly. Ask SchedBot in plain English to narrow them down, then export to CSV or print your favourite — done.",
  },
];

export default function HowItWorks() {
  return (
    <section id="how-it-works" className="py-20 px-6 bg-[var(--color-background)]">
      <div className="max-w-6xl mx-auto">
        <div className="text-center mb-14">
          <h2 className="text-3xl font-bold text-[var(--color-text-main)] mb-3">
            Three steps to your perfect schedule
          </h2>
          <p className="text-[var(--color-text-muted)] text-lg">
            From course list to calendar-ready schedule in minutes.
          </p>
        </div>

        <div className="grid md:grid-cols-3 gap-8">
          {steps.map((step, i) => {
            const Icon = step.Icon;
            return (
              <motion.div
                key={step.num}
                initial={{ opacity: 0, y: 20 }}
                whileInView={{ opacity: 1, y: 0 }}
                viewport={{ once: true }}
                transition={{ duration: 0.4, delay: i * 0.1 }}
                className="relative"
              >
                {/* Connector line */}
                {i < steps.length - 1 && (
                  <div className="hidden md:block absolute top-8 left-full w-full h-px bg-[var(--color-border)] z-0 -translate-x-4" />
                )}

                <div className="relative z-10 bg-[var(--color-surface)] border border-[var(--color-border)] rounded-xl p-6">
                  {/* Step icon */}
                  <div className="flex items-center gap-3 mb-4">
                    <div className="w-10 h-10 rounded-lg bg-blue-50 flex items-center justify-center shrink-0">
                      <Icon className="w-5 h-5 text-[var(--color-primary)]" />
                    </div>
                    <span className="text-3xl font-bold text-[var(--color-border)] leading-none select-none">
                      {step.num}
                    </span>
                  </div>
                  <h3 className="font-semibold text-[var(--color-text-main)] text-lg mb-2">
                    {step.title}
                  </h3>
                  <p className="text-sm text-[var(--color-text-muted)] leading-relaxed">
                    {step.description}
                  </p>
                </div>
              </motion.div>
            );
          })}
        </div>
      </div>
    </section>
  );
}
