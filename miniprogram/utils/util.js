// utils/util.js - 工具函数

/**
 * 格式化价格
 */
const formatPrice = (price) => {
  if (price === null || price === undefined) return '--'
  return Number(price).toFixed(2)
}

/**
 * 格式化涨跌幅
 */
const formatChange = (change) => {
  if (change === null || change === undefined) return '--'
  const value = Number(change)
  const sign = value >= 0 ? '+' : ''
  return `${sign}${(value * 100).toFixed(2)}%`
}

/**
 * 格式化日期
 */
const formatDate = (date) => {
  if (!date) return '--'
  const d = new Date(date)
  const year = d.getFullYear()
  const month = String(d.getMonth() + 1).padStart(2, '0')
  const day = String(d.getDate()).padStart(2, '0')
  return `${year}-${month}-${day}`
}

/**
 * 格式化时间
 */
const formatTime = (date) => {
  if (!date) return '--'
  const d = new Date(date)
  const hour = String(d.getHours()).padStart(2, '0')
  const minute = String(d.getMinutes()).padStart(2, '0')
  return `${hour}:${minute}`
}

/**
 * 获取信号颜色类名
 */
const getSignalClass = (signal) => {
  const classMap = {
    'BUY': 'signal-buy',
    'SELL': 'signal-sell',
    'HOLD': 'signal-hold'
  }
  return classMap[signal] || 'signal-hold'
}

/**
 * 获取信号文本
 */
const getSignalText = (signal) => {
  const textMap = {
    'BUY': '买入',
    'SELL': '卖出',
    'HOLD': '持有'
  }
  return textMap[signal] || '持有'
}

/**
 * 获取信号图标
 */
const getSignalIcon = (signal) => {
  const iconMap = {
    'BUY': '🟢',
    'SELL': '🔴',
    'HOLD': '⚪'
  }
  return iconMap[signal] || '⚪'
}

/**
 * 获取周期文本
 */
const getFreqText = (freq) => {
  const textMap = {
    'd': '日线',
    'w': '周线',
    'm': '月线'
  }
  return textMap[freq] || '日线'
}

/**
 * 防抖函数
 */
const debounce = (fn, delay = 300) => {
  let timer = null
  return function(...args) {
    if (timer) clearTimeout(timer)
    timer = setTimeout(() => {
      fn.apply(this, args)
    }, delay)
  }
}

/**
 * 节流函数
 */
const throttle = (fn, delay = 300) => {
  let lastTime = 0
  return function(...args) {
    const now = Date.now()
    if (now - lastTime >= delay) {
      lastTime = now
      fn.apply(this, args)
    }
  }
}

module.exports = {
  formatPrice,
  formatChange,
  formatDate,
  formatTime,
  getSignalClass,
  getSignalText,
  getSignalIcon,
  getFreqText,
  debounce,
  throttle
}