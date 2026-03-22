import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom'
import { FileInputPage } from './pages/FileInputPage'
import { CourseSelectionPage } from './pages/CourseSelectionPage'
import { SchedulesPage } from './pages/SchedulesPage'
import { SettingsPage } from './pages/SettingsPage'

export default function App() {
  return (
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<FileInputPage />} />
        <Route path="/courses" element={<CourseSelectionPage />} />
        <Route path="/schedules" element={<SchedulesPage />} />
        <Route path="/settings" element={<SettingsPage />} />
        <Route path="*" element={<Navigate to="/" />} />
      </Routes>
    </BrowserRouter>
  )
}
