#!/bin/bash

# 测试所有客户端运行状态

echo "========================================="
echo "客户端运行测试脚本"
echo "========================================="

cd /Users/kirito/Documents/projects/stock_for_cpp/cpp/build

# 1. 测试 stock_exe
echo ""
echo "1. 测试 stock_exe (命令行客户端)"
echo "-----------------------------------------"

if [ -f stock_exe/stock_exe ]; then
    echo "✅ 可执行文件存在"

    # 测试 --help
    echo "测试 --help 参数:"
    ./stock_exe/stock_exe --help

    # 测试 --once（短时间）
    echo ""
    echo "测试 --once 参数（后台运行，5秒后停止）:"
    ./stock_exe/stock_exe --once &
    EXE_PID=$!
    sleep 5
    kill $EXE_PID 2>/dev/null
    wait $EXE_PID 2>/dev/null

    echo "✅ stock_exe 基本功能测试完成"
else
    echo "❌ stock_exe 可执行文件不存在"
fi

# 2. 测试 stock_tui
echo ""
echo "2. 测试 stock_tui (FTXUI 终端界面)"
echo "-----------------------------------------"

if [ -f stock_tui/stock_tui ]; then
    echo "✅ 可执行文件存在"
    echo "⚠️  TUI 客户端需要交互式终端，无法在此脚本中测试"
    echo "   请在终端中运行: ./stock_tui/stock_tui"
else
    echo "❌ stock_tui 可执行文件不存在"
fi

# 3. 测试 stock_gui
echo ""
echo "3. 测试 stock_gui (Qt 图形界面)"
echo "-----------------------------------------"

if [ -f stock_gui/stock_gui ]; then
    echo "✅ 可执行文件存在"
    echo "⚠️  GUI 客户端需要图形环境，无法在此脚本中测试"
    echo "   请在图形环境中运行: ./stock_gui/stock_gui"
else
    echo "❌ stock_gui 可执行文件不存在"
fi

# 4. 检查配置文件
echo ""
echo "4. 检查配置文件"
echo "-----------------------------------------"

cd /Users/kirito/Documents/projects/stock_for_cpp/cpp

if [ -f .env ]; then
    echo "✅ .env 配置文件存在"
    echo "配置文件内容（前10行）:"
    head -10 .env
else
    echo "❌ .env 配置文件不存在"
fi

# 5. 文件大小统计
echo ""
echo "5. 可执行文件大小统计"
echo "-----------------------------------------"

cd /Users/kirito/Documents/projects/stock_for_cpp/cpp/build

echo "stock_exe: $(ls -lh stock_exe/stock_exe 2>/dev/null | awk '{print $5}')"
echo "stock_tui: $(ls -lh stock_tui/stock_tui 2>/dev/null | awk '{print $5}')"
echo "stock_gui: $(ls -lh stock_gui/stock_gui 2>/dev/null | awk '{print $5}')"

echo ""
echo "========================================="
echo "测试完成"
echo "========================================="