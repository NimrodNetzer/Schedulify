import type { Schedule } from '../../types/models'

interface Props {
  schedule: Schedule
  index: number
  total: number
  onPrev: () => void
  onNext: () => void
  onExportCsv: () => void
  onExportIcal: () => void
}

export function ScheduleCard({ schedule, index, total, onPrev, onNext, onExportCsv, onExportIcal }: Props) {
  return (
    <div className="bg-white rounded-xl shadow p-4">
      <div className="flex items-center justify-between mb-3">
        <button onClick={onPrev} disabled={index === 0} className="px-3 py-1 rounded bg-gray-100 hover:bg-gray-200 disabled:opacity-40">&#8249;</button>
        <span className="font-semibold text-gray-700">{index + 1} / {total}</span>
        <button onClick={onNext} disabled={index === total - 1} className="px-3 py-1 rounded bg-gray-100 hover:bg-gray-200 disabled:opacity-40">&#8250;</button>
      </div>
      <div className="grid grid-cols-3 gap-2 text-sm text-gray-600 mb-3">
        <div className="bg-gray-50 rounded p-2 text-center">
          <div className="font-bold text-gray-900">{schedule.amount_days}</div>
          <div>Days</div>
        </div>
        <div className="bg-gray-50 rounded p-2 text-center">
          <div className="font-bold text-gray-900">{schedule.amount_gaps}</div>
          <div>Gaps</div>
        </div>
        <div className="bg-gray-50 rounded p-2 text-center">
          <div className="font-bold text-gray-900">{schedule.total_class_time}h</div>
          <div>Hours</div>
        </div>
      </div>
      <div className="flex gap-2">
        <button onClick={onExportCsv} className="flex-1 bg-blue-600 text-white rounded-lg py-2 hover:bg-blue-700 text-sm font-medium">
          Export CSV
        </button>
        <button onClick={onExportIcal} className="flex-1 bg-green-600 text-white rounded-lg py-2 hover:bg-green-700 text-sm font-medium">
          Add to Calendar
        </button>
      </div>
    </div>
  )
}
