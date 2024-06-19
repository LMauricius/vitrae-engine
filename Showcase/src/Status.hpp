#pragma once

#include <chrono>

using namespace std::chrono_literals;

struct Status
{
    std::chrono::duration<double> totalSumFrameDuration;
    std::size_t totalFrameCount;
    std::chrono::duration<double> totalAvgFrameDuration;
    float totalFPS;

    std::chrono::duration<double> currentAvgFrameDuration;
    float currentFPS;
    std::chrono::steady_clock::time_point currentTimeStamp;

    std::chrono::duration<double> trackingSumFrameDuration;
    std::size_t trackingFrameCount;

    Status();
    ~Status() = default;

    void update(std::chrono::duration<float> lastFrameDuration);
};