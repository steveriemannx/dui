#ifndef UI_CONTROL_COLORPICKER_REGULAR_H_
#define UI_CONTROL_COLORPICKER_REGULAR_H_

#include "duilib/Box/VirtualListBox.h"

namespace ui
{
/** Color picker: regular colors
*/
class ColorPickerRegularProvider;
class DUILIB_API ColorPickerRegular : public VirtualVTileListBox
{
    typedef VirtualVTileListBox BaseClass;
public:
    explicit ColorPickerRegular(Window* pWindow);
    virtual ~ColorPickerRegular() override;

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Select a color
    */
    void SelectColor(const UiColor& color);

    /** Get the currently selected color
    */
    UiColor GetSelectedColor() const;

    /** Listen to the color selection event
    * @param [in] callback The callback function triggered when the selected color changes
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    * Parameter description:
    *    wParam: The currently newly selected color value; a color can be generated with UiColor((uint32_t)wParam)
    *    lParam: The previously selected old color value; a color can be generated with UiColor((uint32_t)lParam)
    */
    void AttachSelectColor(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelectColor, callback, callbackID); }

    /** Set the control position
    */
    virtual void SetPos(UiRect rc) override;

    /** Set the number of display columns
    */
    void SetColumns(int32_t nColumns);

    /** Set the attribute
    */
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

private:
    /** The regular color data provider
    */
    std::unique_ptr<ColorPickerRegularProvider> m_regularColors;

    /** The previously set element control size
    */
    UiSize m_szItem;
};

}//namespace ui

#endif //UI_CONTROL_COLORPICKER_REGULAR_H_
