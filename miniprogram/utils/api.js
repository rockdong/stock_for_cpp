// utils/api.js - API 封装

const app = getApp()

/**
 * 封装请求方法
 */
const request = (url, options = {}) => {
  return new Promise((resolve, reject) => {
    const token = app.globalData.token || wx.getStorageSync('token')
    
    wx.request({
      url: app.globalData.apiBase + url,
      method: options.method || 'GET',
      data: options.data,
      header: {
        'Authorization': token ? `Bearer ${token}` : '',
        'Content-Type': 'application/json'
      },
      success: (res) => {
        if (res.statusCode === 200) {
          resolve(res.data)
        } else if (res.statusCode === 401) {
          // 未授权，跳转登录
          app.login().then(() => {
            // 重试请求
            request(url, options).then(resolve).catch(reject)
          })
        } else {
          reject(res.data)
        }
      },
      fail: (err) => {
        wx.showToast({
          title: '网络请求失败',
          icon: 'none'
        })
        reject(err)
      }
    })
  })
}

/**
 * 股票相关 API
 */
const stockApi = {
  // 获取股票列表
  getList: () => request('/stocks'),
  
  // 获取股票详情
  getDetail: (code) => request(`/stocks/${code}`),
  
  // 搜索股票
  search: (keyword) => request(`/stocks/search?keyword=${encodeURIComponent(keyword)}`),
  
  // 获取 K 线数据
  getChart: (code, freq = 'd') => request(`/charts/${code}?freq=${freq}`)
}

/**
 * 分析相关 API
 */
const analysisApi = {
  // 获取所有分析信号
  getSignals: () => request('/analysis/signals'),
  
  // 获取单股分析结果
  getStockAnalysis: (code) => request(`/analysis/${code}`),
  
  // 获取分析进度
  getProgress: () => request('/analysis/progress')
}

/**
 * 用户相关 API
 */
const userApi = {
  // 获取自选股列表
  getWatchlist: () => request('/user/watchlist'),
  
  // 添加自选股
  addToWatchlist: (code) => request('/user/watchlist', {
    method: 'POST',
    data: { ts_code: code }
  }),
  
  // 移除自选股
  removeFromWatchlist: (code) => request(`/user/watchlist/${code}`, {
    method: 'DELETE'
  }),
  
  // 订阅推送
  subscribe: (code) => request('/user/subscribe', {
    method: 'POST',
    data: { ts_code: code }
  }),
  
  // 取消订阅
  unsubscribe: (code) => request(`/user/subscribe/${code}`, {
    method: 'DELETE'
  })
}

module.exports = {
  request,
  stockApi,
  analysisApi,
  userApi
}