import { create } from 'zustand'
import type { FileRecord } from '../types/models'

interface FilesStore {
  files: FileRecord[]
  setFiles: (files: FileRecord[]) => void
  removeFile: (id: number) => void
}

export const useFilesStore = create<FilesStore>((set) => ({
  files: [],
  setFiles: (files) => set({ files }),
  removeFile: (id) => set(s => ({ files: s.files.filter(f => f.id !== id) })),
}))
