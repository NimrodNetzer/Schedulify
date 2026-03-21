"use client";

import { motion } from "framer-motion";

const steps = [
  {
    num: "01",
    title: "Upload Your Course List",
    description:
      "Drag and drop your Excel or TXT course file, or browse to select it. Previously uploaded files are saved for quick access.",
  },
  {
    num: "02",
    title: "Select Courses & Constraints",
    description:
      "Choose the courses you want, pick your preferences, and block any times you can't attend. Schedulify handles the rest.",
  },
  {
    num: "03",
    title: "Browse, Filter & Export",
    description:
      "Instantly browse every valid schedule. Use SchedBot to filter by natural language. Export as PNG or CSV when you're done.",
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
          {steps.map((step, i) => (
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
                <div className="text-5xl font-bold text-[var(--color-border)] mb-4 leading-none select-none">
                  {step.num}
                </div>
                <h3 className="font-semibold text-[var(--color-text-main)] text-lg mb-2">
                  {step.title}
                </h3>
                <p className="text-sm text-[var(--color-text-muted)] leading-relaxed">
                  {step.description}
                </p>
              </div>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  );
}
