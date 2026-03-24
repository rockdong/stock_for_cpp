// pages/analysis/analysis.js
const { analysisApi } = require('../../utils/api')
const { getFreqText } = require('../../utils/util')

Page({
  data: {
    signals: [],
    buySignals: [],
    sellSignals: [],
    holdSignals: [],
    loading: true
  },

  onLoad() {
    this.loadSignals()
  },

  onShow() {
    this.loadSignals()
  },

  onPullDownRefresh() {
    this.loadSignals().then(() => {
      wx.stopPullDownRefresh()
    })
  },

  async loadSignals() {
    try {
      const res = await analysisApi.getSignals()
      const signals = (res.data || []).map(item => ({
        ...item,
        freqText: getFreqText(item.freq)
      }))

      // 按信号类型分组
      const buySignals = signals.filter(s => s.label === 'BUY')
      const sellSignals = signals.filter(s => s.label === 'SELL')
      const holdSignals = signals.filter(s => s.label === 'HOLD')

      this.setData({
        signals,
        buySignals,
        sellSignals,
        holdSignals,
        loading: false
      })
    } catch (err) {
      console.error('加载信号失败', err)
      this.setData({ loading: false })
    }
  },

  goToStockDetail(e) {
    const code = e.currentTarget.dataset.code
    wx.navigateTo({
      url: `/pages/stock/stock?code=${code}`
    })
  }
})