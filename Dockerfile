FROM ubuntu:22.04

# 元数据标签
LABEL authors="workspace"
LABEL description="Stock for C++ Application with Feishu Bot"

# 设置时区
ENV TZ=Asia/Shanghai
ENV DEBIAN_FRONTEND=noninteractive

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# 安装运行时依赖 (C++)
RUN apt-get update && apt-get install -y --no-install-recommends \
    libsqlite3-0 \
    libssl3 \
    ca-certificates \
    tzdata \
    && rm -rf /var/lib/apt/lists/*

# 安装 Node.js
RUN apt-get update && apt-get install -y --no-install-recommends curl \
    && curl -fsSL https://deb.nodesource.com/setup_20.x | bash - \
    && apt-get install -y nodejs \
    && rm -rf /var/lib/apt/lists/*

# 安装 supervisor (在切换用户之前)
RUN apt-get update && apt-get install -y --no-install-recommends supervisor \
    && rm -rf /var/lib/apt/lists/*

# 创建 supervisor 配置 (在切换用户之前)
RUN mkdir -p /var/log/supervisor
RUN echo '[supervisord]' > /etc/supervisor/conf.d/supervisord.conf && \
    echo 'nodaemon=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '[program:stock-cpp]' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'command=/app/stock_for_cpp' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'directory=/app' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autostart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autorestart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'user=appuser' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '[program:stock-bot]' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'command=node /app/nodejs/src/index.js' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'directory=/app/nodejs' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autostart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autorestart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'user=appuser' >> /etc/supervisor/conf.d/supervisord.conf

# 创建非 root 用户
RUN groupadd -r appgroup && useradd -r -g appgroup appuser

# 创建应用目录和日志目录
RUN mkdir -p /app/logs /app/output /app/nodejs && chown -R appuser:appgroup /app

# 设置工作目录
WORKDIR /app

# 从 GitHub Actions 复制 C++ 可执行文件
COPY --chown=appuser:appgroup cpp/build/stock_for_cpp /app/stock_for_cpp

# 复制 Node.js 文件
COPY --chown=appuser:appgroup nodejs/ /app/nodejs/

# 复制配置文件
COPY --chown=appuser:appgroup cpp/.env /app/.env
COPY --chown=appuser:appgroup nodejs/.env /app/nodejs/.env

# 使用 CI 传入的密钥覆盖敏感配置
ARG DATA_SOURCE_API_KEY
ENV DATA_SOURCE_API_KEY=${DATA_SOURCE_API_KEY}

# 切换到非 root 用户
USER appuser

# 暴露端口
EXPOSE 3000

# 启动 supervisor (会同时启动两个服务)
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]
