#!/bin/bash

# 日志系统文档更新脚本
# 将所有 log:: 命名空间更新为 logger::
# 将所有 log::init(".env") 更新为新的初始化方式

echo "开始更新日志系统文档..."

# 更新 README.md, QUICKSTART.md, USAGE.md 中的命名空间
for file in log/README.md log/QUICKSTART.md log/USAGE.md log/ARCHITECTURE.md log/SUMMARY.md; do
    if [ -f "$file" ]; then
        echo "处理 $file ..."
        
        # 备份文件
        cp "$file" "$file.bak"
        
        # 替换 log:: 为 logger::
        sed -i '' 's/log::/logger::/g' "$file"
        
        # 替换 namespace log 为 namespace logger
        sed -i '' 's/namespace log/namespace logger/g' "$file"
        
        echo "  ✓ 已更新 $file"
    fi
done

echo ""
echo "✅ 文档更新完成！"
echo ""
echo "已更新的文件："
echo "  - log/README.md"
echo "  - log/QUICKSTART.md"
echo "  - log/USAGE.md"
echo "  - log/ARCHITECTURE.md"
echo "  - log/SUMMARY.md"
echo ""
echo "备份文件已保存为 *.bak"
echo ""
echo "请检查更新后的文件，如果没问题可以删除备份："
echo "  rm log/*.bak"

