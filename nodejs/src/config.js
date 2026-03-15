const lark = require('@larksuiteoapi/node-sdk');
const dotenv = require('dotenv');
const fs = require('fs');
const path = require('path');
const Encryptor = require('./utils/encryptor');
const logger = require('./logger');

// 加载配置并自动解密
function loadConfig() {
  // 首先加载 .env
  dotenv.config();
  
  // 检查是否存在 secret.key，如果存在则尝试解密
  const keyFile = path.join(process.cwd(), 'secret.key');
  if (fs.existsSync(keyFile)) {
    const secretKey = fs.readFileSync(keyFile, 'utf8').trim();
    
    if (secretKey) {
      // 手动读取 .env 文件来获取加密的注释行
      const envPath = path.join(process.cwd(), '.env');
      if (fs.existsSync(envPath)) {
        const envContent = fs.readFileSync(envPath, 'utf8');
        const lines = envContent.split('\n');
        
        for (const line of lines) {
          const trimmedLine = line.trim();
          if (trimmedLine.startsWith('#') && trimmedLine.includes('_ENC=')) {
            const match = trimmedLine.match(/# (\w+)_ENC=(.+)/);
            if (match) {
              const varName = match[1];  // 例如：APP_ID
              const encryptedValue = match[2];  // 加密的值
              try {
                const decryptedValue = Encryptor.decrypt(encryptedValue, secretKey);
                // 设置环境变量
                process.env[varName] = decryptedValue;
              } catch (error) {
                logger.error(`Config: 解密 ${varName} 失败: ` + error.message);
              }
            }
          }
        }
      }
    }
  }
}

// 加载配置
loadConfig();

const client = new lark.Client({
  appId: process.env.APP_ID,
  appSecret: process.env.APP_SECRET,
  appType: lark.AppType.SelfBuild,
  domain: lark.Domain.Feishu,
});

const config = {
  appId: process.env.APP_ID,
  appSecret: process.env.APP_SECRET,
  encryptKey: process.env.ENCRYPT_KEY,
  verificationToken: process.env.VERIFICATION_TOKEN,
  port: parseInt(process.env.PORT) || 3000,
  client,
  dbPath: process.env.DB_PATH || '../stock.db',
};

module.exports = config;
