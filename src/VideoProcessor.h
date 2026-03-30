#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <memory>
#include "VehicleTracker.h"
#include "ROIManager.h"
#include "TimerManager.h"

class VideoProcessor {
public:
    VideoProcessor(const std::string& inputPath, const std::string& outputPath);
    bool init(const std::string& modelPath, const std::string& configPath);
    void process();

private:
    std::string m_inputPath;
    std::string m_outputPath;
    cv::VideoCapture m_cap;
    cv::VideoWriter m_writer;
    std::unique_ptr<VehicleTracker> m_tracker;
    std::unique_ptr<ROIManager> m_roiManager;
    std::unique_ptr<TimerManager> m_timerManager;

    void drawVisuals(cv::Mat& frame, double fps);
};

#endif // VIDEOPROCESSOR_H
