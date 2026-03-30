#include "VehicleTracker.h"
#include <spdlog/spdlog.h>
#include <cmath>

VehicleTracker::VehicleTracker(const std::string& modelPath, const std::string& configPath)
    : m_nextId(1), m_maxFramesLost(10), m_confThreshold(0.5f), m_nmsThreshold(0.4f) {
    try {
        m_net = cv::dnn::readNet(modelPath, configPath);
        m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        spdlog::info("DNN Model loaded successfully from: {}", modelPath);
    } catch (const cv::Exception& e) {
        spdlog::error("Error loading DNN model: {}", e.what());
    }
}

void VehicleTracker::detectAndTrack(cv::Mat& frame) {
    if (m_net.empty()) return;

    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1.0 / 127.5, cv::Size(300, 300), cv::Scalar(127.5, 127.5, 127.5), true, false);
    m_net.setInput(blob);
    cv::Mat detections = m_net.forward();

    std::vector<cv::Rect> currentDetections;
    float* data = (float*)detections.data;
    for (int i = 0; i < detections.total(); i += 7) {
        float confidence = data[i + 2];
        if (confidence > m_confThreshold) {
            int classId = (int)data[i + 1];
            // MobileNet-SSD class IDs: 7 is car, 6 is bus, 14 is motorbike
            if (classId == 7 || classId == 6 || classId == 14) {
                int left = static_cast<int>(data[i + 3] * frame.cols);
                int top = static_cast<int>(data[i + 4] * frame.rows);
                int right = static_cast<int>(data[i + 5] * frame.cols);
                int bottom = static_cast<int>(data[i + 6] * frame.rows);
                currentDetections.push_back(cv::Rect(left, top, right - left, bottom - top));
            }
        }
    }

    updateTracks(currentDetections);
}

void VehicleTracker::updateTracks(const std::vector<cv::Rect>& detections) {
    std::vector<cv::Point> currentCentroids;
    for (const auto& rect : detections) {
        currentCentroids.push_back(cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2));
    }

    std::vector<TrackedObject> nextTracks;
    std::vector<bool> usedDetections(detections.size(), false);

    for (auto& track : m_trackedObjects) {
        int bestDetectionIdx = -1;
        double minDistance = 50.0; // Distance threshold for matching

        for (size_t i = 0; i < currentCentroids.size(); ++i) {
            if (usedDetections[i]) continue;
            double dist = getDistance(track.centroid, currentCentroids[i]);
            if (dist < minDistance) {
                minDistance = dist;
                bestDetectionIdx = static_cast<int>(i);
            }
        }

        if (bestDetectionIdx != -1) {
            track.bbox = detections[bestDetectionIdx];
            track.centroid = currentCentroids[bestDetectionIdx];
            track.framesLost = 0;
            usedDetections[bestDetectionIdx] = true;
            nextTracks.push_back(track);
        } else {
            track.framesLost++;
            if (track.framesLost <= m_maxFramesLost) {
                nextTracks.push_back(track);
            }
        }
    }

    for (size_t i = 0; i < detections.size(); ++i) {
        if (!usedDetections[i]) {
            TrackedObject newTrack;
            newTrack.id = m_nextId++;
            newTrack.bbox = detections[i];
            newTrack.centroid = currentCentroids[i];
            newTrack.framesLost = 0;
            nextTracks.push_back(newTrack);
        }
    }

    m_trackedObjects = nextTracks;
}

double VehicleTracker::getDistance(const cv::Point& p1, const cv::Point& p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

const std::vector<TrackedObject>& VehicleTracker::getTrackedObjects() const {
    return m_trackedObjects;
}

void VehicleTracker::drawTracks(cv::Mat& frame) const {
    for (const auto& track : m_trackedObjects) {
        cv::rectangle(frame, track.bbox, cv::Scalar(255, 0, 0), 2);
        cv::putText(frame, "ID: " + std::to_string(track.id),
                    cv::Point(track.bbox.x, track.bbox.y - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);
    }
}
