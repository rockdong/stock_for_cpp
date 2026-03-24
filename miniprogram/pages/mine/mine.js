// pages/mine/mine.js
const app = getApp()
const { userApi } = require('../../utils/api')

Page({
  data: {
    userInfo: null,
    watchlistCount: 0
  },

  onShow() {
    this.loadUserInfo()
  },

  loadUserInfo() {
    const userInfo = app.globalData.userInfo
    const watchlist = app.globalData.watchlist || []
    
    this.setData({
      userInfo,
      watchlistCount: watchlist.length
    })
  },

  goToLogin() {
    wx.getUserProfile({
      desc: '用于完善用户资料',
      success: (res) => {
        app.globalData.userInfo = res.userInfo
        this.setData({ userInfo: res.userInfo })
        wx.showToast({ title: '登录成功', icon: 'success' })
      }
    })
  },

  goToWatchlist() {
    wx.showToast({ title: '功能开发中', icon: 'none' })
  },

  goToSubscribe() {
    wx.showToast({ title: '功能开发中', icon: 'none' })
  },

  clearCache() {
    wx.showModal({
      title: '确认清除',
      content: '确定要清除本地缓存吗？',
      success: (res) => {
        if (res.confirm) {
          wx.clearStorageSync()
          app.globalData.watchlist = []
          wx.showToast({ title: '清除成功', icon: 'success' })
          this.setData({ watchlistCount: 0 })
        }
      }
    })
  },

  goToAbout() {
    wx.showModal({
      title: '关于我们',
      content: '股票分析微信小程序\n基于 C++ 分析引擎\n提供专业的技术分析和买卖信号',
      showCancel: false
    })
  },

  goToFeedback() {
    wx.showToast({ title: '功能开发中', icon: 'none' })
  }
})