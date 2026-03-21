FROM ubuntu:24.04

LABEL authors="workspace"
LABEL description="Stock for C++ Application with Feishu Bot"

ENV TZ=Asia/Shanghai
ENV DEBIAN_FRONTEND=noninteractive
ENV LOG_LEVEL=INFO

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsqlite3-0 \
    libssl3 \
    ca-certificates \
    tzdata \
    libcairo2 \
    libpango-1.0-0 \
    libpangocairo-1.0-0 \
    libgdk-pixbuf2.0-0 \
    libffi-dev \
    libjpeg-dev \
    libgif-dev \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends curl \
    && curl -fsSL https://deb.nodesource.com/setup_20.x | bash - \
    && apt-get install -y nodejs \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends supervisor \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    python3 \
    python3-pip \
    python3-venv \
    libcairo2-dev \
    libpango1.0-dev \
    libjpeg-dev \
    libgif-dev \
    librsvg2-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --break-system-packages sqlite-web

RUN curl -sSL "https://github.com/amir20/dozzle/releases/latest/download/dozzle_linux_amd64" -o /usr/local/bin/dozzle && \
    chmod +x /usr/local/bin/dozzle

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
    echo 'redirect_stderr=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '[program:sqlite-web]' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'command=sqlite_web /app/stock.db --host 0.0.0.0 --port 8080' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'directory=/app' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autostart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autorestart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'user=appuser' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'stdout_logfile=/dev/stdout' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'stdout_logfile_maxbytes=0' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'redirect_stderr=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo '[program:dozzle]' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'command=/usr/local/bin/dozzle --addr :8888 --base /logs' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'directory=/app' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autostart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'autorestart=true' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'user=root' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'stdout_logfile=/dev/stdout' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'stdout_logfile_maxbytes=0' >> /etc/supervisor/conf.d/supervisord.conf && \
    echo 'redirect_stderr=true' >> /etc/supervisor/conf.d/supervisord.conf

# 创建非 root 用户
RUN groupadd -r appgroup && useradd -r -g appgroup appuser

# 创建应用目录
RUN mkdir -p /app
RUN mkdir -p /app/logs/nodejs

# 设置工作目录
WORKDIR /app

# 从 GitHub Actions 复制 C++ 可执行文件
COPY --chown=appuser:appgroup cpp/build/stock_for_cpp /app/stock_for_cpp

# 复制 Node.js 文件
COPY --chown=appuser:appgroup nodejs/ /app/nodejs/

# 安装 Node.js 依赖
RUN cd /app/nodejs && npm install --production

# 创建默认 .env 文件（运行时通过 docker-compose env_file 覆盖）
RUN touch /app/.env && touch /app/nodejs/.env
RUN chown appuser:appgroup /app/.env /app/nodejs/.env

# 复制启动脚本
COPY --chown=appuser:appgroup docker-entrypoint.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/docker-entrypoint.sh

# 给app目录设置正确的权限
RUN chown -R appuser:appgroup /app

# 保持容器使用默认的root用户来允许supervisord正常工作
# supervisord将以root身份运行并按配置启动各服务

# 暴露端口
EXPOSE 3000 8080 8888

# 启动容器初始化脚本，然后启动supervisor
ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]