"use client";

import { motion } from "framer-motion";
import { Download, Github } from "lucide-react";

const GITHUB_URL = "https://github.com/Schedulify-org/Schedulify";
const RELEASES_URL = "https://github.com/Schedulify-org/Schedulify/releases";

export default function DownloadSection() {
  return (
    <section
      id="download"
      className="py-20 px-6 bg-[var(--color-charcoal)]"
    >
      <div className="max-w-3xl mx-auto text-center">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
          transition={{ duration: 0.5 }}
        >
          <h2 className="text-3xl font-bold text-white mb-4">
            Stop building schedules by hand.
          </h2>
          <p className="text-[#9ca3af] text-lg mb-3">
            Schedulify is free, open-source, and runs entirely on your machine.
          </p>
          <p className="text-[#6b7280] text-sm mb-10">
            No account. No subscription. Just download and run.
          </p>

          <div className="flex flex-col sm:flex-row items-center justify-center gap-4 mb-8">
            <a
              href={RELEASES_URL}
              target="_blank"
              rel="noopener noreferrer"
              className="inline-flex items-center gap-2 bg-[var(--color-primary)] text-white px-6 py-3 rounded-lg text-sm font-medium hover:bg-[var(--color-primary-dark)] transition-colors w-full sm:w-auto justify-center"
            >
              <Download className="w-4 h-4" />
              Download for Windows
            </a>
            <a
              href={RELEASES_URL}
              target="_blank"
              rel="noopener noreferrer"
              className="inline-flex items-center gap-2 border border-[#4b5563] text-white px-6 py-3 rounded-lg text-sm font-medium hover:border-[#6b7280] transition-colors w-full sm:w-auto justify-center"
            >
              <Download className="w-4 h-4" />
              Download for macOS
            </a>
          </div>

          <a
            href={GITHUB_URL}
            target="_blank"
            rel="noopener noreferrer"
            className="inline-flex items-center gap-2 text-[#9ca3af] text-sm hover:text-white transition-colors"
          >
            <Github className="w-4 h-4" />
            Open source — view on GitHub
          </a>
        </motion.div>
      </div>
    </section>
  );
}
