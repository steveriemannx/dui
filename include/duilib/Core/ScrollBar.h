#ifndef UI_CORE_SCROLLBAR_H_
#define UI_CORE_SCROLLBAR_H_

#include "duilib/Core/Control.h"
#include "duilib/Image/StateImage.h"

namespace ui
{
    class ScrollBox;

/** Scroll bar control
*/
class DUILIB_API ScrollBar: public Control
{
    typedef Control BaseClass;
public:
    explicit ScrollBar(Window* pWindow);
    ScrollBar(const ScrollBar& r) = delete;
    ScrollBar& operator=(const ScrollBar& r) = delete;

    ScrollBox* GetOwner() const;
    void SetOwner(ScrollBox* pOwner);

    /// Override the base class methods to provide customized behavior; see the base class declaration
    virtual DString GetType() const override;
    virtual void SetFocus() override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool HasHotState() override;
    virtual bool MouseEnter(const EventArgs& msg) override;
    virtual bool MouseLeave(const EventArgs& msg) override;

    virtual void SetPos(UiRect rc) override;
    virtual void HandleEvent(const EventArgs& msg) override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;
    virtual void ClearImageCache() override;

    /** DPI changed; update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /**
     * @brief Whether it is a horizontal scroll bar
     * @return Returns true for a horizontal scroll bar, otherwise false for a vertical scroll bar
     */
    bool IsHorizontal();

    /**
     * @brief Set the scroll bar direction
     * @param[in] Set true for a horizontal scroll bar, false for a vertical scroll bar; defaults to true
     * @return None
     */
    void SetHorizontal(bool bHorizontal);

    /**
     * @brief Get the scroll range, corresponding to the range attribute in XML
     * @return Returns the scroll range
     */
    int64_t GetScrollRange() const;

    /**
     * @brief Set the scroll range
     * @param[in] nRange The scroll range to set
     * @return None
     */
    void SetScrollRange(int64_t nRange);

    /**
     * @brief Determine whether it is valid
     * @return true if valid, otherwise false
     */
    bool IsValid() { return GetScrollRange() != 0; }

    /**
     * @brief Get the scroll bar position
     * @return Returns the scroll bar position
     */
    int64_t GetScrollPos() const;

    /**
     * @brief Set the scroll bar position
     * @param[in] nPos The position to set
     * @return None
     */
    void SetScrollPos(int64_t nPos);

    /**
     * @brief Get the size of scrolling one line
     * @return Returns the size of scrolling one line
     */
    int GetLineSize() const;

    /** Set the size of scrolling one line
     * @param [in] nSize The size value to set
     * @param [in] bNeedDpiScale Whether to perform DPI scaling
     */
    void SetLineSize(int nSize, bool bNeedDpiScale);

    /**
     * @brief Get the minimum length of the thumb
     * @return Returns the minimum length of the thumb
     */
    int GetThumbMinLength() const;

    /** Set the minimum length of the thumb
     * @param[in] nThumbMinLength The minimum length to set
     * @param [in] bNeedDpiScale Whether to perform DPI scaling
     */
    void SetThumbMinLength(int nThumbMinLength, bool bNeedDpiScale);

    /**
     * @brief Whether to show the left or up button
     * @return Returns true to show, false to hide
     */
    bool IsShowButton1();

    /**
     * @brief Set whether to show the left or up button
     * @param[in] bShow Set true to show, false to hide
     * @return None
     */
    void SetShowButton1(bool bShow);

    /**
     * @brief Get the left or up button image for the specified state
     * @param[in] stateType The state to get, see the ControlStateType enum
     * @return Returns the image location
     */
    DString GetButton1StateImage(ControlStateType stateType);

    /**
     * @brief Set the left or up button image for the specified state
     * @param[in] stateType The state for which to set the image
     * @param[in] pStrImage The image location
     * @return None
     */
    void SetButton1StateImage(ControlStateType stateType, const DString& pStrImage);

    /**
     * @brief Whether to show the right or down button
     * @return Returns true to show, false to hide
     */
    bool IsShowButton2();

    /**
     * @brief Set whether to show the right or down button
     * @param[in] bShow Set true to show, false to hide
     * @return None
     */
    void SetShowButton2(bool bShow);

    /**
     * @brief Get the right or down button image for the specified state
     * @param[in] stateType The state to get, see the ControlStateType enum
     * @return Returns the image location
     */
    DString GetButton2StateImage(ControlStateType stateType);

    /**
     * @brief Set the right or down button image for the specified state
     * @param[in] stateType The state for which to set the image
     * @param[in] pStrImage The image location
     * @return None
     */
    void SetButton2StateImage(ControlStateType stateType, const DString& pStrImage);

    /**
     * @brief Get the thumb image for the specified state
     * @param[in] stateType The state identifier to get, see the ControlStateType enum
     * @return Returns the image location
     */
    DString GetThumbStateImage(ControlStateType stateType);

    /**
     * @brief Set the thumb image for the specified state
     * @param[in] stateType The state identifier to set, see the ControlStateType enum
     * @param[in] pStrImage The image location
     * @return None
     */
    void SetThumbStateImage(ControlStateType stateType, const DString& pStrImage);

    /**
     * @brief Get the rail image in the middle of the thumb for the specified state
     * @param[in] stateType The state identifier to get, see the ControlStateType enum
     * @return Returns the image location
     */
    DString GetRailStateImage(ControlStateType stateType);

    /**
     * @brief Set the rail image in the middle of the thumb for the specified state
     * @param[in] stateType The state identifier to set, see the ControlStateType enum
     * @param[in] pStrImage The image location
     * @return None
     */
    void SetRailStateImage(ControlStateType stateType, const DString& pStrImage);

    /**
     * @brief Get the background image for the specified state
     * @param[in] stateType The state identifier to get, see the ControlStateType enum
     * @return Returns the image location
     */
    DString GetBkStateImage(ControlStateType stateType);

    /**
     * @brief Set the background image for the specified state
     * @param[in] stateType The state identifier to set, see the ControlStateType enum
     * @param[in] pStrImage The image location
     * @return None
     */
    void SetBkStateImage(ControlStateType stateType, const DString& pStrImage);

    /**
     * @brief Whether to automatically hide the scroll bar
     * @return Returns true if yes, otherwise false
     */
    bool IsAutoHideScroll(){return m_bAutoHide;}

    /**
     * @brief Set whether to automatically hide the scroll bar
     * @param[in] hide Set to true to auto-hide, false to not auto-hide
     * @return None
     */
    void SetAutoHideScroll(bool hide);

    /**
     * @brief Get the thumb state
     * @return Returns the thumb state, see the ControlStateType enum
     */
    ControlStateType GetThumbState() { return m_uThumbState; }

    /** Set the height of the associated horizontal scroll bar
    */
    void SetHScrollbarHeight(int32_t nHScrollbarHeight);

protected:
    /** Set the visibility state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetVisible(bool bChanged) override;

    /** Set the enabled state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetEnabled(bool bChanged) override;

private:
    void ScrollTimeHandle();

    /// Drawing related functions
    void PaintBk(IRender* pRender);
    void PaintButton1(IRender* pRender);
    void PaintButton2(IRender* pRender);
    void PaintThumb(IRender* pRender);
    void PaintRail(IRender* pRender);

    /** Get the offset of one fast scroll
    */
    int64_t CalcFastScrollOffset(int32_t posOffset) const;

private:
    enum { 
        DEFAULT_SCROLLBAR_SIZE = 12,
    };

    //Whether it is a horizontal scroll bar: true for horizontal, false for vertical
    bool m_bHorizontal;

    //Whether to show the left or up button
    bool m_bShowButton1;

    //Whether to show the right or down button
    bool m_bShowButton2;

    //Whether to automatically hide the scroll bar
    bool m_bAutoHide;

    //The scroll range
    int64_t m_nRange;

    //The scroll bar position
    int64_t m_nScrollPos;

    //The size of scrolling one line
    int32_t m_nLineSize;

    //The minimum length of the scroll bar thumb
    int32_t m_nThumbMinLength;

    //The previous scroll bar position
    int64_t m_nLastScrollPos;

    //The previous scroll bar position offset
    int64_t m_nLastScrollOffset;

    //The delayed scroll repeat count
    int32_t m_nScrollRepeatDelay;

    /** For the vertical scroll bar: the height of the associated horizontal scroll bar
    */
    int32_t m_nHScrollbarHeight;

    //The last mouse position
    UiPoint m_ptLastMouse;

    //The position and size of the left or up button
    UiRect m_rcButton1;

    //The position and size of the right or down button
    UiRect m_rcButton2;

    //The position and size of the thumb
    UiRect m_rcThumb;

    //The state of the left or up button
    ControlStateType m_uButton1State;

    //The state of the right or down button
    ControlStateType m_uButton2State;

    //The state of the thumb
    ControlStateType m_uThumbState;

    //The target area of the image, used for drawing
    DString m_sImageModify;

    //The background images for each state
    std::unique_ptr<StateImage> m_bkStateImage;

    //The state images of the left or up button
    std::unique_ptr<StateImage> m_button1StateImage;

    //The state images of the right or down button
    std::unique_ptr<StateImage> m_button2StateImage;

    //The state images of the thumb
    std::unique_ptr<StateImage> m_thumbStateImage;

    //The rail image in the middle of the thumb for the specified state
    std::unique_ptr<StateImage> m_railStateImage;

    //The cancellation mechanism for delayed scrolling
    WeakCallbackFlag m_weakFlagOwner;

    //The container interface that supports the scroll bar
    ScrollBox* m_pOwner;
};

}//namespace ui

#endif // UI_CORE_SCROLLBAR_H_
