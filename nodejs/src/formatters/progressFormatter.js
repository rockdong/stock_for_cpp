const { buildRichTextCard } = require('../messageBuilder');

function formatProgressCard(progress) {
  if (!progress) {
    return {
      msg_type: 'interactive',
      card: {
        config: { wide_screen_mode: true },
        header: {
          title: { tag: 'plain_text', content: '📊 分析进度' },
          template: 'grey'
        },
        elements: [
          { tag: 'div', text: { tag: 'lark_md', content: '❌ 无法获取分析进度' } }
        ]
      }
    };
  }

  const statusText = { 'idle': '⚪ 空闲', 'running': '🔄 运行中', 'completed': '✅ 已完成' };
  const status = statusText[progress.status] || progress.status;
  const total = progress.total || 0;
  const completed = progress.completed || 0;
  const failed = progress.failed || 0;
  const percent = total > 0 ? Math.round((completed / total) * 100) : 0;

  let elapsed = '';
  if (progress.started_at) {
    const start = new Date(progress.started_at);
    const now = progress.status === 'running' ? new Date() : new Date(progress.updated_at);
    const diffMs = now - start;
    const minutes = Math.floor(diffMs / 60000);
    const seconds = Math.floor((diffMs % 60000) / 1000);
    elapsed = `${minutes}分${seconds}秒`;
  }

  const progressBar = '█'.repeat(Math.round(percent / 5)) + '░'.repeat(20 - Math.round(percent / 5));

  const content = `**状态:** ${status}\n\n` +
    `**进度:** ${completed} / ${total} (${percent}%)\n` +
    `\`${progressBar}\` ${percent}%\n\n` +
    `✅ 成功: ${completed - failed}  |  ❌ 失败: ${failed}` +
    (progress.started_at ? `\n\n⏱️ 已运行: ${elapsed}` : '');

  return {
    msg_type: 'interactive',
    card: {
      config: { wide_screen_mode: true },
      header: {
        title: { tag: 'plain_text', content: '📊 分析进度' },
        template: progress.status === 'running' ? 'green' : 'blue'
      },
      elements: [
        { tag: 'div', text: { tag: 'lark_md', content: content } },
        { tag: 'hr' },
        {
          tag: 'action',
          actions: [
            {
              tag: 'button',
              text: { tag: 'plain_text', content: '🔄 刷新' },
              type: 'primary',
              action_id: 'refresh_progress',
              value: JSON.stringify({ action: 'refresh' })
            }
          ]
        }
      ]
    }
  };
}

module.exports = {
  formatProgressCard,
};