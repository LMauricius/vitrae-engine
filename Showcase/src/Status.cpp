#include "Status.hpp"

Status::Status()
    : totalSumFrameDuration(0.0s), totalFrameCount(0), totalAvgFrameDuration(0.0s), totalFPS(0.0f),
      currentAvgFrameDuration(0.0s), currentFPS(0.0f),
      currentTimeStamp(std::chrono::steady_clock::now()), trackingSumFrameDuration(0.0s),
      trackingFrameCount(0)
{}

void Status::update(std::chrono::duration<float> lastFrameDuration)
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    totalSumFrameDuration += lastFrameDuration;
    totalFrameCount++;
    totalAvgFrameDuration = totalSumFrameDuration / totalFrameCount;

    trackingSumFrameDuration += lastFrameDuration;
    trackingFrameCount++;

    if (now - currentTimeStamp >= 1s) {
        currentAvgFrameDuration = trackingSumFrameDuration / trackingFrameCount;
        currentFPS = 1.0f / currentAvgFrameDuration.count();
        trackingSumFrameDuration = 0s;
        trackingFrameCount = 0;
        currentTimeStamp = now;
    }
}
