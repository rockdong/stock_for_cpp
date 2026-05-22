export interface StrategyInfo {
  emoji: string;
  name: string;
  description: string;
}

export const STRATEGY_EMOJI_MAP: Record<string, StrategyInfo> = {
  '🔗': { emoji: '🔗', name: 'EMA17TO25Strategy', description: 'EMA17/EMA25 黄金交叉' },
  '⬆️': { emoji: '⬆️', name: 'EMA25Greater17PriceMatchStrategy', description: 'EMA25 > EMA17 价格匹配上涨' },
  '🚀': { emoji: '🚀', name: 'EMA17BreakoutStrategy', description: 'EMA17 突破策略' },
  '🔄': { emoji: '🔄', name: 'EMA25CrossoverStrategy', description: 'EMA25 交叉策略' },
  '🎯': { emoji: '🎯', name: 'EMAConvergenceStrategy', description: 'EMA 收敛策略' },
  '📈': { emoji: '📈', name: 'MACrossStrategy', description: '均线交叉策略' },
  '📊': { emoji: '📊', name: 'MACDStrategy', description: 'MACD 指标策略' },
  '🎢': { emoji: '🎢', name: 'RSIStrategy', description: 'RSI 超买超卖策略' },
  '🎪': { emoji: '🎪', name: 'BOLLStrategy', description: '布林带策略' },
  '🧱': { emoji: '🧱', name: 'GridStrategy', description: '网格交易策略' },
  '💥': { emoji: '💥', name: 'SurgeSignalStrategy', description: '暴涨预警策略' },
  '💎': { emoji: '💎', name: 'FundamentalStrategy', description: '基本面策略 - 估值/盈利/增长/健康评分' },
};

export function getStrategyDescription(emoji: string): string {
  return STRATEGY_EMOJI_MAP[emoji]?.description || emoji;
}

export function getStrategyName(emoji: string): string {
  return STRATEGY_EMOJI_MAP[emoji]?.name || emoji;
}