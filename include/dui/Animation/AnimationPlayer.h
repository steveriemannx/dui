#ifndef UI_ANIMATION_ANIMATIONPLAYER_H_
#define UI_ANIMATION_ANIMATIONPLAYER_H_

#include "dui/Animation/EasingFunctions.h"
#include "dui/Core/Callback.h"

namespace ui
{

typedef std::function<void (int32_t)> AnimationPlayCallback;     //Playback callback function
typedef std::function<void (void)> AnimationCompleteCallback;    //Playback complete callback function

//Implementation of the easing functions
class EasingFunctions;

/** Base class interface of the control animation player
*/
class DUI_API AnimationPlayer : public virtual SupportWeakCallback
{
public:
    AnimationPlayer();
    virtual ~AnimationPlayer() override;

    AnimationPlayer(const AnimationPlayer& r) = delete;
    AnimationPlayer& operator=(const AnimationPlayer& r) = delete;

public:
    /** Set the animation type
    * @param [in] animationType The animation type
    */
    void SetAnimationType(AnimationType animationType) { m_animationType = animationType; }

    /** Get the animation type
    */
    AnimationType GetAnimationType() const { return m_animationType; }

    /** Set the easing function type
    */
    void SetEasingFunctionType(EasingFunctionType easingFunctionType) { m_easingFunctionType = easingFunctionType ; }

    /** Get the easing function type
    */
    EasingFunctionType GetEasingFunctionType() const { return m_easingFunctionType; }

    /** Set the start value of the animation playback
    * @param [in] startValue The start value of the animation playback
    */
    void SetStartValue(int32_t startValue) { m_startValue = startValue; }

    /** Get the start value of the animation playback
    */
    int32_t GetStartValue() const { return m_startValue; }

    /** Set the end value of the animation playback
    * @param [in] endValue The end value of the animation playback
    */
    void SetEndValue(int32_t endValue) { m_endValue = endValue; }

    /** Get the end value of the animation playback
    */
    int32_t GetEndValue() const { return m_endValue; }

    /** Set the timer interval for playing the animation (milliseconds)
    * @param [in] frameIntervalMillSeconds The timer interval for playing the animation (milliseconds)
    */
    void SetFrameIntervalMillSeconds(int32_t frameIntervalMillSeconds) { m_frameIntervalMillSeconds = frameIntervalMillSeconds; }

    /** Get the timer interval for playing the animation (milliseconds)
    */
    int32_t GetFrameIntervalMillSeconds() const { return m_frameIntervalMillSeconds; }

    /** Set the total playback time of the animation (milliseconds)
    * @param [in] totalMillSeconds The total playback time of the animation (milliseconds)
    */
    void SetTotalMillSeconds(int32_t totalMillSeconds) { m_totalMillSeconds = totalMillSeconds; }

    /** Get the total playback time of the animation (milliseconds)
    */
    int32_t GetTotalMillSeconds() const { return m_totalMillSeconds; }

public:
    /** Set the playback callback function
    */
    void SetPlayCallback(const AnimationPlayCallback& playCallback) { m_playCallback = playCallback; }

    /** Get the playback callback function
    */
    AnimationPlayCallback GetPlayCallback() const { return m_playCallback; }

    /** Set the playback complete callback function
    */
    void SetCompleteCallback(const AnimationCompleteCallback& completeCallback) { m_completeCallback = completeCallback; }

    /** Get the playback complete callback function
    */
    AnimationCompleteCallback GetCompleteCallback() const { return m_completeCallback; }

    /** Start the animation
    */
    void Start();

    /** Continue the animation (from the start value to the end value)
    */
    void Continue();

    /** Start the animation in reverse
    */
    void ReverseStart();

    /** Continue the animation in reverse (from the end value to the start value, reverse animation)
    */
    void ReverseContinue();

    /** Stop the animation
    */
    void Stop();

    /** Stop and clean up resources
    */
    void Clear();

public:
    /** Get the current value of the animation
    */
    int32_t GetCurrentValue() { return m_currentValue; }

    /** Whether it is playing
    */
    bool IsPlaying() const { return m_bPlaying; }

private:
    /** Initialize
    */
    void Init();

    /** Start the animation timer
    * @param [in] bContinueMode true means continue mode (do not restart, continue from the current progress), false means restart
    * @param [in] bOldReversePlay Whether the original playback mode is reverse playback; this parameter is only valid when bContinueMode is true
    */
    void StartTimer(bool bContinueMode, bool bOldReversePlay);

    /** Play the animation once (triggered by the timer)
    */
    void Play();

    /** Swap the start value and the end value
    */
    void ReverseAllValue();

    /** Complete the animation playback and trigger the playback complete callback function
    */
    void Complete();

private:
    /** Start value
    */
    int32_t m_startValue;

    /** End value
    */
    int32_t m_endValue;

    /** The timer interval for playing the animation (milliseconds)
    */
    int32_t m_frameIntervalMillSeconds;

    /** Total playback time (milliseconds)
    */
    int32_t m_totalMillSeconds;

    /** Playback callback function
    */
    AnimationPlayCallback m_playCallback;

    /** Playback complete callback function
    */
    AnimationCompleteCallback m_completeCallback;

private:
    /** Current value
    */
    int32_t m_currentValue;

    /** The frame index currently being played
    */
    int32_t m_frameIndex;

    /** Timer termination flag
    */
    WeakCallbackFlag m_weakFlagOwner;

    /** Implementation of the easing functions
    */
    std::unique_ptr<EasingFunctions> m_pEasingFunctions;

    /** The animation type
    */
    AnimationType m_animationType;

    /** The easing function type
    */
    EasingFunctionType m_easingFunctionType;

    /** Whether it is playing
    */
    bool m_bPlaying;

    /** Whether it is playing in reverse
    */
    bool m_bReversePlay;
};

} // namespace ui

#endif // UI_ANIMATION_ANIMATIONPLAYER_H_
