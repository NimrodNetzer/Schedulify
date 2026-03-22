import type { Schedule } from '../../types/models'

const DAYS = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday']
const COLORS = [
  'bg-blue-100 border-blue-400 text-blue-900',
  'bg-green-100 border-green-400 text-green-900',
  'bg-purple-100 border-purple-400 text-purple-900',
  'bg-orange-100 border-orange-400 text-orange-900',
  'bg-pink-100 border-pink-400 text-pink-900',
  'bg-teal-100 border-teal-400 text-teal-900',
  'bg-yellow-100 border-yellow-400 text-yellow-900',
  'bg-red-100 border-red-400 text-red-900',
]

function hashColor(name: string) {
  let h = 0
  for (let i = 0; i < name.length; i++) h = (h * 31 + name.charCodeAt(i)) & 0xffff
  return COLORS[h % COLORS.length]
}

function timeToMinutes(t: string) {
  const [h, m] = t.split(':').map(Number)
  return h * 60 + (m || 0)
}

interface Props { schedule: Schedule }

export function WeekGrid({ schedule }: Props) {
  const startMin = schedule.earliest_start * 60
  const endMin = schedule.latest_end * 60
  const totalMin = Math.max(endMin - startMin, 60)

  // Build time labels (every 60 min)
  const timeLabels: string[] = []
  for (let m = startMin; m <= endMin; m += 60) {
    timeLabels.push(`${Math.floor(m / 60).toString().padStart(2, '0')}:00`)
  }

  return (
    <div className="overflow-x-auto">
      <div className="min-w-[600px]">
        {/* Header */}
        <div className="flex" style={{ marginLeft: 48 }}>
          {schedule.week.map((day, i) => (
            day.day_items.length > 0 && (
              <div key={i} className="flex-1 text-center text-sm font-semibold text-gray-700 py-2 border-b border-gray-200">
                {DAYS[i] ?? day.day}
              </div>
            )
          ))}
        </div>

        {/* Grid body */}
        <div className="flex">
          {/* Time labels */}
          <div className="w-12 flex-shrink-0">
            {timeLabels.map(label => (
              <div key={label} className="text-xs text-gray-400 text-right pr-1" style={{ height: 60 }}>
                {label}
              </div>
            ))}
          </div>

          {/* Day columns */}
          {schedule.week.map((day, dayIdx) => {
            if (day.day_items.length === 0) return null
            return (
              <div key={dayIdx} className="flex-1 relative border-l border-gray-100" style={{ height: totalMin }}>
                {/* Hour lines */}
                {timeLabels.map((_, i) => (
                  <div key={i} className="absolute w-full border-t border-gray-100" style={{ top: i * 60 }} />
                ))}
                {/* Items */}
                {day.day_items.map((item, itemIdx) => {
                  const itemStart = timeToMinutes(item.start) - startMin
                  const itemEnd = timeToMinutes(item.end) - startMin
                  const height = Math.max(itemEnd - itemStart, 20)
                  return (
                    <div
                      key={itemIdx}
                      className={`absolute left-0.5 right-0.5 rounded border text-xs p-1 overflow-hidden ${hashColor(item.courseName)}`}
                      style={{ top: itemStart, height }}
                      title={`${item.courseName} | ${item.type} | ${item.start}-${item.end} | Bldg ${item.building} Rm ${item.room}`}
                    >
                      <div className="font-semibold truncate">{item.courseName}</div>
                      <div className="truncate text-[10px]">{item.type}</div>
                      <div className="truncate text-[10px]">{item.start}&ndash;{item.end}</div>
                    </div>
                  )
                })}
              </div>
            )
          })}
        </div>
      </div>
    </div>
  )
}
