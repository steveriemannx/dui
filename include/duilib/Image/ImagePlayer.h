#ifndef UI_IMAGE_IMAGE_PLAYER_H_
#define UI_IMAGE_IMAGE_PLAYER_H_

#include "duilib/Core/UiTypes.h"
#include "duilib/Core/Callback.h"
#include "duilib/Core/ControlPtrT.h"
#include <map>

namespace ui
{
class Image;
class Control;

/** The logic wrapper of the control image animation playback (supports GIF/WebP/APNG/Lottie-JSON/PAG animations)
*/
class DUILIB_API ImagePlayer: public SupportWeakCallback
{
public:
    ImagePlayer();
    virtual ~ImagePlayer() override;
    ImagePlayer(const ImagePlayer&) = delete;
    ImagePlayer& operator = (const ImagePlayer&) = delete;

public:
    /** Set the associated control interface
    */
    void SetControl(Control* pControl);

    /** Set the associated image interface
    */
    void SetImage(Image* pImage);

    /** Set the display area of the animation (call before drawing)
    */
    void SetImageAnimationRect(const UiRect& rcImageRect);

    /** Play the animation (check on demand whether the image animation should be played automatically; under normal circumstances, the image animation plays automatically)
     */
    void CheckStartImageAnimation();

    /** Play the animation
     * @param [in] nStartFrame Which frame to start playing from; the first frame, the current frame, and the last frame can be set. Please refer to the AnimationImagePos enum
     * @param [in] nPlayCount Specify the number of plays
                   -1: Play forever
                    0: No valid play count; use the default value of the image (or the preset value)
                   >0: A specific number of plays; stop playing after reaching the play count
     */
    bool StartImageAnimation(AnimationImagePos nStartFrame, int32_t nPlayCount);

    /** Stop the animation playback
     * @param [in] nStopFrame Which frame to stop at after playback ends; the first frame, the current frame, and the last frame can be set. Please refer to the AnimationImagePos enum
     * @param [in] bTriggerEvent Whether to notify subscribers of the stop event; refer to the AttachImageAnimationStop method
     */
    void StopImageAnimation(AnimationImagePos nStopFrame, bool bTriggerEvent);

    /** Whether the animation is playing
    */
    bool IsAnimationPlaying() const;

    /** Set whether to start playing automatically
    */
    void SetAutoPlay(bool bAutoPlay);

    /** Get whether to start playing automatically
    */
    bool IsAutoPlay() const;

private:
    /** Get the animation frame number
    */
    uint32_t GetImageFrameIndex(AnimationImagePos frame) const;

    /** The timer playback animation callback function
    */
    void PlayingImageAnimation();

    /** Redraw the image
    */
    void RedrawImage();

    /** Whether it is a multi-frame image
    */
    bool IsMultiFrameImage() const;

    /** Get the playback status of the animation image
    */
    void GetImageAnimationStatus(ImageAnimationStatus& animStatus);

private:
    /** The cancellation mechanism of the image animation playback
    */
    WeakCallbackFlag m_animWeakFlag;

    /** The associated Control object
    */
    ControlPtr m_pControl;

    /** The image interface
    */
    Image* m_pImage;

    /** The display area of the animation
    */
    UiRect m_rcImageAnimationRect;

    /** The number of plays completed
    */
    int32_t m_nCycledCount;

    /** The maximum number of plays
    */
    int32_t m_nMaxPlayCount;

    /** Whether the animation image is playing
    */
    bool m_bAnimationPlaying;

    /** Whether to start playing automatically
    */
    bool m_bAutoPlay;
};

} // namespace ui

#endif // UI_IMAGE_IMAGE_PLAYER_H_
