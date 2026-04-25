FROM node:20-slim

WORKDIR /app

# 安装 canvas 运行时依赖
RUN apt-get update && apt-get install -y --no-install-recommends \
    libcairo2 \
    libpango-1.0-0 \
    libjpeg62-turbo \
    libgif7 \
    librsvg2-2 \
    && rm -rf /var/lib/apt/lists/*

COPY node_modules ./node_modules
COPY src ./src
COPY package.json ./

EXPOSE 3000

CMD ["node", "src/index.js"]