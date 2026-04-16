import { useState } from 'react'

type TabType = 'analysis' | 'fundamental'

interface TabNavProps {
  defaultTab?: TabType
  onTabChange?: (tab: TabType) => void
}

export default function TabNav({ defaultTab = 'analysis', onTabChange }: TabNavProps) {
  const [activeTab, setActiveTab] = useState<TabType>(defaultTab)

  const handleTabClick = (tab: TabType) => {
    setActiveTab(tab)
    onTabChange?.(tab)
  }

  return (
    <div className="flex gap-2 mb-6">
      <button
        onClick={() => handleTabClick('analysis')}
        className={`px-6 py-3 rounded-xl font-medium transition-colors cursor-pointer ${
          activeTab === 'analysis'
            ? 'bg-cta text-white'
            : 'bg-secondary text-muted hover:bg-secondary/80'
        }`}
      >
        技术策略分析
      </button>
      <button
        onClick={() => handleTabClick('fundamental')}
        className={`px-6 py-3 rounded-xl font-medium transition-colors cursor-pointer ${
          activeTab === 'fundamental'
            ? 'bg-cta text-white'
            : 'bg-secondary text-muted hover:bg-secondary/80'
        }`}
      >
        基本面筛选
      </button>
    </div>
  )
}