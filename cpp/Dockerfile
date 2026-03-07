FROM ubuntu:22.04

# 元数据标签
LABEL authors="workspace"
LABEL description="Stock for C++ Application"

# 设置时区
ENV TZ=Asia/Shanghai
ENV DEBIAN_FRONTEND=noninteractive

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# 安装运行时依赖
RUN apt-get update && apt-get install -y --no-install-recommends \
    libsqlite3-0 \
    ca-certificates \
    tzdata \
    && rm -rf /var/lib/apt/lists/*

# 创建非 root 用户
RUN groupadd -r appgroup && useradd -r -g appgroup appuser

# 创建应用目录和日志目录
RUN mkdir -p /app/logs /app/output && chown -R appuser:appgroup /app

# 设置工作目录
WORKDIR /app

# 从 GitHub Actions 复制编译好的可执行文件
COPY --chown=appuser:appgroup build/stock_for_cpp /app/stock_for_cpp

# 复制配置文件
COPY --chown=appuser:appgroup env.example /app/.env

# 切换到非 root 用户
USER appuser

# 暴露端口（如果有 HTTP 服务）
# EXPOSE 8080

# 启动应用
CMD ["/app/stock_for_cpp"]
