const logger = require('../logger');

// 导入所有命令模块（触发 register）
require('./stock');
require('./analysis');
require('./chart');
require('./progress');
require('./help');

const commands = new Map();
const commandsList = [];

function register(prefix, handler, description = '') {
  commands.set(prefix, handler);
  commandsList.push({ prefix, description });
  logger.debug(`注册命令: ${prefix}`);
}

function route(text) {
  const trimmedText = text.trim();
  
  for (const [prefix, handler] of commands) {
    if (trimmedText === prefix || trimmedText.startsWith(prefix + ' ')) {
      logger.debug(`匹配命令: ${prefix}`);
      return handler(trimmedText);
    }
  }
  
  return null;
}

function getCommandsList() {
  return commandsList;
}

module.exports = {
  register,
  route,
  getCommandsList,
};