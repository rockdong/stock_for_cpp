FROM node:20-alpine

WORKDIR /app

COPY node_modules ./node_modules
COPY src ./src
COPY package.json ./

EXPOSE 3000

CMD ["node", "src/index.js"]