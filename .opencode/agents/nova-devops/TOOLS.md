# TOOLS.md - Nova's Toolkit

## CI/CD 工具链
1. **流水线平台**
   - GitHub Actions (推荐)
   - GitLab CI
   - Jenkins (企业级)

2. **容器化**
   - Docker
   - Docker Compose (本地)
   - Kubernetes (生产)
   - Helm (K8s 包管理)

3. **IaC 工具**
   - Terraform
   - Pulumi
   - CloudFormation (AWS)

## 部署流程
```
1. 代码提交 → 触发 CI
2. 安装依赖 → 构建
3. 运行测试 → 质量门禁
4. 构建镜像 → 推送仓库
5. 部署到环境 → 健康检查
6. 通知团队 → 完成
```

## 目录结构标准
```
.github/
└── workflows/         # GitHub Actions
    ├── ci.yml
    ├── cd.yml
    └── security.yml

docker/
├── Dockerfile         # 应用镜像
└── docker-compose.yml # 本地开发

k8s/
├── base/              # 基础配置
└── overlays/          # 环境配置

terraform/
├── modules/           # 可复用模块
└── environments/      # 环境配置
```

## 监控配置
| 类型 | 工具 | 用途 |
|------|------|------|
| 指标 | Prometheus + Grafana | 系统监控 |
| 日志 | Loki / ELK | 日志聚合 |
| 追踪 | Jaeger | 分布式追踪 |
| 错误 | Sentry | 错误监控 |
| 告警 | AlertManager | 告警通知 |

## 安全清单
- [ ] 密钥管理 (Vault/Secrets Manager)
- [ ] 镜像安全扫描
- [ ] 网络隔离 (VPC/安全组)
- [ ] SSL/TLS 证书
- [ ] 访问控制 (IAM)
- [ ] 日志脱敏

## 常用命令
```bash
# Docker 构建
docker build -t app:latest .

# Docker 运行
docker-compose up -d

# K8s 部署
kubectl apply -f k8s/

# Terraform
terraform plan
terraform apply
```