#ifndef VEHICLETRACKER_H
#define VEHICLETRACKER_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <map>

struct TrackedObject {
    int id;
    cv::Rect bbox;
    cv::Point centroid;
    int framesLost;
};

class VehicleTracker {
public:
    VehicleTracker(const std::string& modelPath, const std::string& configPath);
    void detectAndTrack(cv::Mat& frame);
    const std::vector<TrackedObject>& getTrackedObjects() const;
    void drawTracks(cv::Mat& frame) const;

private:
    cv::dnn::Net m_net;
    std::vector<TrackedObject> m_trackedObjects;
    int m_nextId;
    int m_maxFramesLost;
    float m_confThreshold;
    float m_nmsThreshold;

    void updateTracks(const std::vector<cv::Rect>& detections);
    double getDistance(const cv::Point& p1, const cv::Point& p2);
};

#endif // VEHICLETRACKER_H
