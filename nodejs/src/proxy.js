const axios = require('axios');
const logger = require('./logger');

const CPP_API_URL = process.env.CPP_API_URL || 'http://localhost:3001';
const CPP_API_TIMEOUT = parseInt(process.env.CPP_API_TIMEOUT) || 30000;

async function proxyRequest(req, res, endpoint) {
    const startTime = Date.now();
    
    try {
        const response = await axios({
            method: req.method,
            url: `${CPP_API_URL}${endpoint}`,
            params: req.query,
            data: req.body,
            timeout: CPP_API_TIMEOUT,
            headers: { 'Content-Type': 'application/json' }
        });
        
        const duration = Date.now() - startTime;
        logger.info(`Proxy ${req.method} ${endpoint} - ${response.status} (${duration}ms)`);
        res.json(response.data);
        
    } catch (error) {
        const duration = Date.now() - startTime;
        
        if (error.code === 'ECONNREFUSED') {
            logger.error(`Proxy ${endpoint} - C++ API unavailable (${duration}ms)`);
            res.status(503).json({ success: false, error: 'C++ API service unavailable' });
        } else if (error.code === 'ETIMEDOUT' || error.message.includes('timeout')) {
            logger.error(`Proxy ${endpoint} - timeout (${duration}ms)`);
            res.status(504).json({ success: false, error: 'Request timeout' });
        } else if (error.response) {
            logger.error(`Proxy ${endpoint} - C++ API error: ${error.response.status}`);
            res.status(error.response.status).json(error.response.data);
        } else {
            logger.error(`Proxy ${endpoint} - error: ${error.message}`);
            res.status(500).json({ success: false, error: error.message });
        }
    }
}

module.exports = { proxyRequest, CPP_API_URL };