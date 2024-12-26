# Temel imaj
FROM ubuntu:22.04

# Gerekli bağımlılıkları yükle
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    git \
    cmake \
    libx11-dev \
    libxcursor-dev \
    libxrandr-dev \
    libxi-dev \
    libxinerama-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    && apt-get clean

# Raylib'i kaynaktan derleyip yükle
RUN git clone https://github.com/raysan5/raylib.git /tmp/raylib && \
    cd /tmp/raylib && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install && \
    rm -rf /tmp/raylib

# Çalışma dizini ayarla
WORKDIR /app
COPY . .

# Derleme komutunu çalıştır
RUN g++ AITermProject.cpp -o AITermProject -lraylib

ENTRYPOINT ["./AITermProject"]

# docker run -e DISPLAY=:0 -v /tmp/.X11-unix:/tmp/.X11-unix --net=host aitermproject
