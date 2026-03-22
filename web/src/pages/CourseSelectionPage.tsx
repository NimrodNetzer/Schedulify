import { useState } from 'react'
import { useNavigate } from 'react-router-dom'
import { schedulesApi } from '../api/api'
import { useCoursesStore } from '../stores/useCoursesStore'
import { useSchedulesStore } from '../stores/useSchedulesStore'
import { LoadingOverlay } from '../components/common/LoadingOverlay'
import { ErrorBanner } from '../components/common/ErrorBanner'
import type { Course } from '../types/models'

const SEMESTER_NAMES: Record<number, string> = { 1: 'Semester A', 2: 'Semester B', 3: 'Summer', 4: 'Yearly' }

function CourseCard({ course, selected, onToggle }: { course: Course; selected: boolean; onToggle: () => void }) {
  return (
    <div onClick={onToggle} className={`p-3 rounded-lg border cursor-pointer transition-all ${selected ? 'border-blue-500 bg-blue-50' : 'border-gray-200 hover:border-gray-300 bg-white'}`}>
      <div className="flex items-start gap-3">
        <input type="checkbox" checked={selected} onChange={onToggle} className="mt-1 rounded" onClick={e => e.stopPropagation()} />
        <div className="min-w-0">
          <p className="font-medium text-gray-800 truncate">{course.name}</p>
          <p className="text-xs text-gray-400">{course.raw_id} &middot; {course.teacher} &middot; {SEMESTER_NAMES[course.semester]}</p>
        </div>
      </div>
    </div>
  )
}

export function CourseSelectionPage() {
  const navigate = useNavigate()
  const { availableCourses, selectedCourses, toggleCourse } = useCoursesStore()
  const { setSchedules, setGenerating } = useSchedulesStore()
  const [search, setSearch] = useState('')
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState<string | null>(null)

  const filtered = availableCourses.filter(c =>
    c.name.toLowerCase().includes(search.toLowerCase()) ||
    c.raw_id.toLowerCase().includes(search.toLowerCase()) ||
    c.teacher.toLowerCase().includes(search.toLowerCase())
  )

  const isSelected = (c: Course) => selectedCourses.some(s => s.raw_id === c.raw_id && s.semester === c.semester)

  const handleGenerate = async () => {
    if (selectedCourses.length === 0) return
    setLoading(true)
    setGenerating(true)
    setError(null)
    try {
      const [a, b, summer] = await Promise.all([
        schedulesApi.generate(selectedCourses, 'A'),
        schedulesApi.generate(selectedCourses, 'B'),
        schedulesApi.generate(selectedCourses, 'SUMMER'),
      ])
      setSchedules('A', a)
      setSchedules('B', b)
      setSchedules('SUMMER', summer)
      navigate('/schedules')
    } catch (e: unknown) {
      const err = e as { response?: { data?: { error?: string } } }
      setError(err?.response?.data?.error || 'Failed to generate schedules.')
    } finally {
      setLoading(false)
      setGenerating(false)
    }
  }

  return (
    <div className="min-h-screen bg-gray-50 flex flex-col">
      {loading && <LoadingOverlay message="Generating schedules... this may take a moment." />}
      <div className="bg-white border-b px-6 py-4 flex items-center justify-between">
        <div>
          <h1 className="text-xl font-bold text-gray-800">Select Courses</h1>
          <p className="text-sm text-gray-500">{selectedCourses.length} selected &middot; {availableCourses.length} available</p>
        </div>
        <div className="flex gap-3">
          <button onClick={() => navigate('/')} className="px-4 py-2 rounded-lg border text-gray-600 hover:bg-gray-50 text-sm">&#8592; Back</button>
          <button onClick={handleGenerate} disabled={selectedCourses.length === 0}
            className="px-4 py-2 rounded-lg bg-blue-600 text-white font-medium hover:bg-blue-700 disabled:opacity-40 text-sm">
            Generate Schedules &#8594;
          </button>
        </div>
      </div>

      <div className="flex-1 flex overflow-hidden">
        {/* Course list */}
        <div className="flex-1 p-6 overflow-y-auto">
          {error && <div className="mb-4"><ErrorBanner message={error} onClose={() => setError(null)} /></div>}
          <input value={search} onChange={e => setSearch(e.target.value)}
            placeholder="Search courses..." className="w-full border rounded-lg px-4 py-2 mb-4 focus:outline-none focus:ring-2 focus:ring-blue-400 text-sm" />
          <div className="space-y-2">
            {filtered.map((c, i) => <CourseCard key={i} course={c} selected={isSelected(c)} onToggle={() => toggleCourse(c)} />)}
            {filtered.length === 0 && <p className="text-gray-400 text-center py-8">No courses found.</p>}
          </div>
        </div>

        {/* Selected panel */}
        {selectedCourses.length > 0 && (
          <div className="w-64 border-l bg-white p-4 overflow-y-auto">
            <h2 className="font-semibold text-gray-700 mb-3">Selected</h2>
            <div className="space-y-2">
              {selectedCourses.map((c, i) => (
                <div key={i} className="flex items-center gap-2 text-sm">
                  <button onClick={() => toggleCourse(c)} className="text-red-400 hover:text-red-600">&#x2715;</button>
                  <span className="truncate">{c.name}</span>
                </div>
              ))}
            </div>
          </div>
        )}
      </div>
    </div>
  )
}
