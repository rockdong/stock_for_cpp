FROM node:20-alpine
WORKDIR /app

RUN apk add --no-cache openssl openssl-dev

COPY dist ./dist
COPY node_modules ./node_modules
COPY prisma ./prisma
COPY package.json ./

EXPOSE 3001

CMD ["node", "dist/main.js"]