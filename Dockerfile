FROM ubuntu:24.04

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

# 安装 supervisor 
RUN apt-get update && apt-get install -y --no-install-recommends supervisor \
    && rm -rf /var/lib/apt/lists/*

# 创建 supervisor 配置 
RUN mkdir -p /var/log/supervisor
RUN echo '[supervisord]' > /etc/supervisor/conf.d/supervisord.conf && \
    echo 'nodaemon=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'user=root' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '[program:stock-cpp]' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'command=/app/stock_for_cpp' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'directory=/app' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autostart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autorestart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'user=appuser' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'stdout_logfile=/dev/stdout' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'stdout_logfile_maxbytes=0' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'redirect_stderr=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '[program:stock-bot]' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'command=node /app/nodejs/src/index.js' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'directory=/app/nodejs' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autostart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autorestart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'user=appuser' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'stdout_logfile=/dev/stdout' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'stdout_logfile_maxbytes=0' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'redirect_stderr=true' >> /etc/supervisor/conf.d/supervisord.conf

# 创建非 root 用户
RUN groupadd -r appgroup && useradd -r -g appgroup appuser

# 创建应用目录
RUN mkdir -p /app

# 设置工作目录
WORKDIR /app

# 从 GitHub Actions 复制 C++ 可执行文件
COPY --chown=appuser:appgroup cpp/build/stock_for_cpp /app/stock_for_cpp

# 复制 Node.js 文件
COPY --chown=appuser:appgroup nodejs/ /app/nodejs/

# 安装 Node.js 依赖
RUN cd /app/nodejs && npm install --production

# 复制包含敏感信息的 .env 文件
COPY --chown=appuser:appgroup cpp/.env /app/.env
COPY --chown=appuser:appgroup nodejs/.env /app/nodejs/.env

# 复制数据库文件（如果存在）
COPY --chown=appuser:appgroup stock.db /app/stock.db

# 复制启动脚本
COPY --chown=appuser:appgroup docker-entrypoint.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/docker-entrypoint.sh

# 给app目录设置正确的权限，然后切换到非root用户
RUN chown -R appuser:appgroup /app

# 切换到非 root 用户
USER appuser

# 暴露端口
EXPOSE 3000

# 启动容器初始化脚本，然后启动supervisor
ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]