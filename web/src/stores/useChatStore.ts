import { create } from 'zustand'
import type { BotMessage } from '../types/models'

interface ChatStore {
  messages: BotMessage[]
  isLoading: boolean
  addMessage: (msg: BotMessage) => void
  setLoading: (v: boolean) => void
  clearMessages: () => void
}

export const useChatStore = create<ChatStore>((set) => ({
  messages: [],
  isLoading: false,
  addMessage: (msg) => set(s => ({ messages: [...s.messages, msg] })),
  setLoading: (v) => set({ isLoading: v }),
  clearMessages: () => set({ messages: [] }),
}))
