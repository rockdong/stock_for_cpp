const dotenv = require('dotenv');

dotenv.config();

const config = {
  appId: process.env.FEISHU_APP_ID || process.env.APP_ID,
  appSecret: process.env.FEISHU_APP_SECRET || process.env.APP_SECRET,
  encryptKey: process.env.FEISHU_ENCRYPT_KEY || process.env.ENCRYPT_KEY,
  verificationToken: process.env.FEISHU_VERIFICATION_TOKEN || process.env.VERIFICATION_TOKEN,
  port: parseInt(process.env.PORT) || 3000,
  dbPath: process.env.DB_PATH || './stock.db',
};

module.exports = config;