#!/bin/bash
# AI 团队 Skills 安装脚本
# 来源: https://skillhub.tencent.com/skills

echo "🚀 开始安装 AI 团队所需的 Skills..."
echo ""

# 计数器
total=0
success=0
failed=0

# 安装函数
install_skill() {
    local skill=$1
    echo "📦 安装: $skill"
    if skillhub install "$skill" 2>/dev/null; then
        ((success++))
        echo "  ✅ $skill 安装成功"
    else
        ((failed++))
        echo "  ❌ $skill 安装失败"
    fi
    ((total++))
}

echo "=== 通用 Skills ==="
install_skill "agent-browser"
install_skill "multi-search-engine"
install_skill "baidu-search"
install_skill "ontology"
install_skill "github"
install_skill "summarize"

echo ""
echo "=== 飞书 Skills ==="
install_skill "feishu-docs"
install_skill "feishu-doc-manager"
install_skill "feishu-media"
install_skill "feishu-img-msg"

echo ""
echo "=== 开发工具 Skills ==="
install_skill "code"
install_skill "codeconductor"
install_skill "ui-ux-pro-max"
install_skill "frontend-design-ultimate"

echo ""
echo "=== 测试 Skills ==="
install_skill "auto-testcase-generator"
install_skill "browser-use"
install_skill "playwright-scraper-skill"

echo ""
echo "=== 运维监控 Skills ==="
install_skill "vercel"
install_skill "security-audit"
install_skill "moltguard"
install_skill "crypto-gold-monitor"
install_skill "blogwatcher"
install_skill "stock-analysis"

echo ""
echo "=== 文档工具 Skills ==="
install_skill "convert-markdown"

echo ""
echo "======================================"
echo "📊 安装统计:"
echo "  总计: $total"
echo "  成功: $success"
echo "  失败: $failed"
echo "======================================"

if [ $failed -eq 0 ]; then
    echo "✨ 所有 Skills 安装完成！"
else
    echo "⚠️ 部分安装失败，请检查网络连接或手动安装"
fi