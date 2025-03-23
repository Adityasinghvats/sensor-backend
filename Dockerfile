FROM node:20-bullseye

WORKDIR /src/app

COPY package*.json ./

RUN npm install

COPY . .

COPY .env .env

EXPOSE 3000

CMD ["node", "server.js"]