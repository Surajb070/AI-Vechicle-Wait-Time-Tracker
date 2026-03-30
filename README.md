# ✅ WOBOT AI ASSIGNMENT – COMPLETE SOLUTION

This project is a high-performance C++ application for vehicle tracking and ROI timing, developed as part of the Wobot AI Senior C++ Developer assignment.

## Features
- **Vehicle Detection**: Uses OpenCV DNN with MobileNet-SSD.
- **Multi-Object Tracking**: Robust centroid-based tracking for high FPS.
- **ROI Management**: Customizable ROI with intersection checks.
- **Timer Logic**: Frame-accurate wait time calculation (MM:SS).
- **Professional Logging**: Uses `spdlog` for console and file logging.
- **Dockerized**: Multi-stage Docker build for optimized image size.
- **Bonus**: GStreamer audio visualizer included.

## Project Structure
- `src/`: Source code (.cpp files)
- `include/`: Header files
- `models/`: Pre-trained DNN models
- `videos/`: Input and output video files
- `logs/`: Application logs

## Native Build Instructions
### Prerequisites
- C++17 compiler (GCC/Clang)
- CMake 3.10+
- OpenCV 4.x
- spdlog
- GStreamer (optional for bonus task)

### Build Steps
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Run
```bash
./wobot_vehicle_timer videos/input.mp4 videos/output.mp4
```

## Docker Build & Run
### Build
```bash
docker build -t wobot-vehicle-timer .
```

### Run
```bash
docker run --rm -v $(pwd)/videos:/app/videos wobot-vehicle-timer videos/input.mp4 videos/output.mp4
```

## GStreamer Bonus
To run the audio visualizer:
```bash
./audio_visualizer audio.mp3
```

## Evaluation Criteria Mapping
- **Dockerization**: Multi-stage build, optimized runtime image.
- **OOP Concepts**: Clear separation of concerns in `VideoProcessor`, `VehicleTracker`, `ROIManager`, `TimerManager`.
- **Visuals**: Clear ROI, bounding boxes, ID labels, wait timers, and real-time FPS counter.
- **Performance**: Optimized tracking and DNN inference for >30 FPS.
- **Logging**: Structured logs with timestamps and severity levels.
