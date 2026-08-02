#ifndef UI_ANIMATION_ANIMATIONMANAGER_H_
#define UI_ANIMATION_ANIMATIONMANAGER_H_

#include "duilib/Animation/AnimationPlayer.h"
#include <map>
#include <memory>

namespace ui
{
class Control;

/** Animation player manager for a control
*/
class DUILIB_API AnimationManager
{
public:
    explicit AnimationManager(Control* pControl);
    AnimationManager(const AnimationManager& r) = delete;
    AnimationManager& operator=(const AnimationManager& r) = delete;

public:
    /** Check whether it contains the specified animation player interface
    */
    bool HasAnimationPlayer(AnimationType animationType) const;

    /** Get the player interface for the specified animation type
    * @param [in] animationType Playback type
    * @return Returns the animation player interface of the specified type, or nullptr if not matched
    */
    AnimationPlayer* GetAnimationPlayer(AnimationType animationType) const;

public:
    /** Set or clear the animation to play, corresponding to the animation type: kAnimationAlpha
    * @param [in] bFadeVisible true to set the animation, false to clear the animation
    * @param [in] nEndAlpha The Alpha value at the end (only used when bFadeVisible is true)
    * @return Returns the animation player interface when set, nullptr when cleared
    */
    AnimationPlayer* SetFadeAlpha(bool bFadeVisible, uint8_t nEndAlpha);

    /** Set or clear the animation to play, corresponding to the animation type: kAnimationWidth
    * @param [in] bFadeWidth true to set the animation, false to clear the animation
    * @return Returns the animation player interface when set, nullptr when cleared
    */
    AnimationPlayer* SetFadeWidth(bool bFadeWidth);

    /** Set or clear the animation to play, corresponding to the animation type: kAnimationHeight
    * @param [in] bFadeHeight true to set the animation, false to clear the animation
    * @return Returns the animation player interface when set, nullptr when cleared
    */
    AnimationPlayer* SetFadeHeight(bool bFadeHeight);

    /** Set or clear the animation to play, corresponding to the animation type: kAnimationSize
    * @param [in] bFadeSize true to set the animation, false to clear the animation
    * @return Returns the animation player interface when set, nullptr when cleared
    */
    AnimationPlayer* SetFadeSize(bool bFadeSize);

    /** Set or clear the animation to play
    * @param [in] bFade true to set the animation, false to clear the animation
    * @param [in] bIsFromRight true to animate from the right (kAnimationInoutXFromRight), false to animate from the left (kAnimationInoutXFromLeft)
    * @return Returns the animation player interface when set, nullptr when cleared
    */
    AnimationPlayer* SetFadeInOutX(bool bFade, bool bIsFromRight);

    /** Set or clear the animation to play
    * @param [in] bFade true to set the animation, false to clear the animation
    * @param [in] bIsFromBottom true to animate from the bottom (kAnimationInoutYFromBottom), false to animate from the top (kAnimationInoutYFromTop)
    * @return Returns the animation player interface when set, nullptr when cleared
    */
    AnimationPlayer* SetFadeInOutY(bool bFade, bool bIsFromBottom);

public:
    /** Show the control with the configured animation
    */
    void Appear();

    /** Hide the control with the configured animation
    */
    void Disappear();

    /** Clear all animation resources
    */
    void Clear(Control* control);

public:
    /** Set the timer interval for playing the animation (milliseconds)
    * @param [in] frameIntervalMillSeconds The timer interval for playing the animation (milliseconds)
    */
    void SetFrameIntervalMillSeconds(int32_t frameIntervalMillSeconds);

    /** Get the timer interval for playing the animation (milliseconds)
    */
    int32_t GetFrameIntervalMillSeconds() const;

    /** Set the total playback time of the animation (milliseconds)
    * @param [in] totalMillSeconds The total playback time of the animation (milliseconds)
    */
    void SetTotalMillSeconds(int32_t totalMillSeconds);

    /** Get the total playback time of the animation (milliseconds)
    */
    int32_t GetTotalMillSeconds() const;

    /** Set the easing function type
    */
    void SetEasingFunctionType(EasingFunctionType easingFunctionType);

    /** Get the easing function type
    */
    EasingFunctionType GetEasingFunctionType() const;

private:
    /** Initialize the animation type list for show/hide
    */
    void InitAppearAnimationList(std::vector<AnimationType>& animationList) const;

    /** Create an animation instance and initialize its default properties
    */
    AnimationPlayer* CreateAnimationPlayer(AnimationType animationType) const;

private:
    /** Interface of the control that owns the animation
    */
    Control* m_pControl;

    /** The timer interval for playing the animation (milliseconds)
    */
    int32_t m_frameIntervalMillSeconds;

    /** Total playback time (milliseconds)
    */
    int32_t m_totalMillSeconds;

    /** The easing function type
    */
    EasingFunctionType m_easingFunctionType;

    /** Current visibility value
    */
    bool m_bControlVisible;

    /** Whether the current visibility value has been initialized
    */
    bool m_bControlVisibleInited;

    /** The animation player interface for each animation type
    */
    std::map<AnimationType, std::unique_ptr<AnimationPlayer>> m_animationMap;

    /** Animation type list for show/hide
    */
    static std::vector<AnimationType> s_animationList;
};

} // namespace ui

#endif // UI_ANIMATION_ANIMATIONMANAGER_H_
