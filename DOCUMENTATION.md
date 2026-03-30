# DOCUMENTATION - Wobot AI Senior C++ Developer Assignment

## 1. Robust Timer Calculation Algorithm

The wait time for each vehicle is calculated based on its tracked presence within the defined Region of Interest (ROI).

### Algorithm Steps:
1.  **Centroid Calculation**: For each detected vehicle bounding box, calculate the centroid $(x_c, y_c)$.
2.  **Intersection Check**: Check if $(x_c, y_c)$ is inside the ROI rectangle.
3.  **Frame Accumulation**: If inside, increment the `frameCount` for that vehicle's unique ID.
4.  **Wait Time Conversion**: Convert `frameCount` to seconds using the video's FPS:
    $$\text{Wait Time (sec)} = \frac{\text{frameCount}}{\text{FPS}}$$
5.  **Format Display**: Format the seconds into `MM:SS` string for overlay.

### Robustness Enhancements:
-   **Grace Period (Hysteresis)**: To handle momentary tracking flickers or occlusions, a `maxFramesLost` threshold (e.g., 10 frames) is implemented. If a vehicle is lost for fewer than this many frames, its timer is preserved.
-   **Centroid Smoothing**: Using the centroid instead of the full bounding box reduces sensitivity to minor bounding box fluctuations at the ROI edges.
-   **Frame Rate Normalization**: The algorithm uses the video's actual FPS (if available) or a default (30.0) to ensure time calculation remains accurate across different video sources.

---

## 2. Edge Cases Investigation

### A. Partial Entry/Exit
-   **Problem**: A vehicle may only partially overlap with the ROI.
-   **Solution**: Using the centroid ensures a consistent "trigger point." Alternatively, an "Area Overlap" threshold (e.g., >50%) could be used for more conservative timing.

### B. Occlusion (Temporary Lost Track)
-   **Problem**: A vehicle is temporarily hidden by another vehicle or structure.
-   **Solution**: The `maxFramesLost` parameter allows the tracker to "wait" for the vehicle to reappear. If it reappears near its last known position within the threshold, the timer resumes from where it left off.

### C. Lost Tracks & Re-entry
-   **Problem**: A vehicle leaves the ROI and returns, or its ID changes due to tracking failure.
-   **Solution**: To make this more robust, Re-Identification (Re-ID) using feature embeddings (e.g., DeepSORT) is suggested to maintain the same ID even after long occlusions.

### D. Multiple Vehicles in ROI
-   **Problem**: Several vehicles are present simultaneously.
-   **Solution**: The `TimerManager` uses a `std::map<int, ObjectTimer>` to maintain independent timers for every unique tracked ID.

### E. Stopped Vehicles
-   **Problem**: A vehicle stops inside the ROI.
-   **Solution**: The timer continues to increment as long as the vehicle is detected and its centroid remains in the ROI. This is the desired behavior for "wait time" calculation.

### F. Lighting Changes & Shadows
-   **Problem**: Shadows may be detected as part of the vehicle, or poor lighting may affect detection.
-   **Solution**: Using a robust DNN-based detector (like YOLOv8) is much more effective against lighting variations than traditional background subtraction methods.

---

## 3. Performance & Memory Management

### FPS Optimization:
-   **DNN Backend**: Configured to use OpenCV's optimized CPU backend.
-   **Efficient Tracking**: A centroid tracker is used instead of computationally expensive trackers like CSRT to maintain >30 FPS on standard hardware.
-   **Resource Management**: Uses `std::unique_ptr` and RAII principles to ensure zero memory leaks and efficient resource cleanup.
-   **Frame Skipping**: For extremely high-resolution videos, the application can be configured to process every $n$-th frame for detection while using faster tracking for intermediate frames.
