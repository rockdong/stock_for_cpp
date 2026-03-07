#ifndef ILOGGER_H
#define ILOGGER_H

#include <string>
#include <memory>
#include "LogLevel.h"

namespace logger {

/**
 * @brief 日志接口类
 * 依赖倒置原则：定义抽象接口，具体实现依赖于此接口
 * 开闭原则：对扩展开放，对修改关闭
 */
class ILogger {
public:
    virtual ~ILogger() = default;

    /**
     * @brief 记录 TRACE 级别日志
     * @param message 日志消息
     */
    virtual void trace(const std::string& message) = 0;

    /**
     * @brief 记录 DEBUG 级别日志
     * @param message 日志消息
     */
    virtual void debug(const std::string& message) = 0;

    /**
     * @brief 记录 INFO 级别日志
     * @param message 日志消息
     */
    virtual void info(const std::string& message) = 0;

    /**
     * @brief 记录 WARN 级别日志
     * @param message 日志消息
     */
    virtual void warn(const std::string& message) = 0;

    /**
     * @brief 记录 ERROR 级别日志
     * @param message 日志消息
     */
    virtual void error(const std::string& message) = 0;

    /**
     * @brief 记录 CRITICAL 级别日志
     * @param message 日志消息
     */
    virtual void critical(const std::string& message) = 0;

    /**
     * @brief 设置日志级别
     * @param level 日志级别
     */
    virtual void setLevel(LogLevel level) = 0;

    /**
     * @brief 刷新日志缓冲区
     */
    virtual void flush() = 0;
};

using LoggerPtr = std::shared_ptr<ILogger>;

} // namespace logger

#endif // ILOGGER_H

