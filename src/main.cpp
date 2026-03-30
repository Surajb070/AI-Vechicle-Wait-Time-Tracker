#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include "VideoProcessor.h"
#include "Logger.h"

int main(int argc, char** argv) {
    Logger::init();
    spdlog::info("Wobot AI Vehicle Timer Application started.");

    if (argc < 3) {
        spdlog::error("Usage: {} <input_video> <output_video> [model_path] [config_path]", argv[0]);
        return -1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    std::string modelPath = (argc > 3) ? argv[3] : "models/mobilenet_iter_73000.caffemodel";
    std::string configPath = (argc > 4) ? argv[4] : "models/deploy.prototxt";

    VideoProcessor processor(inputPath, outputPath);
    if (!processor.init(modelPath, configPath)) {
        spdlog::error("Failed to initialize VideoProcessor.");
        return -1;
    }

    try {
        processor.process();
    } catch (const std::exception& e) {
        spdlog::critical("An error occurred during processing: {}", e.what());
        return -1;
    }

    spdlog::info("Application finished successfully.");
    return 0;
}
