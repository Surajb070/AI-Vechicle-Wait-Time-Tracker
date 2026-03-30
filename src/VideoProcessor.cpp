#include "VideoProcessor.h"
#include <spdlog/spdlog.h>
#include <chrono>

VideoProcessor::VideoProcessor(const std::string& inputPath, const std::string& outputPath)
    : m_inputPath(inputPath), m_outputPath(outputPath) {
    m_roiManager = std::make_unique<ROIManager>();
    m_timerManager = std::make_unique<TimerManager>();
}

bool VideoProcessor::init(const std::string& modelPath, const std::string& configPath) {
    m_cap.open(m_inputPath);
    if (!m_cap.isOpened()) {
        spdlog::error("Could not open input video: {}", m_inputPath);
        return false;
    }

    m_tracker = std::make_unique<VehicleTracker>(modelPath, configPath);
    
    // Set default ROI (example values, can be adjusted)
    int width = static_cast<int>(m_cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(m_cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    m_roiManager->setROI(cv::Rect(width / 4, height / 4, width / 2, height / 2));

    double fps = m_cap.get(cv::CAP_PROP_FPS);
    if (fps <= 0) fps = 30.0;
    
    cv::Size frameSize(width, height);
    m_writer.open(m_outputPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frameSize);
    if (!m_writer.isOpened()) {
        spdlog::error("Could not open output video for writing: {}", m_outputPath);
        return false;
    }

    spdlog::info("VideoProcessor initialized. Input: {}, Output: {}", m_inputPath, m_outputPath);
    return true;
}

void VideoProcessor::process() {
    cv::Mat frame;
    auto start = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    double fps = 0;

    while (m_cap.read(frame)) {
        m_tracker->detectAndTrack(frame);

        for (const auto& track : m_tracker->getTrackedObjects()) {
            bool isInside = m_roiManager->isInside(track.centroid);
            m_timerManager->update(track.id, isInside, 30.0);
            
            if (isInside) {
                std::string waitTime = m_timerManager->getWaitTimeStr(track.id);
                cv::putText(frame, "Wait: " + waitTime,
                            cv::Point(track.bbox.x, track.bbox.y + track.bbox.height + 20),
                            cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 255), 2);
            }
        }

        m_roiManager->drawROI(frame);
        m_tracker->drawTracks(frame);

        frameCount++;
        if (frameCount % 10 == 0) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;
            fps = 10.0 / diff.count();
            start = end;
        }
        
        cv::putText(frame, "FPS: " + std::to_string(static_cast<int>(fps)),
                    cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);

        m_writer.write(frame);
        
        if (frameCount % 100 == 0) {
            spdlog::info("Processed {} frames...", frameCount);
        }
    }
    spdlog::info("Processing complete. Total frames: {}", frameCount);
}
