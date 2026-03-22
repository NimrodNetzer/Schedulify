import { useState } from 'react'

const SESSION_TYPES = [
  'LECTURE', 'TUTORIAL', 'LAB', 'BLOCK', 'DEPARTMENTAL_SESSION',
  'REINFORCEMENT', 'GUIDANCE', 'OPTIONAL_COLLOQUIUM', 'REGISTRATION', 'THESIS', 'PROJECT',
]
const DAYS = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday']
const FIELD_LABELS: Record<string, string> = {
  colCourseId: 'Course ID', colCourseName: 'Course Name', colTeacher: 'Teacher',
  colDay: 'Day', colStartTime: 'Start Time', colEndTime: 'End Time',
  colBuilding: 'Building', colRoom: 'Room', colSessionType: 'Session Type', colSemester: 'Semester (optional)',
}

interface Props {
  previewRows: string[][]     // first few rows including header
  onConfirm: (mapping: Record<string, unknown>) => void
  onCancel: () => void
}

export function ColumnMapperWizard({ previewRows, onConfirm, onCancel }: Props) {
  const headerRow = previewRows[0] ?? []
  const dataRows  = previewRows.slice(1)

  const [step, setStep] = useState<1 | 2 | 3>(1)
  const [colMap, setColMap] = useState<Record<string, number>>({
    colCourseId: -1, colCourseName: -1, colTeacher: -1,
    colDay: -1, colStartTime: -1, colEndTime: -1,
    colBuilding: -1, colRoom: -1, colSessionType: -1, colSemester: -1,
  })
  const [sessionTypeEntries, setSessionTypeEntries] = useState<Array<{ raw: string; mapped: string }>>([
    { raw: '', mapped: 'LECTURE' },
  ])
  const [dayEntries, setDayEntries] = useState<Array<{ raw: string; day: string }>>([
    { raw: '', day: 'Sunday' },
  ])
  const [profileName, setProfileName] = useState('')
  const [defaultSemester, setDefaultSemester] = useState(1)

  const setCol = (field: string, col: number) => setColMap(prev => ({ ...prev, [field]: col }))

  const buildMapping = () => {
    const sessionTypeMap: Record<string, string> = {}
    sessionTypeEntries.forEach(e => { if (e.raw) sessionTypeMap[e.raw] = e.mapped })
    const dayMap: Record<string, number> = {}
    dayEntries.forEach((e, i) => { if (e.raw) dayMap[e.raw] = DAYS.indexOf(e.day) + 1 })
    return {
      universityName: profileName,
      ...colMap,
      headerRow: 1,
      defaultSemester,
      sessionTypeMap,
      dayMap,
    }
  }

  return (
    <div className="fixed inset-0 bg-black/40 flex items-center justify-center z-50 p-4">
      <div className="bg-white rounded-2xl shadow-xl w-full max-w-3xl max-h-[90vh] flex flex-col">
        {/* Header */}
        <div className="px-6 py-4 border-b flex items-center justify-between">
          <h2 className="text-lg font-semibold text-gray-800">Column Mapping Wizard</h2>
          <div className="flex gap-2 text-sm text-gray-400">
            {([1,2,3] as const).map(s => (
              <span key={s} className={`px-2 py-0.5 rounded-full ${step === s ? 'bg-blue-600 text-white' : 'bg-gray-100'}`}>
                Step {s}
              </span>
            ))}
          </div>
        </div>

        <div className="flex-1 overflow-y-auto p-6">

          {/* Step 1: Preview + column mapping */}
          {step === 1 && (
            <div className="space-y-4">
              <p className="text-sm text-gray-500">Preview of your file. Assign each field to the correct column.</p>
              {/* Preview table */}
              <div className="overflow-x-auto border rounded-lg">
                <table className="text-xs w-full">
                  <thead>
                    <tr className="bg-gray-50">
                      <th className="px-2 py-1 text-gray-400 font-normal">#</th>
                      {headerRow.map((h, i) => (
                        <th key={i} className="px-2 py-1 text-left font-medium text-gray-700 border-l">{h || `Col ${i}`}</th>
                      ))}
                    </tr>
                  </thead>
                  <tbody>
                    {dataRows.map((row, ri) => (
                      <tr key={ri} className="border-t hover:bg-gray-50">
                        <td className="px-2 py-1 text-gray-400">{ri + 2}</td>
                        {headerRow.map((_, ci) => (
                          <td key={ci} className="px-2 py-1 border-l text-gray-700 max-w-[120px] truncate">{row[ci] ?? ''}</td>
                        ))}
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
              {/* Field → column selectors */}
              <div className="grid grid-cols-2 gap-3">
                {Object.entries(FIELD_LABELS).map(([field, label]) => (
                  <div key={field}>
                    <label className="block text-xs font-medium text-gray-600 mb-1">{label}</label>
                    <select
                      value={colMap[field] ?? -1}
                      onChange={e => setCol(field, parseInt(e.target.value))}
                      className="w-full border border-gray-300 rounded-lg px-2 py-1.5 text-sm focus:outline-none focus:border-blue-400"
                    >
                      <option value={-1}>— not mapped —</option>
                      {headerRow.map((h, i) => (
                        <option key={i} value={i}>{h || `Column ${i + 1}`}</option>
                      ))}
                    </select>
                  </div>
                ))}
                <div>
                  <label className="block text-xs font-medium text-gray-600 mb-1">Default Semester (when no column)</label>
                  <select value={defaultSemester} onChange={e => setDefaultSemester(parseInt(e.target.value))}
                    className="w-full border border-gray-300 rounded-lg px-2 py-1.5 text-sm focus:outline-none focus:border-blue-400">
                    <option value={1}>Semester A</option>
                    <option value={2}>Semester B</option>
                    <option value={3}>Summer</option>
                    <option value={4}>Yearly</option>
                  </select>
                </div>
              </div>
            </div>
          )}

          {/* Step 2: Session type + day mappings */}
          {step === 2 && (
            <div className="space-y-6">
              <div>
                <p className="text-sm font-medium text-gray-700 mb-2">Session Type Mapping</p>
                <p className="text-xs text-gray-400 mb-3">Map your university's session type names to the standard types.</p>
                {sessionTypeEntries.map((entry, i) => (
                  <div key={i} className="flex gap-2 mb-2">
                    <input placeholder="Your term (e.g. Lecture, שיעור)"
                      value={entry.raw}
                      onChange={e => setSessionTypeEntries(prev => prev.map((x, j) => j === i ? { ...x, raw: e.target.value } : x))}
                      className="flex-1 border border-gray-300 rounded-lg px-2 py-1.5 text-sm focus:outline-none focus:border-blue-400" />
                    <select value={entry.mapped}
                      onChange={e => setSessionTypeEntries(prev => prev.map((x, j) => j === i ? { ...x, mapped: e.target.value } : x))}
                      className="border border-gray-300 rounded-lg px-2 py-1.5 text-sm focus:outline-none focus:border-blue-400">
                      {SESSION_TYPES.map(t => <option key={t} value={t}>{t}</option>)}
                    </select>
                    <button onClick={() => setSessionTypeEntries(prev => prev.filter((_, j) => j !== i))}
                      className="text-red-400 hover:text-red-600 px-2">✕</button>
                  </div>
                ))}
                <button onClick={() => setSessionTypeEntries(prev => [...prev, { raw: '', mapped: 'LECTURE' }])}
                  className="text-sm text-blue-600 hover:underline">+ Add mapping</button>
              </div>

              <div>
                <p className="text-sm font-medium text-gray-700 mb-2">Day Mapping</p>
                <p className="text-xs text-gray-400 mb-3">Map your file's day values (e.g. "Mon", "ב'") to weekday names.</p>
                {dayEntries.map((entry, i) => (
                  <div key={i} className="flex gap-2 mb-2">
                    <input placeholder="Your day text (e.g. Mon, Sunday, Tuesday)"
                      value={entry.raw}
                      onChange={e => setDayEntries(prev => prev.map((x, j) => j === i ? { ...x, raw: e.target.value } : x))}
                      className="flex-1 border border-gray-300 rounded-lg px-2 py-1.5 text-sm focus:outline-none focus:border-blue-400" />
                    <select value={entry.day}
                      onChange={e => setDayEntries(prev => prev.map((x, j) => j === i ? { ...x, day: e.target.value } : x))}
                      className="border border-gray-300 rounded-lg px-2 py-1.5 text-sm focus:outline-none focus:border-blue-400">
                      {DAYS.map(d => <option key={d} value={d}>{d}</option>)}
                    </select>
                    <button onClick={() => setDayEntries(prev => prev.filter((_, j) => j !== i))}
                      className="text-red-400 hover:text-red-600 px-2">✕</button>
                  </div>
                ))}
                <button onClick={() => setDayEntries(prev => [...prev, { raw: '', day: 'Sunday' }])}
                  className="text-sm text-blue-600 hover:underline">+ Add mapping</button>
              </div>
            </div>
          )}

          {/* Step 3: Name & save */}
          {step === 3 && (
            <div className="space-y-4">
              <p className="text-sm text-gray-500">
                Give this mapping a name to save it as a university profile — next time you upload from the same university, it will be applied automatically.
              </p>
              <input
                placeholder="University name (e.g. Tel Aviv University)"
                value={profileName}
                onChange={e => setProfileName(e.target.value)}
                className="w-full border border-gray-300 rounded-lg px-3 py-2 text-sm focus:outline-none focus:border-blue-400"
              />
              <p className="text-xs text-gray-400">Leave blank to use the mapping once without saving.</p>
            </div>
          )}
        </div>

        {/* Footer */}
        <div className="px-6 py-4 border-t flex justify-between">
          <button onClick={onCancel} className="text-sm text-gray-500 hover:text-gray-700">Cancel</button>
          <div className="flex gap-2">
            {step > 1 && (
              <button onClick={() => setStep(s => (s - 1) as 1 | 2 | 3)}
                className="px-4 py-2 text-sm border border-gray-300 rounded-lg hover:bg-gray-50">Back</button>
            )}
            {step < 3 && (
              <button onClick={() => setStep(s => (s + 1) as 1 | 2 | 3)}
                className="px-4 py-2 text-sm bg-blue-600 text-white rounded-lg hover:bg-blue-700">Next</button>
            )}
            {step === 3 && (
              <button onClick={() => onConfirm(buildMapping())}
                className="px-4 py-2 text-sm bg-green-600 text-white rounded-lg hover:bg-green-700">
                Import Courses
              </button>
            )}
          </div>
        </div>
      </div>
    </div>
  )
}
