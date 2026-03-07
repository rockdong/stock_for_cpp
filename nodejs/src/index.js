const lark = require('@larksuiteoapi/node-sdk');
const express = require('express');
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
  console.log(`Webhook 地址: http://localhost:${config.port}/webhook`);
  console.log('注意：需要使用 ngrok 将此地址暴露到公网');
});
