cmake_minimum_required(VERSION 3.10)
project(wobot_vehicle_timer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(spdlog REQUIRED)

include_directories(include src ${OpenCV_INCLUDE_DIRS})

set(SOURCES
    src/main.cpp
    src/VideoProcessor.cpp
    src/VehicleTracker.cpp
    src/ROIManager.cpp
    src/TimerManager.cpp
)

add_executable(wobot_vehicle_timer ${SOURCES})

target_link_libraries(wobot_vehicle_timer PRIVATE ${OpenCV_LIBS} spdlog::spdlog)

# Optional GStreamer Bonus
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(GSTREAMER QUIET gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0 gstreamer-audio-1.0)
    if(GSTREAMER_FOUND)
        add_executable(audio_visualizer src/audio_visualizer.cpp)
        target_include_directories(audio_visualizer PRIVATE ${GSTREAMER_INCLUDE_DIRS})
        target_link_libraries(audio_visualizer PRIVATE ${GSTREAMER_LIBRARIES})
        message(STATUS "GStreamer found. Audio visualizer target added.")
    endif()
endif()
