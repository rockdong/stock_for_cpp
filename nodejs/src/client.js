const lark = require('@larksuiteoapi/node-sdk');
const config = require('./config');

const client = new lark.Client({
  appId: config.appId,
  appSecret: config.appSecret,
  appType: lark.AppType.SelfBuild,
  domain: lark.Domain.Feishu,
});

module.exports = client;