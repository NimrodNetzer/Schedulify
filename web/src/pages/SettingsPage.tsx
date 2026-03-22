import { useState, useEffect } from 'react'
import { useNavigate } from 'react-router-dom'
import { settingsApi } from '../api/api'
import { ErrorBanner } from '../components/common/ErrorBanner'

export function SettingsPage() {
  const navigate = useNavigate()
  const [apiKey, setApiKey] = useState('')
  const [saved, setSaved] = useState(false)
  const [error, setError] = useState<string | null>(null)

  useEffect(() => {
    settingsApi.get('gemini_api_key').then(setApiKey).catch(() => {})
  }, [])

  const handleSave = async () => {
    try {
      await settingsApi.set('gemini_api_key', apiKey)
      setSaved(true)
      setTimeout(() => setSaved(false), 2000)
    } catch {
      setError('Failed to save settings.')
    }
  }

  return (
    <div className="min-h-screen bg-gray-50 p-6">
      <div className="max-w-lg mx-auto">
        <div className="flex items-center gap-4 mb-6">
          <button onClick={() => navigate(-1)} className="text-gray-500 hover:text-gray-700">&#8592; Back</button>
          <h1 className="text-xl font-bold text-gray-800">Settings</h1>
        </div>
        <div className="bg-white rounded-xl shadow p-6">
          {error && <div className="mb-4"><ErrorBanner message={error} onClose={() => setError(null)} /></div>}
          <label className="block text-sm font-medium text-gray-700 mb-1">Gemini API Key</label>
          <input type="password" value={apiKey} onChange={e => setApiKey(e.target.value)}
            placeholder="Enter your Gemini API key..."
            className="w-full border rounded-lg px-3 py-2 text-sm focus:outline-none focus:ring-2 focus:ring-blue-400 mb-4" />
          <button onClick={handleSave} className="bg-blue-600 text-white px-6 py-2 rounded-lg text-sm font-medium hover:bg-blue-700">
            {saved ? '&#10003; Saved!' : 'Save'}
          </button>
        </div>
      </div>
    </div>
  )
}
