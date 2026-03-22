import axios from 'axios'
import type { Course, Schedule, FileRecord } from '../types/models'

const api = axios.create({ baseURL: '/api' })

export const coursesApi = {
  upload: async (file: File): Promise<Course[]> => {
    const form = new FormData()
    form.append('file', file)
    const { data } = await api.post('/courses/upload', form)
    return data
  },
  fetchFromUrl: async (url: string): Promise<Course[]> => {
    const { data } = await api.post('/courses/fetch', { url })
    return data
  },
  loadFromHistory: async (fileIds: number[]): Promise<Course[]> => {
    const { data } = await api.post('/courses/load', { file_ids: fileIds })
    return data
  },
  validate: async (courses: Course[]): Promise<string[]> => {
    const { data } = await api.post('/courses/validate', { courses })
    return data.warnings ?? []
  },
}

export const filesApi = {
  getHistory: async (): Promise<FileRecord[]> => {
    const { data } = await api.get('/files')
    return data
  },
  delete: async (id: number): Promise<boolean> => {
    const { data } = await api.delete(`/files/${id}`)
    return data.success
  },
}

export const schedulesApi = {
  generate: async (courses: Course[], semester: string): Promise<Schedule[]> => {
    const { data } = await api.post('/schedules/generate', { courses, semester })
    return data
  },
  exportCsv: async (schedule: Schedule): Promise<void> => {
    const response = await api.post('/schedules/export/csv', schedule, { responseType: 'blob' })
    const url = URL.createObjectURL(response.data)
    const a = document.createElement('a')
    a.href = url
    a.download = `schedule_${schedule.index}.csv`
    a.click()
    URL.revokeObjectURL(url)
  },
  exportIcal: async (schedule: Schedule): Promise<void> => {
    const response = await api.post('/schedules/export/ical', schedule, { responseType: 'blob' })
    const url = URL.createObjectURL(response.data)
    const a = document.createElement('a')
    a.href = url
    a.download = `schedule_${schedule.index}.ics`
    a.click()
    URL.revokeObjectURL(url)
  },
  getFilteredIds: async (): Promise<number[]> => {
    const { data } = await api.get('/schedules/filtered-ids')
    return data.ids ?? []
  },
  getFilteredUniqueIds: async (): Promise<string[]> => {
    const { data } = await api.get('/schedules/filtered-unique-ids')
    return data.unique_ids ?? []
  },
  clean: async (): Promise<void> => {
    await api.post('/schedules/clean')
  },
}

export const botApi = {
  query: async (
    message: string,
    availableUniqueIds: string[],
    semester: string
  ): Promise<{ response: string; filteredUniqueIds: string[]; isFilter: boolean }> => {
    const { data } = await api.post('/bot/query', {
      message,
      available_unique_ids: availableUniqueIds,
      semester,
    })
    return {
      response: data.message || 'Done.',
      filteredUniqueIds: data.filtered_unique_ids ?? [],
      isFilter: data.is_filter_query ?? false,
    }
  },
}

export const universityProfilesApi = {
  list: async () => {
    const { data } = await api.get('/university-profiles')
    return data as Array<{ id: number; name: string; mapping: Record<string, unknown>; created_at: string }>
  },
  save: async (name: string, mapping: Record<string, unknown>) => {
    const { data } = await api.post('/university-profiles', { name, mapping })
    return data.success as boolean
  },
  delete: async (id: number) => {
    const { data } = await api.delete(`/university-profiles/${id}`)
    return data.success as boolean
  },
  previewFile: async (file: File): Promise<string[][]> => {
    const form = new FormData()
    form.append('file', file)
    const { data } = await api.post('/courses/upload/preview', form)
    return data as string[][]
  },
  uploadWithMapping: async (file: File, mapping: Record<string, unknown>): Promise<Course[]> => {
    const form = new FormData()
    form.append('file', file)
    form.append('mapping', JSON.stringify(mapping))
    const { data } = await api.post('/courses/upload/mapped', form)
    return data
  },
}

export const settingsApi = {
  get: async (key: string): Promise<string> => {
    const { data } = await api.get(`/settings/${key}`)
    return data.value ?? ''
  },
  set: async (key: string, value: string): Promise<void> => {
    await api.put(`/settings/${key}`, { value })
  },
}
