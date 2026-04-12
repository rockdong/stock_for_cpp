const express = require('express');
const router = express.Router();
const http = require('http');

const CPP_API_HOST = 'localhost';
const CPP_API_PORT = process.env.API_PORT || 3001;

function proxyToCpp(req, res) {
  const options = {
    hostname: CPP_API_HOST,
    port: CPP_API_PORT,
    path: req.originalUrl,
    method: req.method,
    headers: req.headers
  };
  
  const proxyReq = http.request(options, (proxyRes) => {
    res.status(proxyRes.statusCode);
    res.set(proxyRes.headers);
    proxyRes.pipe(res);
  });
  
  proxyReq.on('error', (err) => {
    res.status(502).json({ success: false, error: 'C++ API 服务不可用' });
  });
  
  if (req.body) {
    proxyReq.write(JSON.stringify(req.body));
  }
  
  proxyReq.end();
}

router.get('/stocks', proxyToCpp);
router.get('/stocks/search', proxyToCpp);
router.get(/^\/stocks\/([^\/]+)$/, proxyToCpp);

router.get('/analysis/signals', proxyToCpp);
router.get('/analysis/progress', proxyToCpp);
router.get('/analysis/process', proxyToCpp);
router.get('/analysis/process/strategies', proxyToCpp);
router.get(/^\/analysis\/process\/chart\/([^\/]+)$/, proxyToCpp);
router.get(/^\/analysis\/process\/(\d+)$/, proxyToCpp);
router.get(/^\/analysis\/([^\/]+)$/, proxyToCpp);

router.get(/^\/charts\/([^\/]+)$/, proxyToCpp);

router.get('/user/watchlist', proxyToCpp);
router.post('/user/watchlist', proxyToCpp);
router.delete(/^\/user\/watchlist\/([^\/]+)$/, proxyToCpp);
router.post('/user/subscribe', proxyToCpp);
router.delete(/^\/user\/subscribe\/([^\/]+)$/, proxyToCpp);

router.post('/analysis/process', proxyToCpp);
router.delete('/analysis/process/expired', proxyToCpp);

module.exports = router;