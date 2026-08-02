#include "duilib/Animation/AnimationPlayer.h"
#include "duilib/Animation/EasingFunctions.h"
#include "duilib/Core/GlobalManager.h"

#define AP_NO_VALUE -1

namespace ui 
{
AnimationPlayer::AnimationPlayer():
    m_animationType(AnimationType::kAnimationNone),
    m_playCallback(nullptr),
    m_completeCallback(nullptr)
{
    Init();
}

AnimationPlayer::~AnimationPlayer()
{
}

void AnimationPlayer::Clear()
{
    m_weakFlagOwner.Cancel();
    m_bPlaying = false;
    m_playCallback = nullptr;
    m_completeCallback = nullptr;
    m_pEasingFunctions.reset();
}

void AnimationPlayer::Init()
{
    m_startValue = 0;
    m_endValue = 0;
    m_currentValue = 0;
    m_totalMillSeconds = -1;
    m_frameIntervalMillSeconds = -1;
    m_bReversePlay = false;
    m_bPlaying = false;
    m_frameIndex = 0;
    m_pEasingFunctions.reset();
    m_easingFunctionType = EasingFunctionType::EaseInOutCubic;
}

void AnimationPlayer::Start()
{
    m_weakFlagOwner.Cancel();
    if (m_bReversePlay) {
        ReverseAllValue();
        m_bReversePlay = false;
    }
    StartTimer(false, false);
}

void AnimationPlayer::Stop()
{
    m_weakFlagOwner.Cancel();
    if (m_bReversePlay) {
        ReverseAllValue();
        m_bReversePlay = false;
    }
    m_pEasingFunctions.reset();
    m_frameIndex = 0;
    m_currentValue = m_startValue;
    m_bPlaying = false;
}

void AnimationPlayer::Continue()
{
    const bool bContinueMode = IsPlaying();
    const bool bOldReversePlay = m_bReversePlay;
    m_weakFlagOwner.Cancel();
    if (m_bReversePlay) {
        ReverseAllValue();
        m_bReversePlay = false;
    }    
    StartTimer(bContinueMode, bOldReversePlay);
}

void AnimationPlayer::ReverseStart()
{
    m_weakFlagOwner.Cancel();
    if (!m_bReversePlay) {
        ReverseAllValue();
        m_bReversePlay = true;
    }
    StartTimer(false, false);
}

void AnimationPlayer::ReverseContinue()
{
    const bool bContinueMode = IsPlaying();
    const bool bOldReversePlay = m_bReversePlay;
    m_weakFlagOwner.Cancel();
    if (!m_bReversePlay) {
        ReverseAllValue();
        m_bReversePlay = true;
    }    
    StartTimer(bContinueMode, bOldReversePlay);
}

void AnimationPlayer::StartTimer(bool bContinueMode, bool bOldReversePlay)
{
    if (m_endValue == m_startValue) {
        Complete();
        return;
    }

    int32_t timerIntervalMs = m_frameIntervalMillSeconds;
    if (timerIntervalMs <= 0) {
        timerIntervalMs = 1000 / 60;//By default, play at 60 frames per second
    }

    int32_t totalMillSeconds = m_totalMillSeconds;
    if (totalMillSeconds <= 0) {
        totalMillSeconds = 180; //By default, the total animation duration is 180 milliseconds
    }

    //Calculate the frame count
    int32_t frameCount = totalMillSeconds / timerIntervalMs;
    if (frameCount < 1) {
        frameCount = 1;
    }

    //Check whether the playback should be continued (do not restart, but continue the current progress)
    if (m_pEasingFunctions == nullptr) {
        bContinueMode = false;
    }
    if (bContinueMode && (m_pEasingFunctions != nullptr)) {
        int32_t nStartValue = m_startValue;
        int32_t nEndValue = m_endValue;
        if (bOldReversePlay != m_bReversePlay) {
            std::swap(nStartValue, nEndValue);
        }
        if ((m_pEasingFunctions->GetFrameCount() != frameCount) ||
            (m_pEasingFunctions->GetStartValue() != nStartValue) ||
            (m_pEasingFunctions->GetEndValue() != nEndValue)) {
            //Important parameters have changed, restart
            bContinueMode = false;
        }
    }
    m_bPlaying = true;
    m_pEasingFunctions = std::make_unique<EasingFunctions>(m_startValue, m_endValue, frameCount, m_easingFunctionType);
    if (bContinueMode && (m_frameIndex >= 0) && (m_frameIndex <= frameCount)) {
        //Continue playback from where it left off
        if (bOldReversePlay != m_bReversePlay) {
            m_frameIndex = frameCount - m_frameIndex;
        }
        m_currentValue = m_pEasingFunctions->GetEasingValue(m_frameIndex);
    }
    else {
        //Restart
        m_frameIndex = 0;
        m_currentValue = m_startValue;
    }
    auto playCallback = UiBind(&AnimationPlayer::Play, this);
    GlobalManager::Instance().Timer().AddTimer(m_weakFlagOwner.GetWeakFlag(), playCallback, (uint32_t)timerIntervalMs);

    //First invocation: initialize the current value (to avoid errors caused by delayed invocation; for example, when setting the control size or position, initialization is required, otherwise exceptions may occur)
    if (m_playCallback) {
        m_playCallback(m_currentValue);
    }
}

void AnimationPlayer::Play()
{
    if (m_pEasingFunctions == nullptr) {
        m_weakFlagOwner.Cancel();
        return;
    }
    ++m_frameIndex;
    if (m_frameIndex > m_pEasingFunctions->GetFrameCount()) {
        m_frameIndex = m_pEasingFunctions->GetFrameCount();
    }
    int32_t newCurrentValue = m_pEasingFunctions->GetEasingValue(m_frameIndex);
    if (m_playCallback) {
        if (newCurrentValue != m_currentValue) {
            m_playCallback(newCurrentValue);
        }
    }
    m_currentValue = newCurrentValue;
    if (m_frameIndex == m_pEasingFunctions->GetFrameCount()) {
        //Playback complete
        Complete();
    }
}

void AnimationPlayer::ReverseAllValue()
{
    std::swap(m_startValue, m_endValue);
    m_currentValue = m_startValue;
}

void AnimationPlayer::Complete()
{
    m_weakFlagOwner.Cancel();
    m_bPlaying = false;
    m_currentValue = m_endValue;
    if (m_completeCallback) {
        m_completeCallback();
    }
}

} //namespace ui
