#ifndef CORE_STRATEGY_H
#define CORE_STRATEGY_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "Stock.h"

namespace core {

/**
 * @brief 交易信号枚举
 */
enum class Signal {
    BUY,        // 买入信号
    SELL,       // 卖出信号
    HOLD,       // 持有信号
    NONE        // 无信号
};

/**
 * @brief 信号强度枚举
 */
enum class SignalStrength {
    WEAK,       // 弱信号
    MEDIUM,     // 中等信号
    STRONG      // 强信号
};

/**
 * @brief 交易信号结构
 */
struct TradeSignal {
    std::string tsCode;         // 股票代码
    Signal signal;              // 信号类型
    SignalStrength strength;    // 信号强度
    double price;               // 建议价格
    int quantity;               // 建议数量
    std::string reason;         // 信号原因
    std::string timestamp;      // 信号时间
    
    TradeSignal()
        : signal(Signal::NONE)
        , strength(SignalStrength::WEAK)
        , price(0.0)
        , quantity(0)
    {}
    
    std::string toString() const;
};

/**
 * @brief 策略接口
 * 
 * 定义交易策略的统一接口
 * 遵循开闭原则（OCP）和依赖倒置原则（DIP）
 */
class IStrategy {
public:
    virtual ~IStrategy() = default;
    
    /**
     * @brief 获取策略名称
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief 获取策略描述
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * @brief 分析并生成交易信号
     * @param tsCode 股票代码
     * @param data 历史数据
     * @return 交易信号
     */
    virtual TradeSignal analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) = 0;
    
    /**
     * @brief 设置策略参数
     * @param params 参数映射
     */
    virtual void setParameters(const std::map<std::string, double>& params) = 0;
    
    /**
     * @brief 获取策略参数
     */
    virtual std::map<std::string, double> getParameters() const = 0;
    
    /**
     * @brief 验证策略参数
     */
    virtual bool validateParameters() const = 0;
};

using StrategyPtr = std::shared_ptr<IStrategy>;

/**
 * @brief 策略基类
 * 
 * 提供策略的通用实现
 * 遵循模板方法模式
 */
class StrategyBase : public IStrategy {
public:
    explicit StrategyBase(const std::string& name, const std::string& description);
    
    std::string getName() const override { return name_; }
    std::string getDescription() const override { return description_; }
    
    void setParameters(const std::map<std::string, double>& params) override;
    std::map<std::string, double> getParameters() const override;
    
    bool validateParameters() const override;

protected:
    std::string name_;
    std::string description_;
    std::map<std::string, double> parameters_;
    
    /**
     * @brief 获取参数值
     * @param key 参数名
     * @param defaultValue 默认值
     */
    double getParameter(const std::string& key, double defaultValue = 0.0) const;
    
    /**
     * @brief 设置参数值
     */
    void setParameter(const std::string& key, double value);
    
    /**
     * @brief 检查数据是否足够
     */
    bool hasEnoughData(const std::vector<StockData>& data, size_t minSize) const;
    
    /**
     * @brief 创建交易信号
     */
    TradeSignal createSignal(
        const std::string& tsCode,
        Signal signal,
        SignalStrength strength,
        double price,
        int quantity,
        const std::string& reason
    ) const;
};

} // namespace core

#endif // CORE_STRATEGY_H

