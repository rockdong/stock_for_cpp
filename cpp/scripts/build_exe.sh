#!/bin/bash

# 构建 exe 客户端 (命令行执行)
# 适合服务器部署，无 UI 依赖

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPP_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${CPP_DIR}/build_exe"

echo "========================================"
echo "构建 exe 客户端 (命令行执行)"
echo "========================================"
echo "源码目录: ${CPP_DIR}"
echo "构建目录: ${BUILD_DIR}"
echo "========================================"

# 创建构建目录
mkdir -p "${BUILD_DIR}"

# 进入构建目录
cd "${BUILD_DIR}"

# 配置 CMake
cmake \
    -DBUILD_CORE=ON \
    -DBUILD_TUI=OFF \
    -DBUILD_GUI=OFF \
    -DBUILD_EXE=ON \
    -DBUILD_LEGACY=OFF \
    "${CPP_DIR}"

# 编译
make -j$(nproc)

echo "========================================"
echo "构建完成"
echo "========================================"
echo "可执行文件: ${BUILD_DIR}/stock_exe"
echo "核心库: ${BUILD_DIR}/stock_core/libstock_core.a"
echo "========================================"

# 返回原目录
cd "${CPP_DIR}"