#!/bin/bash
# 段错误调试脚本
# 使用 lldb 定位段错误位置

echo "========================================="
echo "Stock TUI 段错误调试"
echo "========================================="

cd /Users/kirito/Documents/projects/stock_for_cpp/cpp/build

echo "1. 启动 lldb 调试器..."
lldb -s /dev/stdin ./stock_tui/stock_tui <<'LLDB_CMDS'
run
bt
frame select 0
p this
p config_
p database_
quit
LLDB_CMDS

echo ""
echo "========================================="
echo "调试完成"
echo "========================================="