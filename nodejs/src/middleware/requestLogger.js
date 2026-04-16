const logger = require('../logger');

function generateRequestId() {
  const timestamp = Date.now();
  const randomStr = Math.random().toString(36).substring(2, 8);
  return `${timestamp}-${randomStr}`;
}

function requestLogger(req, res, next) {
  const requestId = generateRequestId();
  req.requestId = requestId;
  
  const startTime = Date.now();
  const method = req.method;
  const url = req.originalUrl || req.url;
  
  logger.info(`[${requestId}] START ${method} ${url}`);
  
  res.on('finish', () => {
    const duration = Date.now() - startTime;
    const statusCode = res.statusCode;
    
    logger.info(`[${requestId}] END ${statusCode} ${duration}ms`);
    
    if (duration > 5000) {
      logger.warn(`[${requestId}] SLOW_REQUEST ${method} ${url} ${duration}ms`);
    }
    
    if (duration > 10000) {
      logger.warn(`[${requestId}] TIMEOUT_WARNING ${method} ${url} ${duration}ms`);
    }
  });
  
  next();
}

module.exports = requestLogger;