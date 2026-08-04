#ifndef UI_CORE_CONTROL_LOADING_H_
#define UI_CORE_CONTROL_LOADING_H_

#include "dui/Core/UiTypes.h"
#include "dui/Core/ControlPtrT.h"
#include <memory>
#include <chrono>

namespace ui 
{
/** Logic wrapper for the control loading state
*/
class Control;
class IRender;
class Box;
class LoadingAttribute;

//The loading status of the control
struct ControlLoadingStatus
{
    //The control interface associated with this loading status
    ControlPtrT<Control> m_pControl;

    //The root container interface displayed by this Loading status UI, which can be used to operate UI elements
    ControlPtrT<Box> m_pLoadingUiRootBox;

    //Time elapsed since loading started (milliseconds)
    uint64_t m_nElapsedTimeMs = 0;

    //The control interface where the animation image is located (the XML attributes need to be configured correctly)
    //For manual animation playback, use this interface's SetImageAnimationFrame to control which frame the animation plays to
    ControlPtrT<Control> m_pAnimationControl;

    //Return value: returning true means the loading status should be terminated, otherwise it stays in the loading status (only valid in the kEventLoading event)
    bool m_bStopLoading = false;
};

class DUI_API ControlLoading: public SupportWeakCallback
{
public:
    explicit ControlLoading(Control* pControl);
    virtual ~ControlLoading() override;
    ControlLoading(const ControlLoading&) = delete;
    ControlLoading& operator = (const ControlLoading&) = delete;

public:
    /** Set the loading attributes, create the display UI according to the XML file specified in the attributes, and set the UI attributes
    * @param [in] loadingAttribute The attribute string of loading
    */
    bool SetLoadingAttribute(const DString& loadingAttribute);

    /** Draw the "loading" status
    */
    void PaintLoading(IRender* pRender, const UiRect& rcPaint);

    /** Start the loading status
    * @param [in] nIntervalMs The callback interval (milliseconds), minimum 10 milliseconds
    * @param [in] nMaxCount Set the maximum number of callbacks, loading stops automatically when exceeded, the total loading time is nMaxCount * nIntervalMs milliseconds; -1 means loading continuously until manually stopped
    */
    bool StartLoading(int32_t nIntervalMs, int32_t nMaxCount);

    /** Close the loading status
    */
    void StopLoading();

    /** Whether it is loading
    * @return Returns true when loading, false otherwise
    */
    bool IsLoading() const;

    /** Get the root container interface of the loading UI (available only after StartLoading succeeds, and unavailable after StopLoading ends)
    */
    Box* GetLoadingUiRootBox() const;

    /** Update the position of the loading control
    */
    void UpdateLoadingPos();

private:
    /** Calculate the rotation angle of the loading image and rotate the image
    */
    void Loading();

    /** Parse the loading attributes
    */
    bool InitAttribute(LoadingAttribute& loadingAttribute, const DString& loadingString);

private:
    //Loading attributes
    std::unique_ptr<LoadingAttribute> m_pLoadingAttribute;

    //Lifecycle management and cancellation mechanism of the loading status
    WeakCallbackFlag m_loadingFlag;

    //The associated Control object
    Control* m_pControl;

    //The container of the Loading UI
    std::unique_ptr<Box> m_pLoadingBox;

    //The control interface where the animation image is located (the XML attributes need to be configured correctly)
    //For manual animation playback, use this interface's SetImageAnimationFrame to control which frame the animation plays to
    Control* m_pAnimationControl;

    //Start time
    std::chrono::steady_clock::time_point m_startTime;

    //The calling interval of the loading callback function
    int32_t m_nIntervalMs;

    //The maximum number of callbacks, -1 means loading continuously until manually stopped
    int32_t m_nMaxCount;

    //The current number of loading callbacks
    int32_t m_nCallbackCount;

    //Whether it is in the loading status
    bool m_bIsLoading;
};

} // namespace ui

#endif // UI_CORE_CONTROL_LOADING_H_
