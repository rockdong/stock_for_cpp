import * as React from "react"
import { ChevronLeft, ChevronRight } from "lucide-react"
import { DayPicker } from "react-day-picker"

import { cn } from "@/lib/utils"

export type CalendarProps = React.ComponentProps<typeof DayPicker>

function Calendar({
  className,
  classNames,
  showOutsideDays = true,
  ...props
}: CalendarProps) {
  return (
    <DayPicker
      showOutsideDays={showOutsideDays}
      className={cn("p-3", className)}
      classNames={{
        months: "flex flex-col gap-4",
        month: "flex flex-col gap-4",
        month_caption: "flex justify-between pt-1 relative items-center w-full px-8",
        caption_label: "text-sm font-medium text-text-primary",
        nav: "flex items-center gap-1",
        button_previous: "h-7 w-7 bg-elevated border border-border-default rounded-md p-0 opacity-70 hover:opacity-100 text-text-secondary hover:text-text-primary hover:border-accent-blue/50 transition-all cursor-pointer flex items-center justify-center",
        button_next: "h-7 w-7 bg-elevated border border-border-default rounded-md p-0 opacity-70 hover:opacity-100 text-text-secondary hover:text-text-primary hover:border-accent-blue/50 transition-all cursor-pointer flex items-center justify-center",
        month_grid: "w-full border-collapse",
        weekdays: "flex flex-row",
        weekday: "text-text-tertiary rounded-md w-9 font-normal text-[0.8rem]",
        week: "flex flex-row w-full mt-2",
        day: "relative p-0 text-center text-sm focus-within:relative focus-within:z-20 [&:has([aria-selected])]:bg-accent-blue-bg first:[&:has([aria-selected])]:rounded-l-md last:[&:has([aria-selected])]:rounded-r-md",
        day_button: "h-9 w-9 p-0 font-normal rounded-md text-text-secondary hover:bg-elevated hover:text-text-primary transition-colors cursor-pointer",
        range_start: "day-range-start",
        range_end: "day-range-end",
        selected: "!bg-accent-blue !text-white font-bold text-base rounded-md shadow-glow-blue ring-2 ring-white/20",
        today: "bg-accent-blue/20 text-accent-blue rounded-md font-semibold",
        outside: "text-text-tertiary opacity-40",
        disabled: "text-text-tertiary opacity-30 cursor-not-allowed",
        range_middle: "aria-selected:bg-accent-blue-bg aria-selected:text-accent-blue",
        hidden: "invisible",
        ...classNames,
      }}
      components={{
        Chevron: ({ orientation }) => {
          const Icon = orientation === "left" ? ChevronLeft : ChevronRight
          return <Icon className="h-4 w-4" />
        },
      }}
      {...props}
    />
  )
}
Calendar.displayName = "Calendar"

export { Calendar }