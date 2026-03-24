// app.js
App({
  globalData: {
    userInfo: null,
    token: null,
    watchlist: [],
    apiBase: 'https://your-domain.com/api'
  },

  onLaunch() {
    // 检查登录态
    this.checkLoginStatus()
  },

  checkLoginStatus() {
    const token = wx.getStorageSync('token')
    if (token) {
      this.globalData.token = token
      this.getUserInfo()
    }
  },

  getUserInfo() {
    wx.getUserInfo({
      success: (res) => {
        this.globalData.userInfo = res.userInfo
      }
    })
  },

  login() {
    return new Promise((resolve, reject) => {
      wx.login({
        success: (res) => {
          if (res.code) {
            // 发送 code 到后端换取 token
            wx.request({
              url: `${this.globalData.apiBase}/auth/login`,
              method: 'POST',
              data: { code: res.code },
              success: (response) => {
                const token = response.data.token
                this.globalData.token = token
                wx.setStorageSync('token', token)
                resolve(token)
              },
              fail: reject
            })
          } else {
            reject(new Error('wx.login failed'))
          }
        },
        fail: reject
      })
    })
  },

  getWatchlist() {
    return this.globalData.watchlist
  },

  addToWatchlist(stock) {
    if (!this.globalData.watchlist.find(s => s.ts_code === stock.ts_code)) {
      this.globalData.watchlist.push(stock)
    }
  },

  removeFromWatchlist(ts_code) {
    const index = this.globalData.watchlist.findIndex(s => s.ts_code === ts_code)
    if (index > -1) {
      this.globalData.watchlist.splice(index, 1)
    }
  }
})