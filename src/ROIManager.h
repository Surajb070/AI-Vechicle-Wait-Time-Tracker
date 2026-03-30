#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include <opencv2/opencv.hpp>
#include <string>

class ROIManager {
public:
    ROIManager();
    void setROI(const cv::Rect& roi);
    bool isInside(const cv::Point& point) const;
    cv::Rect getROI() const;
    void drawROI(cv::Mat& frame) const;
    void selectROI(const std::string& windowName, const cv::Mat& firstFrame);

private:
    cv::Rect m_roi;
    bool m_isSet;
};

#endif // ROIMANAGER_H
