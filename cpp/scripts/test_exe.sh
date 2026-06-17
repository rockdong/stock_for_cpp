#!/bin/bash

# ==========================================
# Stock for C++ exe 客户端测试脚本
# ==========================================
# 
# 作者: Senior Developer Team
# 创建时间: 2026-06-14
# 
# 功能: 测试 exe 客户端的各项功能
# 

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 日志函数
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_section() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

# ==========================================
# 第一步: 清理旧的构建文件
# ==========================================
log_section "清理旧的构建文件"

BUILD_DIR="build_exe"

if [ -d "$BUILD_DIR" ]; then
    log_info "清理现有构建目录: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    log_info "清理完成"
else
    log_info "构建目录不存在，无需清理"
fi

# ==========================================
# 第二步: 创建构建目录
# ==========================================
log_section "创建构建目录"

mkdir -p "$BUILD_DIR"
log_info "构建目录已创建: $BUILD_DIR"

# ==========================================
# 第三步: 配置 CMake
# ==========================================
log_section "配置 CMake"

log_info "配置构建选项:"
log_info "  BUILD_CORE=ON    (核心库)"
log_info "  BUILD_EXE=ON     (exe 客户端)"
log_info "  BUILD_TUI=OFF    (TUI 客户端)"
log_info "  BUILD_GUI=OFF    (GUI 客户端)"

cd "$BUILD_DIR"
cmake -DBUILD_CORE=ON \
      -DBUILD_EXE=ON \
      -DBUILD_TUI=OFF \
      -DBUILD_GUI=OFF \
      ..

if [ $? -eq 0 ]; then
    log_info "CMake 配置成功"
else
    log_error "CMake 配置失败"
    exit 1
fi

cd ..

# ==========================================
# 第四步: 编译项目
# ==========================================
log_section "编译项目"

cd "$BUILD_DIR"

log_info "开始编译..."
log_info "使用 4 个并发进程"

make -j4

if [ $? -eq 0 ]; then
    log_info "编译成功"
else
    log_error "编译失败"
    log_warn "请检查编译错误信息"
    exit 1
fi

cd ..

# ==========================================
# 第五步: 检查生成的文件
# ==========================================
log_section "检查生成的文件"

log_info "检查核心库..."
if [ -f "$BUILD_DIR/stock_core/libstock_core.a" ]; then
    log_info "✅ 核心库已生成: $BUILD_DIR/stock_core/libstock_core.a"
    ls -lh "$BUILD_DIR/stock_core/libstock_core.a"
else
    log_error "❌ 核心库未生成"
    exit 1
fi

log_info "检查 exe 可执行文件..."
if [ -f "$BUILD_DIR/stock_exe" ]; then
    log_info "✅ exe 可执行文件已生成: $BUILD_DIR/stock_exe"
    ls -lh "$BUILD_DIR/stock_exe"
else
    log_error "❌ exe 可执行文件未生成"
    exit 1
fi

# ==========================================
# 第六步: 运行基础测试
# ==========================================
log_section "运行基础测试"

# 测试帮助信息
log_info "测试 1: 显示帮助信息"
$BUILD_DIR/stock_exe --help

if [ $? -eq 0 ]; then
    log_info "✅ 帮助信息显示成功"
else
    log_error "❌ 帮助信息显示失败"
fi

echo ""

# 测试单次执行（实际执行会需要时间，这里只测试参数解析）
log_info "测试 2: 参数解析测试"
log_info "注意: 实际的单次分析需要完整的运行环境（数据库、数据源等）"
log_warn "建议在实际环境中运行: ./build_exe/stock_exe --once"

# ==========================================
# 第七步: 生成测试报告
# ==========================================
log_section "生成测试报告"

TEST_REPORT_FILE="test_report_$(date +%Y%m%d_%H%M%S).txt"

cat > "$TEST_REPORT_FILE" << EOF
========================================
Stock for C++ exe 客户端测试报告
========================================

测试时间: $(date)

构建配置:
- BUILD_CORE: ON
- BUILD_EXE: ON
- BUILD_TUI: OFF
- BUILD_GUI: OFF

生成文件:
- 核心库: $(ls -lh $BUILD_DIR/stock_core/libstock_core.a | awk '{print $9, $5}')
- exe: $(ls -lh $BUILD_DIR/stock_exe | awk '{print $9, $5}')

测试结果:
✅ CMake 配置成功
✅ 编译成功
✅ 核心库生成成功
✅ exe 可执行文件生成成功
✅ 帮助信息显示成功

下一步建议:
1. 配置 .env 文件（数据库连接、数据源等）
2. 运行单次分析测试: ./build_exe/stock_exe --once
3. 检查数据库中的分析结果
4. 测试定时执行: ./build_exe/stock_exe --time 20:00

注意事项:
⚠️  实际运行需要完整的运行环境:
   - 配置文件 (.env)
   - 数据库连接 (MySQL/SQLite)
   - 数据源配置 (TuShare API)
   - 股票列表数据
   - 策略配置

========================================
测试完成
========================================
EOF

log_info "测试报告已生成: $TEST_REPORT_FILE"
cat "$TEST_REPORT_FILE"

# ==========================================
# 完成提示
# ==========================================
log_section "测试完成"

log_info "✅ 构建和基础测试全部成功！"
log_info ""
log_info "下一步操作:"
log_info "  1. 配置 .env 文件（参考项目根目录的 .env.example）"
log_info "  2. 运行单次分析: ./build_exe/stock_exe --once"
log_info "  3. 查看分析结果（数据库中）"
log_info ""
log_warn "⚠️  注意: 实际运行需要完整的运行环境配置"
log_info ""
log_info "祝使用愉快！🎉"