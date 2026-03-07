const lark = require('@larksuiteoapi/node-sdk');
const dotenv = require('dotenv');

dotenv.config();

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
};

module.exports = config;
