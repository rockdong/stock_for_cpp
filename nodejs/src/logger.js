const winston = require('winston');
const path = require('path');
const fs = require('fs');

const LOG_DIR = process.env.LOG_DIR || '/app/logs/nodejs';
const LOG_LEVEL = process.env.LOG_LEVEL || 'INFO';
const NODE_ENV = process.env.NODE_ENV || 'development';

if (!fs.existsSync(LOG_DIR)) {
  fs.mkdirSync(LOG_DIR, { recursive: true });
}

const logFormat = winston.format.combine(
  winston.format.timestamp({ format: 'YYYY-MM-DD HH:mm:ss' }),
  winston.format.printf(({ timestamp, level, message }) => {
    return `${timestamp} [${level.toUpperCase()}] ${message}`;
  })
);

const transports = [
  new winston.transports.File({
    filename: path.join(LOG_DIR, 'bot.log'),
    maxsize: 10485760,
    maxFiles: 3,
    format: logFormat
  })
];

if (NODE_ENV !== 'production') {
  transports.push(
    new winston.transports.Console({
      format: logFormat
    })
  );
}

const logger = winston.createLogger({
  level: LOG_LEVEL.toLowerCase(),
  transports: transports
});

module.exports = logger;