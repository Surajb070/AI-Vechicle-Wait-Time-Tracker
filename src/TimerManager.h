#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include <map>
#include <string>
#include <chrono>

class TimerManager {
public:
    TimerManager();
    void update(int objectId, bool isInside, double fps);
    std::string getWaitTimeStr(int objectId) const;
    double getWaitTimeSec(int objectId) const;
    void reset(int objectId);

private:
    struct ObjectTimer {
        long long frameCount;
        bool active;
    };
    std::map<int, ObjectTimer> m_timers;
};

#endif // TIMERMANAGER_H
