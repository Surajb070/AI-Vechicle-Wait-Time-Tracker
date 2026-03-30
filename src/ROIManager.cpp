#include "ROIManager.h"
#include <spdlog/spdlog.h>

ROIManager::ROIManager() : m_roi(0, 0, 0, 0), m_isSet(false) {}

void ROIManager::setROI(const cv::Rect& roi) {
    m_roi = roi;
    m_isSet = true;
    spdlog::info("ROI set to: [x:{}, y:{}, w:{}, h:{}]", roi.x, roi.y, roi.width, roi.height);
}

bool ROIManager::isInside(const cv::Point& point) const {
    if (!m_isSet) return false;
    return m_roi.contains(point);
}

cv::Rect ROIManager::getROI() const {
    return m_roi;
}

void ROIManager::drawROI(cv::Mat& frame) const {
    if (m_isSet) {
        cv::rectangle(frame, m_roi, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, "ROI", cv::Point(m_roi.x, m_roi.y - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    }
}

void ROIManager::selectROI(const std::string& windowName, const cv::Mat& firstFrame) {
    spdlog::info("Selecting ROI manually. Use mouse to select region.");
    m_roi = cv::selectROI(windowName, firstFrame, false);
    if (m_roi.width > 0 && m_roi.height > 0) {
        m_isSet = true;
        spdlog::info("ROI manually selected: [x:{}, y:{}, w:{}, h:{}]", m_roi.x, m_roi.y, m_roi.width, m_roi.height);
    } else {
        spdlog::warn("No ROI selected. Falling back to default.");
    }
}
