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
