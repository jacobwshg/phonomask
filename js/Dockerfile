
# stage 1: build
FROM node:20-alpine AS builder

WORKDIR /
# copy base feature profile
COPY ./lx301-base.csv ./
# copy src files
COPY ./package*.json ./
RUN npm install --production
COPY ./server.js ./worker.js ./phmask.js ./
COPY ./phmask.wasm ./

#RUN npm run build # frontend

# stage 2: deploy
#FROM public.ecr.aws/lambda/nodejs:20
#WORKDIR /
#WORKDIR ${LAMBDA_TASK_ROOT}
#COPY --from=builder /js/node_modules ./node_modules
#COPY --from=builder /js/*.wasm ./
#COPY --from=builder /js/*.js   ./
#COPY --from=builder /js/package*.json ./
RUN npm ci --only=production
ENV NODE_OPTIONS="--experimental-wasm-modules"

EXPOSE 3000

CMD [ "node", "./server.js" ]

