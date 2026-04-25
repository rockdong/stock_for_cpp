FROM node:20-alpine AS builder
WORKDIR /app

# 安装 OpenSSL 和构建依赖
RUN apk add --no-cache openssl openssl-dev

COPY package*.json ./
COPY prisma ./prisma/

# 安装依赖并生成 Prisma Client (在 Alpine 环境中)
RUN npm ci
RUN npx prisma generate

COPY . .
RUN npm run build

FROM node:20-alpine
WORKDIR /app

# 安装 OpenSSL 运行时依赖
RUN apk add --no-cache openssl

COPY --from=builder /app/dist ./dist
COPY --from=builder /app/node_modules ./node_modules
COPY --from=builder /app/prisma ./prisma
COPY --from=builder /app/package.json ./

EXPOSE 3001

CMD ["node", "dist/main.js"]