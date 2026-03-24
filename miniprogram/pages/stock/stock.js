// pages/stock/stock.js
const { stockApi, analysisApi, userApi } = require('../../utils/api')
const { formatPrice, formatChange, getSignalClass, getSignalText, getSignalIcon } = require('../../utils/util')

Page({
  data: {
    code: '',
    stock: {},
    analysis: null,
    indicators: {},
    chartData: [],
    freq: 'd',
    chartLoading: false,
    isSubscribed: false
  },

  onLoad(options) {
    const code = options.code
    if (code) {
      this.setData({ code })
      this.loadStockData(code)
    }
  },

  async loadStockData(code) {
    wx.showLoading({ title: '加载中...' })
    
    try {
      // 并行加载数据
      const [stockRes, analysisRes, chartRes] = await Promise.all([
        stockApi.getDetail(code).catch(() => null),
        analysisApi.getStockAnalysis(code).catch(() => null),
        stockApi.getChart(code, this.data.freq).catch(() => null)
      ])

      const stock = stockRes?.data || {}
      const analysis = analysisRes?.data || null

      this.setData({
        stock: {
          ...stock,
          price: formatPrice(stock.price),
          changeText: formatChange(stock.change)
        },
        analysis: analysis ? {
          ...analysis,
          signalClass: getSignalClass(analysis.label),
          signalText: getSignalText(analysis.label),
          signalIcon: getSignalIcon(analysis.label)
        } : null,
        indicators: {
          ema17: formatPrice(stock.ema17),
          ema25: formatPrice(stock.ema25),
          macd: stock.macd_signal || '--',
          rsi: stock.rsi || '--'
        },
        chartData: chartRes?.data || [],
        isSubscribed: stock.isSubscribed || false
      })
    } catch (err) {
      console.error('加载数据失败', err)
    } finally {
      wx.hideLoading()
    }
  },

  async changeFreq(e) {
    const freq = e.currentTarget.dataset.freq
    if (freq === this.data.freq) return
    
    this.setData({ freq, chartLoading: true })
    
    try {
      const res = await stockApi.getChart(this.data.code, freq)
      this.setData({ 
        chartData: res?.data || [],
        chartLoading: false 
      })
    } catch (err) {
      console.error('加载图表失败', err)
      this.setData({ chartLoading: false })
    }
  },

  async toggleSubscribe() {
    try {
      if (this.data.isSubscribed) {
        await userApi.unsubscribe(this.data.code)
        wx.showToast({ title: '已取消订阅', icon: 'success' })
      } else {
        await userApi.subscribe(this.data.code)
        wx.showToast({ title: '订阅成功', icon: 'success' })
      }
      this.setData({ isSubscribed: !this.data.isSubscribed })
    } catch (err) {
      wx.showToast({ title: '操作失败', icon: 'none' })
    }
  },

  onShareAppMessage() {
    return {
      title: `${this.data.stock.name} - 股票分析`,
      path: `/pages/stock/stock?code=${this.data.code}`
    }
  }
})