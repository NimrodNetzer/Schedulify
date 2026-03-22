import { create } from 'zustand'
import type { Course } from '../types/models'

interface CoursesStore {
  availableCourses: Course[]
  selectedCourses: Course[]
  setAvailableCourses: (courses: Course[]) => void
  toggleCourse: (course: Course) => void
  clearSelected: () => void
}

export const useCoursesStore = create<CoursesStore>((set, get) => ({
  availableCourses: [],
  selectedCourses: [],
  setAvailableCourses: (courses) => set({ availableCourses: courses }),
  toggleCourse: (course) => {
    const { selectedCourses } = get()
    const exists = selectedCourses.some(c => c.raw_id === course.raw_id && c.semester === course.semester)
    set({
      selectedCourses: exists
        ? selectedCourses.filter(c => !(c.raw_id === course.raw_id && c.semester === course.semester))
        : [...selectedCourses, course]
    })
  },
  clearSelected: () => set({ selectedCourses: [] }),
}))
