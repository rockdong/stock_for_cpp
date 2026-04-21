FROM ubuntu:24.04

LABEL authors="workspace"
LABEL description="Stock for C++ Analysis Engine"

ENV TZ=Asia/Shanghai
ENV DEBIAN_FRONTEND=noninteractive
ENV LOG_LEVEL=INFO

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsqlite3-0 \
    libssl3 \
    ca-certificates \
    tzdata \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd -r appgroup && useradd -r -g appgroup appuser

RUN mkdir -p /app

WORKDIR /app

COPY --chown=appuser:appgroup cpp/build/stock_for_cpp /app/stock_for_cpp
RUN chmod +x /app/stock_for_cpp

RUN chown -R appuser:appgroup /app

USER appuser

ENTRYPOINT ["/app/stock_for_cpp"]