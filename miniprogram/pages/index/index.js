// pages/index/index.js
const { stockApi, analysisApi, userApi } = require('../../utils/api')
const { formatPrice, formatChange, getSignalClass, getSignalText } = require('../../utils/util')

Page({
  data: {
    overview: {
      buyCount: 0,
      sellCount: 0,
      holdCount: 0
    },
    watchlist: [],
    loading: true
  },

  onLoad() {
    this.loadData()
  },

  onShow() {
    // 每次显示时刷新数据
    this.loadData()
  },

  onPullDownRefresh() {
    this.loadData().then(() => {
      wx.stopPullDownRefresh()
    })
  },

  async loadData() {
    try {
      // 并行加载数据
      const [watchlist, signals] = await Promise.all([
        this.loadWatchlist(),
        this.loadSignals()
      ])
      
      this.setData({ 
        watchlist,
        loading: false 
      })
    } catch (err) {
      console.error('加载数据失败', err)
      this.setData({ loading: false })
    }
  },

  async loadWatchlist() {
    try {
      const res = await userApi.getWatchlist()
      const watchlist = (res.data || []).map(item => ({
        ...item,
        price: formatPrice(item.price),
        changeText: formatChange(item.change),
        signalClass: getSignalClass(item.signal),
        signalText: getSignalText(item.signal)
      }))
      return watchlist
    } catch (err) {
      // 如果获取失败，使用本地缓存
      const app = getApp()
      return app.globalData.watchlist || []
    }
  },

  async loadSignals() {
    try {
      const res = await analysisApi.getSignals()
      const signals = res.data || []
      
      // 统计各类信号数量
      const overview = {
        buyCount: signals.filter(s => s.label === 'BUY').length,
        sellCount: signals.filter(s => s.label === 'SELL').length,
        holdCount: signals.filter(s => s.label === 'HOLD').length
      }
      
      this.setData({ overview })
    } catch (err) {
      console.error('加载信号失败', err)
    }
  },

  goToSearch() {
    wx.navigateTo({
      url: '/pages/search/search'
    })
  },

  goToStockDetail(e) {
    const code = e.currentTarget.dataset.code
    wx.navigateTo({
      url: `/pages/stock/stock?code=${code}`
    })
  }
})