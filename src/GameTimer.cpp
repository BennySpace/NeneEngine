#include "GameTimer.h"

GameTimer::GameTimer()
    : mBaseTime{},
    mPausedDuration{},
    mStopTime{},
    mPrevTime{},
    mCurrTime{},
    mDeltaTime(-1.0),
    mStopped(false)
{
}

float GameTimer::TotalTime() const
{
    clock::time_point currentTime = mStopped ? mStopTime : mCurrTime;

    auto effective = currentTime - mBaseTime - mPausedDuration;

    return static_cast<float>(std::chrono::duration<double>(effective).count());
}

float GameTimer::DeltaTime() const
{
    return static_cast<float>(mDeltaTime);
}

void GameTimer::Reset()
{
    auto currTime = clock::now();

    mBaseTime = currTime;
    mPrevTime = currTime;
    mCurrTime = currTime;
    mStopTime = {};
    mPausedDuration = clock::duration::zero();
    mStopped = false;
}

void GameTimer::Start()
{
    if (mStopped)
    {
        auto startTime = clock::now();

        mPausedDuration += (startTime - mStopTime);

        mPrevTime = startTime;
        mCurrTime = startTime;
        mStopTime = {};
        mStopped = false;
    }
}

void GameTimer::Stop()
{
    if (!mStopped)
    {
        mStopTime = clock::now();
        mStopped = true;
    }
}

void GameTimer::Tick()
{
    if (mStopped)
    {
        mDeltaTime = 0.0;
        return;
    }

    auto currTime = clock::now();
    mCurrTime = currTime;

    auto deltaDuration = currTime - mPrevTime;
    mDeltaTime = std::chrono::duration<double>(deltaDuration).count();

    if (mDeltaTime < 0.0)
        mDeltaTime = 0.0;

    mPrevTime = currTime;
}