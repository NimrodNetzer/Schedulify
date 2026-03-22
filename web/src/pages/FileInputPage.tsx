import { useState, useRef, useEffect } from 'react'
import { useNavigate } from 'react-router-dom'
import { coursesApi, filesApi, universityProfilesApi } from '../api/api'
import { useCoursesStore } from '../stores/useCoursesStore'
import { useFilesStore } from '../stores/useFilesStore'
import { LoadingOverlay } from '../components/common/LoadingOverlay'
import { ErrorBanner } from '../components/common/ErrorBanner'
import { ColumnMapperWizard } from '../components/files/ColumnMapperWizard'

export function FileInputPage() {
  const navigate = useNavigate()
  const { setAvailableCourses } = useCoursesStore()
  const { files, setFiles, removeFile } = useFilesStore()
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState<string | null>(null)
  const [tab, setTab] = useState<'upload' | 'history' | 'url'>('upload')
  const [urlInput, setUrlInput] = useState('')
  const [selectedFileIds, setSelectedFileIds] = useState<number[]>([])
  const fileRef = useRef<HTMLInputElement>(null)
  const mappedFileRef = useRef<HTMLInputElement>(null)

  // Wizard state
  const [wizardFile, setWizardFile] = useState<File | null>(null)
  const [previewRows, setPreviewRows] = useState<string[][] | null>(null)
  const [savedProfiles, setSavedProfiles] = useState<Array<{ id: number; name: string; mapping: Record<string, unknown> }>>([])
  const [selectedProfile, setSelectedProfile] = useState<number | null>(null)

  useEffect(() => {
    filesApi.getHistory().then(setFiles).catch(() => {})
    universityProfilesApi.list().then(setSavedProfiles).catch(() => {})
  }, [])

  const finishWithCourses = async (courses: ReturnType<typeof Array>[]) => {
    if (!courses || courses.length === 0) {
      setError('No courses found. Check the column mapping.')
      return
    }
    setAvailableCourses(courses as never)
    await filesApi.getHistory().then(setFiles).catch(() => {})
    navigate('/courses')
  }

  const handleUpload = async (file: File) => {
    setLoading(true)
    setError(null)
    try {
      const courses = await coursesApi.upload(file)
      if (!courses || courses.length === 0) {
        setError('No courses found in file. Check the format.')
        return
      }
      setAvailableCourses(courses)
      await filesApi.getHistory().then(setFiles).catch(() => {})
      navigate('/courses')
    } catch (e: unknown) {
      const err = e as { response?: { data?: { error?: string } } }
      setError(err?.response?.data?.error || 'Failed to upload file.')
    } finally {
      setLoading(false)
    }
  }

  // Custom format upload: preview first, then show wizard
  const handleCustomUploadFile = async (file: File) => {
    setLoading(true)
    setError(null)
    try {
      const rows = await universityProfilesApi.previewFile(file)
      setWizardFile(file)
      setPreviewRows(rows)
    } catch (e: unknown) {
      const err = e as { response?: { data?: { error?: string } } }
      setError(err?.response?.data?.error || 'Failed to preview file.')
    } finally {
      setLoading(false)
    }
  }

  const handleWizardConfirm = async (mapping: Record<string, unknown>) => {
    if (!wizardFile) return
    setLoading(true)
    setPreviewRows(null)
    try {
      // Save profile if name provided
      if (mapping.universityName) {
        await universityProfilesApi.save(mapping.universityName as string, mapping).catch(() => {})
        universityProfilesApi.list().then(setSavedProfiles).catch(() => {})
      }
      const courses = await universityProfilesApi.uploadWithMapping(wizardFile, mapping)
      await finishWithCourses(courses)
    } catch (e: unknown) {
      const err = e as { response?: { data?: { error?: string } } }
      setError(err?.response?.data?.error || 'Import failed.')
    } finally {
      setLoading(false)
      setWizardFile(null)
    }
  }

  // Upload using a saved profile directly (no wizard)
  const handleUploadWithProfile = async (file: File) => {
    if (selectedProfile === null) return
    const profile = savedProfiles.find(p => p.id === selectedProfile)
    if (!profile) return
    setLoading(true)
    setError(null)
    try {
      const courses = await universityProfilesApi.uploadWithMapping(file, profile.mapping)
      await finishWithCourses(courses)
    } catch (e: unknown) {
      const err = e as { response?: { data?: { error?: string } } }
      setError(err?.response?.data?.error || 'Import failed.')
    } finally {
      setLoading(false)
    }
  }

  const handleFetchUrl = async () => {
    if (!urlInput.trim()) return
    setLoading(true)
    setError(null)
    try {
      const courses = await coursesApi.fetchFromUrl(urlInput.trim())
      if (!courses || courses.length === 0) {
        setError('No courses found in the file at that URL.')
        return
      }
      setAvailableCourses(courses)
      await filesApi.getHistory().then(setFiles).catch(() => {})
      navigate('/courses')
    } catch (e: unknown) {
      const err = e as { response?: { data?: { error?: string } } }
      setError(err?.response?.data?.error || 'Failed to fetch file from URL.')
    } finally {
      setLoading(false)
    }
  }

  const handleLoadHistory = async () => {
    if (selectedFileIds.length === 0) return
    setLoading(true)
    setError(null)
    try {
      const courses = await coursesApi.loadFromHistory(selectedFileIds)
      setAvailableCourses(courses)
      navigate('/courses')
    } catch (e: unknown) {
      const err = e as { response?: { data?: { error?: string } } }
      setError(err?.response?.data?.error || 'Failed to load from history.')
    } finally {
      setLoading(false)
    }
  }

  const handleDelete = async (id: number) => {
    try { await filesApi.delete(id); removeFile(id) } catch { /* ignore */ }
  }

  const toggleFileId = (id: number) => {
    setSelectedFileIds(prev =>
      prev.includes(id) ? prev.filter(x => x !== id) : [...prev, id]
    )
  }

  return (
    <div className="min-h-screen bg-gray-50 flex flex-col items-center justify-center p-6">
      {loading && <LoadingOverlay message="Processing file..." />}
      {previewRows && wizardFile && (
        <ColumnMapperWizard
          previewRows={previewRows}
          onConfirm={handleWizardConfirm}
          onCancel={() => { setPreviewRows(null); setWizardFile(null) }}
        />
      )}

      <div className="bg-white rounded-2xl shadow-lg p-8 w-full max-w-lg">
        <h1 className="text-2xl font-bold text-gray-800 mb-2">&#128197; Schedulify</h1>
        <p className="text-gray-500 mb-6">Upload your university course file to get started.</p>

        {error && <div className="mb-4"><ErrorBanner message={error} onClose={() => setError(null)} /></div>}

        {/* Tabs */}
        <div className="flex border-b mb-6 flex-wrap gap-0">
          {(['upload', 'history', 'url'] as const).map(t => (
            <button key={t} onClick={() => setTab(t)}
              className={`pb-2 px-3 text-sm font-medium border-b-2 transition-colors ${tab === t ? 'border-blue-600 text-blue-600' : 'border-transparent text-gray-500 hover:text-gray-700'}`}>
              {t === 'upload' ? 'Upload File' : t === 'history' ? `History (${files.length})` : 'Fetch URL'}
            </button>
          ))}
        </div>

        {tab === 'upload' && (
          <div className="space-y-4">
            {/* Standard upload */}
            <div
              className="border-2 border-dashed border-gray-300 rounded-xl p-8 text-center cursor-pointer hover:border-blue-400 hover:bg-blue-50 transition-colors"
              onClick={() => fileRef.current?.click()}
              onDragOver={e => e.preventDefault()}
              onDrop={e => { e.preventDefault(); const f = e.dataTransfer.files[0]; if (f) handleUpload(f) }}
            >
              <div className="text-4xl mb-2">&#128194;</div>
              <p className="text-gray-600 font-medium text-sm">Drag &amp; drop or click — standard format</p>
              <input ref={fileRef} type="file" accept=".xlsx,.txt" className="hidden"
                onChange={e => { const f = e.target.files?.[0]; if (f) handleUpload(f) }} />
            </div>

            {/* Custom format section */}
            <div className="border border-gray-200 rounded-xl p-4 space-y-3">
              <p className="text-sm font-medium text-gray-700">Different format?</p>

              {savedProfiles.length > 0 && (
                <div>
                  <p className="text-xs text-gray-500 mb-1">Use a saved university profile:</p>
                  <div className="flex gap-2">
                    <select value={selectedProfile ?? ''} onChange={e => setSelectedProfile(parseInt(e.target.value) || null)}
                      className="flex-1 border border-gray-300 rounded-lg px-2 py-1.5 text-sm focus:outline-none focus:border-blue-400">
                      <option value="">— select profile —</option>
                      {savedProfiles.map(p => <option key={p.id} value={p.id}>{p.name}</option>)}
                    </select>
                    <button disabled={selectedProfile === null}
                      onClick={() => mappedFileRef.current?.click()}
                      className="px-3 py-1.5 bg-blue-600 text-white rounded-lg text-sm hover:bg-blue-700 disabled:opacity-40">
                      Upload
                    </button>
                    <input ref={mappedFileRef} type="file" accept=".xlsx,.txt" className="hidden"
                      onChange={e => { const f = e.target.files?.[0]; if (f) handleUploadWithProfile(f) }} />
                  </div>
                </div>
              )}

              <button onClick={() => { /* trigger file picker for wizard */ fileRef.current && (fileRef.current.onchange = null); const input = document.createElement('input'); input.type='file'; input.accept='.xlsx,.txt'; input.onchange = (e) => { const f = (e.target as HTMLInputElement).files?.[0]; if (f) handleCustomUploadFile(f) }; input.click() }}
                className="w-full border border-dashed border-gray-300 rounded-lg py-2 text-sm text-gray-500 hover:border-blue-400 hover:text-blue-600 transition-colors">
                + Upload with column mapping wizard
              </button>
            </div>
          </div>
        )}

        {tab === 'url' && (
          <div className="space-y-3">
            <p className="text-sm text-gray-500">Enter a direct URL to an <b>.xlsx</b> or <b>.txt</b> course file.</p>
            <input type="url" placeholder="https://example.com/courses.xlsx"
              value={urlInput} onChange={e => setUrlInput(e.target.value)}
              onKeyDown={e => e.key === 'Enter' && handleFetchUrl()}
              className="w-full border border-gray-300 rounded-lg px-3 py-2 text-sm focus:outline-none focus:border-blue-400" />
            <button onClick={handleFetchUrl} disabled={!urlInput.trim()}
              className="w-full bg-blue-600 text-white rounded-lg py-2 font-medium hover:bg-blue-700 disabled:opacity-40 text-sm">
              Fetch &amp; Load
            </button>
          </div>
        )}

        {tab === 'history' && (
          <div className="space-y-2">
            {files.length === 0 && <p className="text-gray-400 text-center py-4">No files in history.</p>}
            {files.map(f => (
              <div key={f.id} className={`flex items-center gap-3 p-3 rounded-lg border cursor-pointer transition-colors ${selectedFileIds.includes(f.id) ? 'border-blue-500 bg-blue-50' : 'border-gray-200 hover:border-gray-300'}`}
                onClick={() => toggleFileId(f.id)}>
                <input type="checkbox" checked={selectedFileIds.includes(f.id)} onChange={() => toggleFileId(f.id)} className="rounded" onClick={e => e.stopPropagation()} />
                <div className="flex-1 min-w-0">
                  <p className="font-medium text-gray-800 truncate">{f.file_name}</p>
                  <p className="text-xs text-gray-400">{new Date(f.upload_time).toLocaleDateString()}</p>
                </div>
                <button onClick={e => { e.stopPropagation(); handleDelete(f.id) }} className="text-red-400 hover:text-red-600 text-sm px-2">&#x2715;</button>
              </div>
            ))}
            {files.length > 0 && (
              <button onClick={handleLoadHistory} disabled={selectedFileIds.length === 0}
                className="w-full mt-3 bg-blue-600 text-white rounded-lg py-2 font-medium hover:bg-blue-700 disabled:opacity-40">
                Load Selected ({selectedFileIds.length})
              </button>
            )}
          </div>
        )}
      </div>
    </div>
  )
}
