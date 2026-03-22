export interface Session {
  day_of_week: number
  start_time: string
  end_time: string
  building_number: string
  room_number: string
}

export type SessionType =
  | 'LECTURE' | 'TUTORIAL' | 'LAB' | 'BLOCK'
  | 'DEPARTMENTAL_SESSION' | 'REINFORCEMENT' | 'GUIDANCE'
  | 'OPTIONAL_COLLOQUIUM' | 'REGISTRATION' | 'THESIS' | 'PROJECT' | 'UNSUPPORTED'

export interface Group {
  type: SessionType
  sessions: Session[]
}

export interface Course {
  id: number
  semester: number
  raw_id: string
  name: string
  teacher: string
  lectures: Group[]
  tutorials: Group[]
  labs: Group[]
  blocks: Group[]
  departmental_sessions: Group[]
  reinforcements: Group[]
  guidance: Group[]
  optional_colloquium: Group[]
  registration: Group[]
  thesis: Group[]
  project: Group[]
}

export interface ScheduleItem {
  courseName: string
  raw_id: string
  type: string
  start: string
  end: string
  building: string
  room: string
}

export interface ScheduleDay {
  day: string
  day_items: ScheduleItem[]
}

export interface Schedule {
  index: number
  unique_id: string
  semester: string
  amount_days: number
  amount_gaps: number
  gaps_time: number
  avg_start: number
  avg_end: number
  earliest_start: number
  latest_end: number
  longest_gap: number
  total_class_time: number
  consecutive_days: number
  days_json: string
  weekend_classes: boolean
  has_morning_classes: boolean
  has_early_morning: boolean
  has_evening_classes: boolean
  has_late_evening: boolean
  max_daily_hours: number
  min_daily_hours: number
  avg_daily_hours: number
  has_lunch_break: boolean
  max_daily_gaps: number
  avg_gap_length: number
  schedule_span: number
  compactness_ratio: number
  weekday_only: boolean
  has_monday: boolean
  has_tuesday: boolean
  has_wednesday: boolean
  has_thursday: boolean
  has_friday: boolean
  has_saturday: boolean
  has_sunday: boolean
  week: ScheduleDay[]
}

export interface FileRecord {
  id: number
  file_name: string
  file_type: string
  upload_time: string
  updated_at: string
}

export interface BotMessage {
  role: 'user' | 'bot'
  text: string
  isFilter?: boolean
  filteredCount?: number
}
