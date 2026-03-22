import { create } from 'zustand'
import type { Schedule } from '../types/models'

type Semester = 'A' | 'B' | 'SUMMER'

interface SchedulesStore {
  schedules: Record<Semester, Schedule[]>
  currentSemester: Semester
  currentIndex: number
  filteredUniqueIds: string[] | null
  isGenerating: boolean
  setSchedules: (semester: Semester, schedules: Schedule[]) => void
  setSemester: (sem: Semester) => void
  setIndex: (i: number) => void
  applyFilter: (uniqueIds: string[]) => void
  clearFilter: () => void
  setGenerating: (v: boolean) => void
  currentSchedules: () => Schedule[]
  displaySchedules: () => Schedule[]
}

export const useSchedulesStore = create<SchedulesStore>((set, get) => ({
  schedules: { A: [], B: [], SUMMER: [] },
  currentSemester: 'A',
  currentIndex: 0,
  filteredUniqueIds: null,
  isGenerating: false,
  setSchedules: (semester, schedules) =>
    set(s => ({ schedules: { ...s.schedules, [semester]: schedules }, currentIndex: 0 })),
  setSemester: (sem) => set({ currentSemester: sem, currentIndex: 0, filteredUniqueIds: null }),
  setIndex: (i) => set({ currentIndex: i }),
  applyFilter: (uniqueIds) => set({ filteredUniqueIds: uniqueIds, currentIndex: 0 }),
  clearFilter: () => set({ filteredUniqueIds: null, currentIndex: 0 }),
  setGenerating: (v) => set({ isGenerating: v }),
  currentSchedules: () => {
    const { schedules, currentSemester } = get()
    return schedules[currentSemester]
  },
  displaySchedules: () => {
    const { filteredUniqueIds, currentSemester, schedules } = get()
    const all = schedules[currentSemester]
    if (!filteredUniqueIds) return all
    return all.filter(s => filteredUniqueIds.includes(s.unique_id))
  },
}))
