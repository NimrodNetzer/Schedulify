import { useState, useRef, useEffect } from 'react'
import { botApi } from '../../api/api'
import { useChatStore } from '../../stores/useChatStore'
import { useSchedulesStore } from '../../stores/useSchedulesStore'

export function ChatPanel() {
  const [input, setInput] = useState('')
  const { messages, isLoading, addMessage, setLoading } = useChatStore()
  const { displaySchedules, applyFilter, currentSemester } = useSchedulesStore()
  const bottomRef = useRef<HTMLDivElement>(null)

  useEffect(() => { bottomRef.current?.scrollIntoView({ behavior: 'smooth' }) }, [messages])

  const send = async () => {
    if (!input.trim() || isLoading) return
    const text = input.trim()
    setInput('')
    addMessage({ role: 'user', text })
    setLoading(true)
    try {
      const schedules = displaySchedules()
      const uniqueIds = schedules.map(s => s.unique_id)
      const result = await botApi.query(text, uniqueIds, currentSemester)
      addMessage({
        role: 'bot',
        text: result.response,
        isFilter: result.isFilter,
        filteredCount: result.filteredUniqueIds.length,
      })
      if (result.isFilter && result.filteredUniqueIds.length > 0) {
        applyFilter(result.filteredUniqueIds)
      }
    } catch {
      addMessage({ role: 'bot', text: 'Error connecting to SchedBot. Please try again.' })
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="flex flex-col h-full bg-white rounded-xl shadow">
      <div className="p-3 border-b font-semibold text-gray-700 flex items-center gap-2">
        <span>&#129302;</span> SchedBot
      </div>
      <div className="flex-1 overflow-y-auto p-3 space-y-2 min-h-0">
        {messages.length === 0 && (
          <p className="text-gray-400 text-sm text-center mt-4">Ask me to filter your schedules!</p>
        )}
        {messages.map((msg, i) => (
          <div key={i} className={`flex ${msg.role === 'user' ? 'justify-end' : 'justify-start'}`}>
            <div className={`max-w-[85%] rounded-xl px-3 py-2 text-sm ${
              msg.role === 'user' ? 'bg-blue-600 text-white' : 'bg-gray-100 text-gray-800'
            }`}>
              {msg.text}
              {msg.isFilter && msg.filteredCount !== undefined && (
                <div className="mt-1 text-xs bg-blue-100 text-blue-700 rounded px-2 py-0.5 inline-block">
                  Filtered: {msg.filteredCount} schedules
                </div>
              )}
            </div>
          </div>
        ))}
        {isLoading && (
          <div className="flex justify-start">
            <div className="bg-gray-100 rounded-xl px-3 py-2 text-sm text-gray-500 animate-pulse">Thinking...</div>
          </div>
        )}
        <div ref={bottomRef} />
      </div>
      <div className="p-3 border-t flex gap-2">
        <input
          value={input}
          onChange={e => setInput(e.target.value)}
          onKeyDown={e => e.key === 'Enter' && send()}
          placeholder="e.g. No gaps, starts after 9am..."
          className="flex-1 border rounded-lg px-3 py-2 text-sm focus:outline-none focus:ring-2 focus:ring-blue-400"
        />
        <button onClick={send} disabled={isLoading} className="bg-blue-600 text-white px-4 py-2 rounded-lg text-sm font-medium hover:bg-blue-700 disabled:opacity-50">
          Send
        </button>
      </div>
    </div>
  )
}
