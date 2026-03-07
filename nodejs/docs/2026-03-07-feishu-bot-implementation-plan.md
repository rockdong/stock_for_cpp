# 飞书机器人实现计划

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers/subagent-driven-development to implement this plan task-by-task.

**Goal:** 开发飞书机器人，接收消息并根据关键词自动回复

**Architecture:** 使用 @larksuiteoapi/node-sdk 搭建 HTTP 服务，监听消息事件并回复

**Tech Stack:** Node.js, @larksuiteoapi/node-sdk, Express

---

## Task 1: 初始化项目结构

**Files:**
- Modify: `nodejs/package.json`
- Create: `nodejs/src/index.js`
- Create: `nodejs/src/config.js`
- Create: `nodejs/src/handler.js`
- Create: `nodejs/src/reply.js`
- Create: `nodejs/.env`

**Step 1: 更新 package.json**

```json
{
  "name": "stock-bot",
  "version": "1.0.0",
  "description": "飞书股票分析机器人",
  "main": "src/index.js",
  "scripts": {
    "start": "node src/index.js",
    "dev": "node --watch src/index.js"
  },
  "dependencies": {
    "@larksuiteoapi/node-sdk": "^1.59.0",
    "dotenv": "^16.0.0",
    "express": "^4.18.0"
  }
}
```

**Step 2: 创建 .env**

```
APP_ID=cli_a919f8dcdfb59bca
APP_SECRET=T38zJ6iZxyKNteZi9LWnafpdp6EWpMUR
ENCRYPT_KEY=hVCQ8AfBIkCvBNBuRiihAerXUoLTv03A
VERIFICATION_TOKEN=snlDtNW2S4vCm83iTgil4flSUbNtgvfa
PORT=3000
```

**Step 3: 创建 config.js**

```javascript
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
```

**Step 4: 创建 reply.js（关键词回复逻辑）**

```javascript
const replies = {
  '帮助': `📖 帮助信息

支持的命令：
- 帮助：显示此帮助信息
- hello：测试连接
- 股票：获取股票分析
- 分析：开始分析

欢迎使用股票分析机器人！`,

  'hello': '👋 你好！我是股票分析机器人，已连接成功！',

  '股票': `📈 股票功能

我可以通过以下方式帮助你：
- 分析股票走势
- 获取技术指标
- 执行策略分析

输入"分析"开始分析！`,

  '分析': `🔍 开始分析

正在准备分析功能...
请稍候！`,
};

function getReply(messageText) {
  const text = messageText.trim().toLowerCase();
  
  for (const [key, reply] of Object.entries(replies)) {
    if (text.includes(key)) {
      return reply;
    }
  }
  
  return `🤖 收到消息：${messageText}

输入"帮助"查看可用命令。`;
}

module.exports = { getReply, replies };
```

**Step 5: 创建 handler.js（消息处理）**

```javascript
const { getReply } = require('./reply');
const config = require('./config');

async function handleMessage(event) {
  const message = event.message;
  const messageId = message.message_id;
  const chatId = message.chat_id;
  const messageType = message.message_type;
  
  if (messageType !== 'text') {
    console.log('Ignoring non-text message');
    return;
  }
  
  const textContent = JSON.parse(message.content).text;
  const senderId = message.sender_id;
  
  console.log(`收到消息 from ${senderId?.open_id}: ${textContent}`);
  
  const replyText = getReply(textContent);
  
  try {
    await config.client.im.message.create({
      params: {
        receive_id_type: 'chat_id',
      },
      data: {
        receive_id        msg_type: 'text: chatId,
',
        content: JSON.stringify({ text: replyText }),
      },
    });
    console.log('回复成功');
  } catch (error) {
    console.error('回复失败:', error);
  }
}

module.exports = { handleMessage };
```

**Step 6: 创建 index.js（主入口）**

```javascript
const express = require('express');
const lark = require('@larksuiteoapi/node-sdk');
const config = require('./config');
const { handleMessage } = require('./handler');

const app = express();

app.use(express.json());

app.get('/', (req, res) => {
  res.send('飞书股票分析机器人 running!');
});

app.post('/webhook', async (req, res) => {
  const { type, event } = req.body;
  
  console.log('收到请求:', JSON.stringify(req.body));
  
  if (type === 'url_verification') {
    console.log('URL 验证');
    return res.json({
      challenge: req.body.challenge,
    });
  }
  
  if (type === 'event_callback' && event) {
    if (event.type === 'im.message') {
      const messageEvent = event.message;
      if (messageEvent && messageEvent.message_type === 'text') {
        await handleMessage(event);
      }
    }
  }
  
  res.json({ code: 0 });
});

app.listen(config.port, () => {
  console.log(`机器人服务启动，端口: ${config.port}`);
  console.log(`请将 webhook 地址配置为: http://your-domain.com/webhook`);
});
```

---

## Task 2: 安装依赖

**Step 1: 进入 nodejs 目录并安装**

```bash
cd nodejs
npm install
```

---

## Task 3: 测试运行

**Step 1: 启动服务**

```bash
cd nodejs
npm start
```

**Step 2: 使用 ngrok 内网穿透（开发环境）**

```bash
ngrok http 3000
```

---

## Task 4: 配置飞书开放平台

1. 进入飞书开放平台 → 你的应用 → 事件订阅
2. 添加事件：`im.message.receive_v1`
3. 配置请求网址：`https://your-ngrok-url/webhook`
4. 发布新版本

---

## Task 5: 测试

1. 在飞书中添加机器人为好友
2. 发送消息测试关键词回复
