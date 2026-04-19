import * as React from "react"
import { format } from "date-fns"
import { CalendarIcon } from "lucide-react"

import { cn } from "@/lib/utils"
import { Calendar } from "@/components/ui/calendar"
import {
  Popover,
  PopoverContent,
  PopoverTrigger,
} from "@/components/ui/popover"

interface DatePickerProps {
  value?: string
  onChange?: (value: string) => void
  placeholder?: string
  className?: string
}

export function DatePicker({
  value,
  onChange,
  placeholder = "选择日期",
  className,
}: DatePickerProps) {
  const [date, setDate] = React.useState<Date | undefined>(
    value ? new Date(value) : undefined
  )

  const handleSelect = (selectedDate: Date | undefined) => {
    setDate(selectedDate)
    if (selectedDate && onChange) {
      onChange(format(selectedDate, "yyyy-MM-dd"))
    }
  }

  return (
    <Popover>
      <PopoverTrigger asChild>
        <button
          className={cn(
            "w-full flex items-center gap-2 px-3.5 py-2.5 bg-elevated border border-border-default rounded-lg text-sm tabular-nums",
            "hover:border-accent-blue/50 hover:bg-elevated/80 transition-colors cursor-pointer",
            "focus:outline-none focus:border-accent-blue focus:ring-1 focus:ring-accent-blue/30",
            !date && "text-text-tertiary",
            date && "text-text-primary",
            className
          )}
        >
          <CalendarIcon className="w-4 h-4 text-text-tertiary" />
          {date ? format(date, "yyyy-MM-dd") : placeholder}
        </button>
      </PopoverTrigger>
      <PopoverContent 
        className="w-auto p-0 bg-surface border border-border-default rounded-xl shadow-lg" 
        align="start"
      >
        <Calendar
          mode="single"
          selected={date}
          onSelect={handleSelect}
          initialFocus
        />
      </PopoverContent>
    </Popover>
  )
}