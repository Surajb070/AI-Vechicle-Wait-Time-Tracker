# Build Stage
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libopencv-dev \
    libspdlog-dev \
    pkg-config \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Runtime Stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libopencv-core4.5d \
    libopencv-videoio4.5d \
    libopencv-imgproc4.5d \
    libopencv-dnn4.5d \
    libopencv-highgui4.5d \
    libspdlog-dev \
    libgstreamer1.0-0 \
    libgstreamer-plugins-base1.0-0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/wobot_vehicle_timer .
COPY --from=builder /app/build/audio_visualizer .
COPY models/ models/
COPY videos/ videos/

ENTRYPOINT ["./wobot_vehicle_timer"]
CMD ["videos/input.mp4", "videos/output.mp4"]
