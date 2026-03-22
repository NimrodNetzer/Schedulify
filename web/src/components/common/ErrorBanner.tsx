export function ErrorBanner({ message, onClose }: { message: string; onClose: () => void }) {
  return (
    <div className="bg-red-50 border border-red-200 rounded-lg p-4 flex items-start gap-3">
      <span className="text-red-500 text-xl">&#9888;</span>
      <p className="text-red-700 flex-1">{message}</p>
      <button onClick={onClose} className="text-red-400 hover:text-red-600 font-bold">&#x2715;</button>
    </div>
  )
}
