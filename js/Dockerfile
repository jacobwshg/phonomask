
# stage 1: build
FROM node:20-alpine AS builder
WORKDIR /app
COPY package*.json ./
RUN npm ci
COPY *.js ./
COPY *.wasm ./
#RUN npm run build # frontend

# stage 2: deploy
#FROM public.ecr.aws/lambda/nodejs:20
#WORKDIR /
#WORKDIR ${LAMBDA_TASK_ROOT}
#COPY --from=builder /app/node_modules ./node_modules
#COPY --from=builder /app/*.wasm ./
#COPY --from=builder /app/*.js   ./
#COPY --from=builder /app/package*.json ./
RUN npm ci --only=production
ENV NODE_OPTIONS="--experimental-wasm-modules"

EXPOSE 3000

CMD [ "node", "./server.js" ]

