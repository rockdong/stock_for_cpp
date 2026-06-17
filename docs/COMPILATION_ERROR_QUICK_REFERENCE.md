# 编译错误快速诊断清单

**用途**: 快速查找常见编译错误的解决方案  
**使用方法**: 按错误类型查找对应的修复方案

---

## 🔍 快速诊断流程

```
遇到错误
    ↓
检查错误类型（见下方分类）
    ↓
查看对应的诊断步骤
    ↓
实施修复方案
    ↓
验证编译结果
```

---

## 📋 错误分类速查表

### 类型 A: 类型定义问题

#### A1. unknown type name

```cpp
error: unknown type name 'ProgressCallback'
```

**快速诊断**:
```bash
# 1. 查找类型定义位置
grep -rn "using ProgressCallback" .
grep -rn "typedef.*ProgressCallback" .

# 2. 如果没找到，需要定义
# 方案：在使用前添加定义
using ProgressCallback = std::function<void(int, int, int, std::string)>;
```

#### A2. incomplete type / forward declaration

```cpp
error: arithmetic on a pointer to an incomplete type 'Stock'
```

**快速诊断**:
```cpp
// 检查：当前只有前置声明？
class Stock;  // 只有这个？→ 需要 #include "Stock.h"

// 场景判断：
使用容器（vector<Stock>）？     → 必须有完整定义
访问成员（stock->price）？     → 必须有完整定义
使用指针（Stock*）？           → 前置声明足够
```

**修复方案**:
```cpp
// 添加完整定义的包含
#include "core/Stock.h"  // 包含完整定义
```

---

### 类型 B: 访问权限问题

#### B1. private constructor / destructor

```cpp
error: calling a private constructor of class 'Config'
error: calling a private destructor of class 'StrategyManager'
```

**快速诊断**:
```bash
# 检查是否是单例
grep -rn "getInstance" path/to/Config.h
# 如果有 getInstance() → 是单例
```

**判断标准**:
```cpp
class Config {
private:
    Config() = default;   // 私有构造 → 单例特征
    ~Config() = default;  // 私有析构 → 单例特征
    
public:
    static Config& getInstance();  // 单例入口
};
```

**修复方案**:
```cpp
// ❌ 错误
config_ = new Config();
delete config_;

// ✅ 正确
config_ = &Config::getInstance();  // 获取引用
config_ = nullptr;  // 只置空，不 delete
```

---

### 类型 C: 命名空间问题

#### C1. undeclared identifier (namespace)

```cpp
error: use of undeclared identifier 'scheduler'
```

**快速诊断**:
```cpp
// 检查：是否使用了 scheduler::Scheduler？
// 如果是 → 需要先声明命名空间
```

**修复方案**:
```cpp
// 在头文件中添加前置声明
namespace scheduler {
    class Scheduler;
}

// 在源文件中包含完整定义
#include "scheduler/Scheduler.h"
```

---

### 类型 D: 函数调用问题

#### D1. too few arguments

```cpp
error: too few arguments to function call, expected 4, have 3
```

**快速诊断**:
```cpp
// 检查函数签名
void updatePhase2Progress(int total, int completed, int failed, const std::string& status);

// 检查调用
updatePhase2Progress(-1, completed, failed);  // 缺 status
```

**修复方案**:
```cpp
// 补充缺失参数
updatePhase2Progress(-1, completed, failed, "running");
```

#### D2. type mismatch / no viable conversion

```cpp
error: no viable conversion from 'std::string' to 'const char*'
```

**快速诊断**:
```cpp
// 检查函数参数类型
void initialize(const char* path);

// 检查传入类型
std::string configPath = ".env";
initialize(configPath);  // string ≠ const char*
```

**修复方案**:
```cpp
// 类型转换
initialize(configPath.c_str());
```

---

### 类型 E: 头文件路径问题

#### E1. file not found

```cpp
error: '../version.h' file not found
```

**快速诊断**:
```bash
# 检查文件是否存在
ls build/version.h  # CMake 生成的文件在 build 目录

# 检查 CMakeLists.txt 是否包含 build 目录
grep "CMAKE_BINARY_DIR" stock_exe/CMakeLists.txt
```

**修复方案**:
```cmake
# CMakeLists.txt
target_include_directories(my_target PRIVATE
    ${CMAKE_BINARY_DIR}  # 添加 build 目录
)
```

```cpp
// 代码中
#include "version.h"  // 不用相对路径
```

---

### 类型 F: 宏定义问题

#### F1. undeclared identifier (macro)

```cpp
error: use of undeclared identifier 'VERSION'
```

**快速诊断**:
```bash
# 检查宏定义文件
grep -rn "VERSION" build/version.h
# 如果只有 VERSION_STRING → 改用 VERSION_STRING
```

**修复方案**:
```cpp
// ❌ 错误
LOG_INFO("Version: " << VERSION);

// ✅ 正确
LOG_INFO("Version: " << VERSION_STRING);
```

---

## 🎯 诊断决策树

```
编译错误
    │
    ├─ 类型相关？
    │   ├─ "unknown type" → A1 (查找定义)
    │   ├─ "incomplete type" → A2 (包含完整定义)
    │   └─ "undeclared identifier" → C1 (声明命名空间)
    │
    ├─ 访问权限？
    │   └─ "private constructor/destructor" → B1 (检查单例)
    │
    ├─ 函数调用？
    │   ├─ "too few arguments" → D1 (补充参数)
    │   └─ "no viable conversion" → D2 (类型转换)
    │
    ├─ 文件路径？
    │   └─ "file not found" → E1 (检查 CMake 配置)
    │
    └─ 宏定义？
        └─ "undeclared identifier (macro)" → F1 (检查宏定义)
```

---

## 💡 通用诊断命令

### 1. 快速查找定义

```bash
# 查找类定义
grep -rn "class ClassName" .

# 查找类型定义
grep -rn "using TypeName" .
grep -rn "typedef.*TypeName" .

# 查找函数定义
grep -rn "void functionName" .
```

### 2. 查看错误上下文

```bash
# 编译并查看详细错误
make clean
make -j4 2>&1 | tee errors.log

# 查看错误文件
grep "error:" errors.log
```

### 3. 检查单例模式

```bash
# 快速判断是否是单例
grep -n "getInstance\|private.*default" path/to/Class.h
# 如果两个都有 → 是单例
```

---

## 📊 错误修复优先级

| 优先级 | 错误类型 | 修复难度 | 影响范围 |
|--------|---------|---------|---------|
| P0 | 文件路径问题 (E1) | 低 | 小 |
| P0 | 宏定义问题 (F1) | 低 | 小 |
| P1 | 函数参数问题 (D1/D2) | 低 | 小 |
| P1 | 命名空间问题 (C1) | 中 | 中 |
| P2 | 类型定义问题 (A1) | 中 | 中 |
| P2 | 前置声明问题 (A2) | 中 | 中 |
| P3 | 单例访问问题 (B1) | 高 | 大 |

---

## 🔄 修复验证清单

修复后必须验证：

```bash
# 1. 清理旧构建
make clean

# 2. 重新编译
make -j4

# 3. 检查是否有新错误
grep "error:" build.log

# 4. 验证功能
./stock_exe --help  # 确保可执行文件正常

# 5. Git 提交
git add .
git commit -m "Fix: 编译错误修复"
```

---

## 📝 快速修复模板

### 单例修复模板

```cpp
// Before
MySingleton* ptr = new MySingleton();
delete ptr;

// After
MySingleton* ptr = &MySingleton::getInstance();
ptr = nullptr;  // shutdown 时
```

### 前置声明修复模板

```cpp
// Before (只有前置声明)
#include "stock_core.h"  // 只有 class Stock;
std::vector<Stock> stocks;  // 错误

// After (包含完整定义)
#include "stock_core.h"
#include "core/Stock.h"  // 完整定义
std::vector<Stock> stocks;  // 正确
```

### CMake 配置模板

```cmake
# Before
target_include_directories(my_exe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# After
target_include_directories(my_exe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}  # 添加生成文件目录
)
```

---

## 🚨 禁止事项

### ❌ 不要做的事

1. **不要猜测修复**
   ```cpp
   // ❌ 看到错误就随便改改
   // 应该：分析根本原因
   ```

2. **不要堆叠修复**
   ```cpp
   // ❌ 一次改多处
   // 应该：一次只修复一个错误
   ```

3. **不要忽略警告**
   ```cpp
   // ❌ 警告不算错误，忽略
   // 应该：警告也要分析原因
   ```

4. **不要复制粘贴代码**
   ```cpp
   // ❌ 从网上复制代码片段
   // 应该：理解原理后自己实现
   ```

---

## 📞 求助决策

**何时需要求助？**

```
尝试次数 < 3 次 → 继续尝试
尝试次数 ≥ 3 次 → 求助资深开发者

每个修复尝试都要：
1. 记录假设
2. 记录测试结果
3. 分析失败原因
```

---

**使用建议**: 将此清单打印或保存到本地，遇到编译错误时快速查阅对应的解决方案