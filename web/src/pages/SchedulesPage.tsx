import { useState } from 'react'
import { useNavigate } from 'react-router-dom'
import { schedulesApi } from '../api/api'
import { useSchedulesStore } from '../stores/useSchedulesStore'
import { useChatStore } from '../stores/useChatStore'
import { WeekGrid } from '../components/schedule/WeekGrid'
import { ScheduleCard } from '../components/schedule/ScheduleCard'
import { ChatPanel } from '../components/chat/ChatPanel'

type Semester = 'A' | 'B' | 'SUMMER'
const SEMESTER_LABELS: Record<Semester, string> = { A: 'Semester A', B: 'Semester B', SUMMER: 'Summer' }

export function SchedulesPage() {
  const navigate = useNavigate()
  const { schedules, currentSemester, currentIndex, filteredUniqueIds, setSemester, setIndex, clearFilter, displaySchedules } = useSchedulesStore()
  const { clearMessages } = useChatStore()
  const [chatOpen, setChatOpen] = useState(false)

  const shown = displaySchedules()
  const current = shown[currentIndex]
  const semesters: Semester[] = ['A', 'B', 'SUMMER']

  const handleExportCsv = async () => {
    if (!current) return
    try { await schedulesApi.exportCsv(current) } catch { /* ignore */ }
  }

  const handleExportIcal = async () => {
    if (!current) return
    try { await schedulesApi.exportIcal(current) } catch { /* ignore */ }
  }

  return (
    <div className="min-h-screen bg-gray-50 flex flex-col">
      {/* Top bar */}
      <div className="bg-white border-b px-6 py-3 flex items-center gap-4">
        <button onClick={() => navigate('/courses')} className="text-gray-500 hover:text-gray-700 text-sm">&#8592; Back</button>
        <div className="flex gap-1">
          {semesters.map(sem => (
            <button key={sem} onClick={() => setSemester(sem)}
              className={`px-3 py-1.5 rounded-lg text-sm font-medium transition-colors ${currentSemester === sem ? 'bg-blue-600 text-white' : 'bg-gray-100 text-gray-600 hover:bg-gray-200'}`}>
              {SEMESTER_LABELS[sem]} ({schedules[sem].length})
            </button>
          ))}
        </div>
        {filteredUniqueIds && (
          <button onClick={clearFilter} className="text-sm text-blue-600 border border-blue-300 rounded-lg px-3 py-1 hover:bg-blue-50">
            Clear Filter ({shown.length} shown)
          </button>
        )}
        <div className="ml-auto">
          <button onClick={() => { setChatOpen(!chatOpen); if (!chatOpen) clearMessages() }}
            className={`px-4 py-1.5 rounded-lg text-sm font-medium border transition-colors ${chatOpen ? 'bg-blue-600 text-white border-blue-600' : 'border-gray-300 text-gray-600 hover:bg-gray-50'}`}>
            &#129302; SchedBot
          </button>
        </div>
      </div>

      {shown.length === 0 ? (
        <div className="flex-1 flex items-center justify-center text-gray-400">
          No schedules for this semester.
        </div>
      ) : (
        <div className="flex-1 flex overflow-hidden">
          {/* Main schedule view */}
          <div className="flex-1 p-6 overflow-y-auto">
            {current && (
              <>
                <div className="mb-4">
                  <ScheduleCard schedule={current} index={currentIndex} total={shown.length}
                    onPrev={() => setIndex(currentIndex - 1)}
                    onNext={() => setIndex(currentIndex + 1)}
                    onExportCsv={handleExportCsv}
                    onExportIcal={handleExportIcal}
                  />
                </div>
                <div className="bg-white rounded-xl shadow p-4">
                  <WeekGrid schedule={current} />
                </div>
              </>
            )}
          </div>

          {/* Chat panel */}
          {chatOpen && (
            <div className="w-80 border-l flex flex-col p-4">
              <div className="flex-1 min-h-0">
                <ChatPanel />
              </div>
            </div>
          )}
        </div>
      )}
    </div>
  )
}
