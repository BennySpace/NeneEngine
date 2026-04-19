// GameTimer.h

#pragma once

#include <chrono>

class GameTimer
{
public:
    GameTimer();

    float TotalTime() const;
    float DeltaTime() const;

    void Reset(); // Call before message loop.
    void Start(); // Call when unpaused.
    void Stop();  // Call when paused.
    void Tick();  // Call every frame.

private:
    using clock = std::chrono::steady_clock;

    clock::time_point mBaseTime;
    clock::duration   mPausedDuration;
    clock::time_point mStopTime;
    clock::time_point mPrevTime;
    clock::time_point mCurrTime;

    double mDeltaTime;
    bool   mStopped;
};
