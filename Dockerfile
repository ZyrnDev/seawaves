FROM ubuntu:latest

RUN apt-get update && apt-get install -yq \
    curl \
    build-essential \
    clang

WORKDIR /app
COPY . /app/

ARG RAYLIB_VERSION=5.5
RUN curl -fsSL https://github.com/raysan5/raylib/releases/download/${RAYLIB_VERSION}/raylib-${RAYLIB_VERSION}_linux_amd64.tar.gz \
    | tar -xz --strip-components=1 -C /usr/local \
        raylib-${RAYLIB_VERSION}_linux_amd64/lib/ \
        raylib-${RAYLIB_VERSION}_linux_amd64/include/
RUN make
