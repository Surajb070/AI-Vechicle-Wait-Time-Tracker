#include "TimerManager.h"
#include <iomanip>
#include <sstream>
#include <spdlog/spdlog.h>

TimerManager::TimerManager() {}

void TimerManager::update(int objectId, bool isInside, double fps) {
    if (isInside) {
        if (m_timers.find(objectId) == m_timers.end()) {
            m_timers[objectId] = {0, true};
            spdlog::debug("Started timer for vehicle ID: {}", objectId);
        }
        m_timers[objectId].frameCount++;
        m_timers[objectId].active = true;
    } else {
        if (m_timers.find(objectId) != m_timers.end()) {
            m_timers[objectId].active = false;
            spdlog::debug("Vehicle ID: {} exited ROI.", objectId);
        }
    }
}

std::string TimerManager::getWaitTimeStr(int objectId) const {
    auto it = m_timers.find(objectId);
    if (it != m_timers.end()) {
        double seconds = static_cast<double>(it->second.frameCount) / 30.0; // Assume 30 FPS or use dynamic
        int mm = static_cast<int>(seconds) / 60;
        int ss = static_cast<int>(seconds) % 60;
        std::stringstream ss_stream;
        ss_stream << std::setw(2) << std::setfill('0') << mm << ":"
                  << std::setw(2) << std::setfill('0') << ss;
        return ss_stream.str();
    }
    return "00:00";
}

double TimerManager::getWaitTimeSec(int objectId) const {
    auto it = m_timers.find(objectId);
    if (it != m_timers.end()) {
        return static_cast<double>(it->second.frameCount) / 30.0;
    }
    return 0.0;
}

void TimerManager::reset(int objectId) {
    m_timers.erase(objectId);
}
