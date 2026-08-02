#ifndef UI_RENDER_IRENDER_H_
#define UI_RENDER_IRENDER_H_

#include "duilib/Core/Callback.h"
#include "duilib/Core/UiTypes.h"
#include "duilib/Core/SharePtr.h"
#include <map>

namespace ui 
{
/** Font interface
*/
class DUILIB_API IFont: public virtual SupportWeakCallback
{
public:
    /** Initialize the font (the font size is not DPI-adaptive internally)
     */
    virtual bool InitFont(const UiFont& fontInfo) = 0;

    /** Get the font name
    */
    virtual DString FontName() const = 0;

    /** Get the font size (font height)
    */
    virtual int32_t FontSize() const = 0;

    /** Whether it is bold
    */
    virtual bool IsBold() const = 0;

    /** Font underline state
    */
    virtual bool IsUnderline() const = 0;

    /** Font italic state
    */
    virtual bool IsItalic() const = 0;

    /** Font strikethrough state
    */
    virtual bool IsStrikeOut() const = 0;
};

/** Font manager interface
*/
class DUILIB_API IFontMgr : public virtual SupportWeakCallback
{
public:
    /** Get the number of fonts
    * @return Returns the number of fonts
    */
    virtual uint32_t GetFontCount() const = 0;

    /** Get the font name
    * @param [in] nIndex The index of the font: [0, GetFontCount())
    * @param [out] fontName Returns the font name
    * @return Returns true on success, false on failure
    */
    virtual bool GetFontName(uint32_t nIndex, DString& fontName) const = 0;

    /** Determine whether the font exists
    * @param [int] fontName The font name
    * @return Returns true if a font matching the font name exists, otherwise false
    */
    virtual bool HasFontName(const DString& fontName) const = 0;

    /** Set the default font name (used when the font to be loaded does not exist)
    * @param [in] fontName The default font name
    */
    virtual void SetDefaultFontName(const DString& fontName) = 0;

    /** Load the specified font file
    * @param [in] fontFilePath The path of the font file (local absolute path)
    * @return Returns true on success, false on failure
    */
    virtual bool LoadFontFile(const DString& fontFilePath) = 0;

    /** Load the specified font data
    * @param [in] data The in-memory data of the font file
    * @param [in] length The length of the in-memory font file data
    * @return Returns true on success, false on failure
    */
    virtual bool LoadFontFileData(const void* data, size_t length) = 0;
  
    /** Clear the loaded font files
    */
    virtual void ClearFontFiles() = 0;

    /** Clear the font cache
    */
    virtual void ClearFontCache() = 0;
};

/** The Skia engine requires the Alpha type to be passed in
*/
enum class DUILIB_API BitmapAlphaType: int
{
    kUnknown_SkAlphaType,   //!< uninitialized
    kOpaque_SkAlphaType,    //!< pixel is opaque
    kPremul_SkAlphaType,    //!< pixel components are premultiplied by alpha
    kUnpremul_SkAlphaType   //!< pixel components are independent of alpha
};

/** Bitmap interface
*/
class DUILIB_API IBitmap : public virtual SupportWeakCallback
{
public:
    /** Initialize from data (ARGB format)
    @param [in] nWidth Width
    @param [in] nHeight Height
    @param [in] pPixelBits Bitmap data; nullptr means an empty bitmap for the window, otherwise the data length is: nHeight * nWidth * 4
    @param [in] fImageSizeScale The image scale factor; 1.0f means the original value
    @param [in] alphaType The Alpha type of the bitmap; only the Skia engine needs this parameter
    */
    virtual bool Init(uint32_t nWidth, uint32_t nHeight,
                      const void* pPixelBits, float fImageSizeScale = 1.0f,
                      BitmapAlphaType alphaType = BitmapAlphaType::kPremul_SkAlphaType) = 0;

    /** Get the image width
    */
    virtual uint32_t GetWidth() const = 0;

    /** Get the image height
    */
    virtual uint32_t GetHeight() const = 0;

    /** Get the image size
    @return The image size
    */
    virtual UiSize GetSize() const = 0;

    /** Lock the bitmap data; the data length is GetWidth() * GetHeight() * 4
    */
    virtual void* LockPixelBits() = 0;

    /** Release the bitmap data
    */
    virtual void UnLockPixelBits() = 0;

    /** Clone to create a new bitmap
    *@return Returns the newly created bitmap interface; the caller releases the resources
    */
    virtual IBitmap* Clone() = 0;
};

/** Pen interface
*/
class DUILIB_API IPen : public virtual SupportWeakCallback
{
public:
    /** Set the pen width
    */
    virtual void SetWidth(float fWidth) = 0;

    /** Get the pen width
    */
    virtual float GetWidth() const = 0;

    /** Set the pen color
    */
    virtual void SetColor(UiColor color) = 0;

    /** Get the pen color
    */
    virtual UiColor GetColor() const = 0;

    /** Line cap style
    */
    enum LineCap
    {
        kButt_Cap   = 0,    //Flat cap (default)
        kRound_Cap  = 1,    //Round cap
        kSquare_Cap = 2     //Square cap
    };

    /** Set the line cap style at the start of the line segment
    */
    virtual void SetStartCap(LineCap cap) = 0;

    /** Get the line cap style at the start of the line segment
    */
    virtual LineCap GetStartCap() const = 0;

    /** Set the line cap style at the end of the line segment
    */
    virtual void SetEndCap(LineCap cap) = 0;

    /** Get the line cap style at the end of the line segment
    */
    virtual LineCap GetEndCap() const = 0;

    /** Set the dash cap style
    */
    virtual void SetDashCap(LineCap cap) = 0;

    /** Get the dash cap style
    */
    virtual LineCap GetDashCap() const = 0;

    /** The join style used at the end of a line segment where it meets another segment
    */
    enum LineJoin
    {
        kMiter_Join = 0,    //Miter join (default)
        kBevel_Join = 1,    //Bevel join
        kRound_Join = 2     //Round join        
    };

    /** Set the join style used at the end of a line segment
    */
    virtual void SetLineJoin(LineJoin join) = 0;

    /** Get the join style used at the end of a line segment
    */
    virtual LineJoin GetLineJoin() const = 0;

    /** The line style for lines drawn with the pen
    */
    enum DashStyle
    {
        kDashStyleSolid         = 0,    //Solid line (default)
        kDashStyleDash          = 1,    //Dashed line
        kDashStyleDot           = 2,    //Dotted line
        kDashStyleDashDot       = 3,    //Alternating dash-dot line
        kDashStyleDashDotDot    = 4     //Alternating dash-dot-dot line
    };

    /** Set the drawn line style
    */
    virtual void SetDashStyle(DashStyle style) = 0;

    /** Get the drawn line style
    */
    virtual DashStyle GetDashStyle() const = 0;

    /** Clone the Pen object
    */
    virtual IPen* Clone() const = 0;
};

/** Brush interface
*/
//Only one SolidBrush is currently supported
class DUILIB_API IBrush : public virtual SupportWeakCallback
{
public:
    virtual IBrush* Clone() = 0;
    virtual UiColor GetColor() const = 0;
};

/** Path interface
*/
class IMatrix;
class DUILIB_API IPath : public virtual SupportWeakCallback
{
public:    
    /** Fill type: how to fill the area formed when paths or curves intersect
    */
    enum class FillType 
    {
        /** Specifies that "inside" is computed by an odd number of edge crossings
        */
        kEvenOdd        = 0, //FillModeAlternate

        /** Specifies that "inside" is computed by a non-zero sum of signed edge crossings 
        */
        kWinding        = 1, //FillModeWinding

        /** Same as EvenOdd, but draws outside of the path, rather than inside 
        */
        kInverseEvenOdd    = 2,

        /** Same as Winding, but draws outside of the path, rather than inside 
        */
        kInverseWinding    = 3
    };

    /** Set the fill type
    */
    virtual void SetFillType(FillType mode) = 0;

    /** Get the fill type
    */
    virtual FillType GetFillType() = 0;

    /** Add a line to the current figure of this path
    * @param [in] x1 The x coordinate of the line start point
    * @param [in] y1 The y coordinate of the line start point
    * @param [in] x2 The x coordinate of the line end point
    * @param [in] y2 The y coordinate of the line end point
    */
    virtual void AddLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) = 0;
    virtual void AddLine(float x1, float y1, float x2, float y2) = 0;

    /** Add a series of connected lines to the current figure of this path
    * @param [in] points The array of points for the start and end of the lines; the first point in the array is the start of the first line,
                         and the last point in the array is the end of the last line. 
                         Every other point is used as the end of one line and the start of the next line.
    * @param [in] count The number of elements in the point array
    */
    virtual void AddLines(const UiPoint* points, int32_t count) = 0;
    virtual void AddLines(const UiPointF* points, int32_t count) = 0;

    /** Add a Bézier curve spline to the current figure of this path
    *    A Bézier spline is a curve specified by four points:
    *    two endpoints (p1[x1,y1] and p2[x4,y4]) and two control points (c1[x2,y2] and c2[x3,y3]). 
    *     The curve starts at p1 and ends at p2. 
    *     The curve does not pass through the control points, but the control points act like magnets, pulling the curve in a direction and affecting how it bends.
    * @param [in] x1 The x coordinate of the start point
    * @param [in] y1 The y coordinate of the start point
    * @param [in] x2 The x coordinate of the first control point
    * @param [in] y2 The y coordinate of the first control point
    * @param [in] x3 The x coordinate of the second control point
    * @param [in] y3 The y coordinate of the second control point
    * @param [in] x4 The x coordinate of the end point
    * @param [in] y4 The y coordinate of the end point
    */
    virtual void AddBezier(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t x4, int32_t y4) = 0;
    virtual void AddBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;

    /** Add a sequence of connected Bézier splines to the current figure of this path
    * @param [in] points A pointer to the array of start points, end points, and control points of the connected splines. 
    *                   The first spline is constructed from the first point to the fourth point in the array, using the second and third points as control points. 
    *                    Each subsequent spline in the sequence needs only three more points:
    *                    the end point of the previous spline is used as the start point, the next two points in the sequence are control points, and the third point is the end point.
    * @param [in] count The number of elements in the array
    */
    virtual void AddBeziers(const UiPoint* points, int32_t count) = 0;
    virtual void AddBeziers(const UiPointF* points, int32_t count) = 0;

    /** Add a rectangle to this path
    * @param [in] rect The rectangle area
    */
    virtual void AddRect(const UiRect& rect) = 0;
    virtual void AddRect(const UiRectF& rect) = 0;

    /** Add an ellipse to this path
    * @param [in] The rectangle area of the ellipse 
    *             left The x coordinate of the top-left corner of the ellipse's bounding box
    *             top  The y coordinate of the top-left corner of the ellipse's bounding box
    *             right left + the width of the ellipse's bounding rectangle
    *             bottom top + the height of the ellipse's bounding rectangle
    */
    virtual void AddEllipse(const UiRect& rect) = 0;
    virtual void AddEllipse(const UiRectF& rect) = 0;

    /** Add an elliptical arc to this path
    * @param [in] The rectangle area of the ellipse 
    * @param [in] startAngle The clockwise angle (in degrees) between the horizontal axis of the ellipse and the start point of the arc
    * @param [in] sweepAngle The clockwise angle (in degrees) between the start point (startAngle) and the end point of the arc
    */
    virtual void AddArc(const UiRect& rect, float startAngle, float sweepAngle) = 0;
    virtual void AddArc(const UiRectF& rect, float startAngle, float sweepAngle) = 0;

    /** Add a polygon to this path
    * @param [in] points The array of points specifying the vertices of the polygon
    * @param [in] count The number of elements in the array
    */
    virtual void AddPolygon(const UiPoint* points, int32_t count) = 0;
    virtual void AddPolygon(const UiPointF* points, int32_t count) = 0;

    /** Apply a matrix transform to the path, such as rotation
    * @param [in] pMatrix The matrix interface
    */
    virtual void Transform(IMatrix* pMatrix) = 0;

    /** Get the bounding rectangle of this path
    * @param [in] pen The associated Pen object, can be nullptr
    */
    virtual UiRect GetBounds(const IPen* pen) = 0;

    /** Close the current figure
    */
    virtual void Close() = 0;

    /** Reset the Path data
    */
    virtual void Reset() = 0;

    /** Clone the Path object
    */
    virtual IPath* Clone() = 0;
};

/** 3x3 matrix interface
*/
class DUILIB_API IMatrix : public SupportWeakCallback
{
public:
    /** Translation operation
    * @param [in] offsetX The translation offset along the X axis
    * @param [in] offsetY The translation offset along the Y axis
    */
    virtual void Translate(float offsetX, float offsetY) = 0;

    /** Scale operation (centered at the origin)
    * @param [in] scaleX The scale factor along the X axis
    * @param [in] scaleY The scale factor along the Y axis
    */
    virtual void Scale(float scaleX, float scaleY) = 0;

    /** Scale operation (centered at (px, py))
    * @param [in] scaleX The scale factor along the X axis
    * @param [in] scaleY The scale factor along the Y axis
    * @param [in] px The center point along the X axis
    * @param [in] py The center point along the Y axis
    */
    virtual void Scale(float scaleX, float scaleY, float px, float py) = 0;

    /** Rotation operation (centered at the origin)
    * @param [in] angle The rotation angle; positive is clockwise, negative is counterclockwise
    */
    virtual void Rotate(float angle) = 0;

    /** Rotation operation centered at the point (px, py)
    * @param [in] angle The rotation angle; positive is clockwise, negative is counterclockwise
    * @param [in] px The X coordinate of the rotation center point
    * @param [in] py The Y coordinate of the rotation center point
    */
    virtual void RotateAt(float angle, float px, float py) = 0;

    /** Skew operation (centered at the origin)
    * @param [in] kx The skew factor along the X axis
    * @param [in] ky The skew factor along the Y axis
    */
    virtual void Skew(float kx, float ky) = 0;

    /** Skew operation (centered at (px, py))
    * @param [in] kx The skew factor along the X axis
    * @param [in] ky The skew factor along the Y axis
    * @param [in] px The center point along the X axis
    * @param [in] py The center point along the Y axis
    */
    virtual void Skew(float kx, float ky, float px, float py) = 0;
};

/** DPI conversion helper interface used by the render interface
*/
class DUILIB_API IRenderDpi : public SupportWeakCallback
{
public:
    /** Scale an integer according to the UI scale factor
    * @param [in] iValue The integer
    * @return int The scaled value
    */
    virtual int32_t GetScaleInt(int32_t iValue) const = 0;

    /** Scale a floating-point number according to the UI scale factor
    * @param [in] fValue The floating-point number
    * @return The scaled value
    */
    virtual float GetScaleFloat(float fValue) const = 0;
};

typedef std::shared_ptr<IRenderDpi> IRenderDpiPtr;

/** Draw callback interface
*/
class DUILIB_API IRenderPaint
{
public:
    /** Complete drawing through the callback interface
    * @param [in] rcPaint The area to be drawn (client coordinates)
    */
    virtual bool DoPaint(const UiRect& rcPaint) = 0;

    /** Callback interface to get the current window's alpha value
    */
    virtual uint8_t GetLayeredWindowAlpha() = 0;

    /** Get the area that needs to be drawn, to support partial drawing
    * @param [out] rcUpdate Returns the rectangle range of the area to be drawn
    * @return Returns true if partial drawing is supported, false otherwise
    */
    virtual bool GetUpdateRect(UiRect& rcUpdate) const = 0;
};

/** Raster operation code
*/
enum class DUILIB_API RopMode
{
    kSrcCopy,    //Corresponds to SRCCOPY
    kDstInvert, //Corresponds to DSTINVERT
    kSrcInvert, //Corresponds to SRCINVERT
    kSrcAnd     //Corresponds to SRCAND
};

/** Format used when drawing text
*/
enum DUILIB_API DrawStringFormat
{
    TEXT_LEFT           = 0x0001,   //Horizontal alignment: left
    TEXT_HCENTER        = 0x0002,   //Horizontal alignment: center
    TEXT_RIGHT          = 0x0004,   //Horizontal alignment: right
    TEXT_HJUSTIFY       = 0x0008,   //Horizontal alignment: justify
    TEXT_HALIGN_ALL     = TEXT_LEFT | TEXT_HCENTER | TEXT_RIGHT | TEXT_HJUSTIFY,

    TEXT_TOP            = 0x0010,   //Vertical alignment: top
    TEXT_VCENTER        = 0x0020,   //Vertical alignment: center
    TEXT_BOTTOM         = 0x0040,   //Vertical alignment: bottom
    TEXT_VJUSTIFY       = 0x0080,   //Vertical alignment: justify
    TEXT_VALIGN_ALL     = TEXT_TOP | TEXT_VCENTER | TEXT_BOTTOM | TEXT_VJUSTIFY,

    TEXT_SINGLELINE     = 0x0100,   //Single-line text
    TEXT_NOCLIP         = 0x0200,   //Do not set a clip region when drawing
    TEXT_WORD_WRAP      = 0x0400,   //Automatic line wrapping (only supported by the IRender::DrawRichText interface; other text drawing functions do not support this property)

    TEXT_VERTICAL       = 0x0800,   //Draw text vertically; the drawing direction is from top to bottom and from right to left

    TEXT_PATH_ELLIPSIS  = 0x4000,   //If the drawing area is insufficient, ellipsize part of the text in the middle as with displayed file paths, adding "..."
    TEXT_END_ELLIPSIS   = 0x8000    //If the drawing area is insufficient, add "..." at the end to ellipsize part of the text
};

/** Render type
*/
enum class RenderType
{
    kRenderType_Skia = 0
};

/** Rich text data
*/
class RichTextData
{
public:
    /** Text content (the caller is responsible for ensuring the lifetime of the memory the string points to)
    */
    std::wstring_view m_textView;

    /** Text color
    */
    UiColor m_textColor;

    /** Background color
    */
    UiColor m_bgColor;

    /** Font information
    */
    SharePtr<UiFontEx> m_pFontInfo;

    /** Row spacing multiplier: the ratio as a multiple of the font size (usually 1.0 by default, i.e. 100% of the font size), used to adjust the row spacing proportionally
    */
    float m_fRowSpacingMul = 1.0f;

    /** Row spacing addition: a fixed additional pixel value (usually 0 by default), used to add a fixed offset (pixels) on top of the proportional adjustment
    */
    float m_fRowSpacingAdd = 0.0f;

    /** Text drawing attributes (including text alignment, etc.; see enum DrawStringFormat)
    */
    uint16_t m_textStyle = 0;
};

/** Character flags for drawn characters
*/
enum RichTextCharFlag: uint8_t
{
    kIsIgnoredChar  = 0x01,     //The current character is a non-drawn character
    kIsLowSurrogate = 0x02,     //The character is a low surrogate (a character composed of two Unicode code units; a UTF-16 encoded glyph, each character takes 1 or 2 Unicode code units)
    kIsReturn       = 0x04,     //Whether the current character is a carriage return '\r'
    kIsNewLine      = 0x08,     //Whether the current character is a newline '\n'
};

/** Attributes of a drawn character (4 bytes in total)
*/
struct RichTextCharInfo
{
    /** Character flags (read)
    */
    inline uint8_t CharFlag() const
    {
        uint32_t v = m_value;
        v >>= 24;
        return (uint8_t)v;
    }

    /** Character flags (set)
    */
    inline void SetCharFlag(uint8_t flag)
    {
        uint32_t v = flag;
        v <<= 24;
        m_value &= 0x00FFFFFF;
        m_value |= v;
    }

    /** Character flags (add)
    */
    inline void AddCharFlag(uint8_t flag)
    {
        uint32_t v = flag;
        v <<= 24;
        m_value |= v;
    }

    /** Character width (read)
    */
    inline float CharWidth() const
    {
        uint32_t v = m_value & 0x00FFFFFF;
        float fValue = (float)v;
        fValue /= 1000.0f;
        return fValue;
    }

    /** Character width (set)
    */
    inline void SetCharWidth(float charWidth)
    {
        uint32_t v = (uint32_t)(ui::CEILF(charWidth * 1000.0f));
        ASSERT(v < 0x00FFFFFF);
        v &= 0x00FFFFFF;
        m_value &= 0xFF000000;
        m_value |= v;
    }

    /** Whether the character is a carriage return
    */
    inline bool IsReturn() const { return CharFlag() & RichTextCharFlag::kIsReturn; }

    /** Whether the character is a newline
    */
    inline bool IsNewLine() const { return CharFlag() & RichTextCharFlag::kIsNewLine; }

    /** Whether the character is a non-drawn character
    */
    inline bool IsIgnoredChar() const { return CharFlag() & RichTextCharFlag::kIsIgnoredChar; }

    /** Whether the character is a low surrogate
    */
    inline bool IsLowSurrogate() const { return CharFlag() & RichTextCharFlag::kIsLowSurrogate; }

    /** Comparison operator
    */
    inline bool operator == (const RichTextCharInfo& r) const { return m_value == r.m_value; }
    inline bool operator != (const RichTextCharInfo& r) const { return m_value != r.m_value; }

private:
    /** Stored as an integer to reduce memory usage
    */
    uint32_t m_value = 0;
};

/** Basic information of a logical line (a line displayed in the rectangle area; a physical line corresponds to multiple logical lines when wrapped automatically)
*/
struct RichTextRowInfo: public NVRefCount<RichTextRowInfo>
{
    /** The characters in this line and their attributes
    */
    std::vector<RichTextCharInfo> m_charInfo;

    /** The rectangle area occupied by the text of this line
    */
    UiRectF m_rowRect;

    /** The left coordinate offset of this line (used to support center and right alignment)
    */
    int32_t m_xOffset = 0;
};
typedef SharePtr<RichTextRowInfo> RichTextRowInfoPtr;

/** Data of a physical line of text
*/
struct RichTextLineInfo: public NVRefCount<RichTextLineInfo>
{
    /** Text data length
    */
    uint32_t m_nLineTextLen = 0;

    /** Text data
    */
    UiStringW m_lineText;

    /** Basic information of logical rows
    */
    std::vector<RichTextRowInfoPtr> m_rowInfo;
};
typedef SharePtr<RichTextLineInfo> RichTextLineInfoPtr;

/** Data structure of physical lines
*/
typedef std::vector<RichTextLineInfoPtr> RichTextLineInfoList;

/** Input parameters for physical line data
*/
struct RichTextLineInfoParam
{
    /** The starting index of the associated physical line data in this drawing
    */
    uint32_t m_nStartLineIndex = 0;

    /** The starting row number (logical row number)
    */
    uint32_t m_nStartRowIndex = 0;

    /** The physical line data
    */
    RichTextLineInfoList* m_pLineInfoList = nullptr;
};

/** Draw cache for DrawRichText
*/
class DrawRichTextCache;

/** Clip region type
*/
enum class RenderClipType
{
    kEmpty, //Empty, no clip information
    kRect,  //The clip region is a rectangle
    kRegion //The clip region is a Region
};

/** Backend drawing type
*/
enum class RenderBackendType
{
    /** Draw using the CPU
    */
    kRaster_BackendType = 0,

    /** Draw using OpenGL
    *   Notes:
    *   (1) Only one window per thread is allowed to use OpenGL drawing; otherwise, the program may crash
    *   (2) Windows drawn with OpenGL must not be layered windows (i.e. windows with the WS_EX_LAYERED attribute)
    *   (3) Windows using OpenGL redraw the entire window on every draw and do not support partial drawing, so performance is not necessarily better than with CPU drawing; it is best to evaluate and choose the optimal drawing method for the actual situation
    */
    kNativeGL_BackendType = 1
};

/** Parameters used to measure the width and height of the specified text string
*/
struct MeasureStringParam
{
    int32_t rectSize = DUI_NOSET_VALUE; //For horizontal text: the limiting width of the current area; for vertical text: the limiting height of the current area
                                        //For multi-line text: a reasonable value should be set, otherwise the text will not wrap; for single-line text: this value is ignored)

    IFont* pFont = nullptr;             //The font data interface of the text; must not be nullptr
    uint32_t uFormat = 0;               //The format of the text; see the enum DrawStringFormat type definition
    float fSpacingMul = 1.0f;           //Row spacing multiplier: the ratio as a multiple of the font size (usually 1.0 by default, i.e. 100% of the font size), used to adjust the row spacing proportionally
    float fSpacingAdd = 0;              //Row spacing addition: a fixed additional pixel value (usually 0 by default), used to add a fixed offset (pixels) on top of the proportional adjustment
    float fWordSpacing = 0;             //Sets the spacing between two adjacent characters (pixels)

    bool bUseFontHeight = true;         //When drawing vertically, use the font's default height instead of each font's height (all fonts display at the same height)
    bool bRotate90ForAscii = true;      //When drawing vertically, rotate letters, digits, etc. by 90 degrees for display
};

struct DrawStringParam
{
    UiRect textRect;        //The rectangle area for drawing the text
    UiColor dwTextColor;    //The text color value
    uint8_t uFade = 255;    //The alpha of the text [0 - 255]

    IFont* pFont = nullptr;             //The font of the text
    uint32_t uFormat = 0;               //The format of the text; see the enum DrawStringFormat type definition    
    float fSpacingMul = 1.0f;           //Row spacing multiplier: the ratio as a multiple of the font size (usually 1.0 by default, i.e. 100% of the font size), used to adjust the row spacing proportionally
    float fSpacingAdd = 0;              //Row spacing addition: a fixed additional pixel value (usually 0 by default), used to add a fixed offset (pixels) on top of the proportional adjustment
    float fWordSpacing = 0;             //Sets the spacing between two adjacent characters (pixels)

    bool bUseFontHeight = true;         //When drawing vertically, use the font's default height instead of each font's height (all fonts display at the same height)
    bool bRotate90ForAscii = true;      //When drawing vertically, rotate letters, digits, etc. by 90 degrees for display
};

/** Render interface
*/
class IRenderFactory;
class DUILIB_API IRender : public virtual SupportWeakCallback
{
public:
    /** Get the render implementation type
    */
    virtual RenderType GetRenderType() const = 0;

    /** Get the backend render type
    */
    virtual RenderBackendType GetRenderBackendType() const = 0;

    /** Get the canvas width
    */
    virtual int32_t GetWidth() const = 0;

    /** Get the canvas height
    */
    virtual int32_t GetHeight() const = 0;

    /** Resize the canvas
    */
    virtual bool Resize(int32_t width, int32_t height) = 0;

    /** Set the window viewport origin coordinate offset,
     *  the original viewport origin x plus ptOffset.x becomes the new viewport origin x;
     *  the original viewport origin y plus ptOffset.y becomes the new viewport origin y;
     * @param [in] ptOffset The viewport origin coordinate offset
     *@return Returns the original viewport origin coordinates (x,y)
     */
    virtual UiPoint OffsetWindowOrg(UiPoint ptOffset) = 0;

    /** Map the point (pt.x, pt.y) to the viewport origin (0, 0)
     *@return Returns the original viewport origin coordinates (x,y)
     */
    virtual UiPoint SetWindowOrg(UiPoint pt) = 0;

    /** Get the viewport origin coordinates (x,y)
     * @return Returns the current viewport origin coordinates (x,y)
     */
    virtual UiPoint GetWindowOrg() const = 0;
    
    /** Save the current state of the specified device context
    * @param [out] Returns the saved device context state flag, passed as a parameter when calling RestoreClip
    */
    virtual void SaveClip(int32_t& nState) = 0;

    /** Restore the device context to the most recently saved state
    * @param [in] The saved device context state flag (returned by SaveClip)
    */
    virtual void RestoreClip(int32_t nState) = 0;

    /** Set a rectangular clip region and save the current device context state
    * @param [in] rc The clip region; the intersection with the current clip region becomes the new clip region
    * @param [in] bIntersect The ClipOp flag: true means the kIntersect operation, false means the kDifference operation
    */
    virtual void SetClip(const UiRect& rc, bool bIntersect = true) = 0;

    /** Set a rounded-rectangle clip region and save the current device context state
    * @param [in] rcItem The clip region; the intersection with the current clip region becomes the new clip region
    * @param [in] rx The corner radius width
    * @param [in] ry The corner radius height
    * @param [in] bIntersect The ClipOp flag: true means the kIntersect operation, false means the kDifference operation
    */
    virtual void SetRoundClip(const UiRect& rcItem, float rx, float ry, bool bIntersect = true) = 0;

    /** Clear the rectangular clip region and restore the device context to the most recently saved state
    */
    virtual void ClearClip() = 0;

    /** Performs a bit-block transfer of the color data corresponding to the pixel rectangle from the specified source device context to the target device context
    * @param [in] x The x coordinate of the top-left corner of the destination rectangle
    * @param [in] y The y coordinate of the top-left corner of the destination rectangle
    * @param [in] cx The width of the source and destination rectangles
    * @param [in] cy The height of the source and destination rectangles
    * @param [in] pSrcBitmap The source image interface
    * @param [in] pSrcRender The source Render object
    * @param [in] xSrc The x coordinate of the top-left corner of the source rectangle
    * @param [in] ySrc The y coordinate of the top-left corner of the source rectangle
    * @param [in] rop The raster operation code
    */
    virtual bool BitBlt(int32_t x, int32_t y, int32_t cx, int32_t cy,
                        IRender* pSrcRender, int32_t xSrc, int32_t ySrc,
                        RopMode rop) = 0;

    /** Copies a bitmap from the source rectangle to the destination rectangle, stretching or compressing the bitmap to fit the destination rectangle size if necessary. 
        The system stretches or compresses the bitmap according to the stretch mode currently set in the destination device context.
    * @param [in] xDest The x coordinate of the top-left corner of the destination rectangle
    * @param [in] yDest The y coordinate of the top-left corner of the destination rectangle
    * @param [in] widthDest The width of the destination rectangle
    * @param [in] heightDest The height of the destination rectangle
    * @param [in] pSrcRender The source Render object
    * @param [in] xSrc The x coordinate of the top-left corner of the source rectangle
    * @param [in] ySrc The y coordinate of the top-left corner of the source rectangle
    * @param [in] widthSrc The width of the source rectangle
    * @param [in] heightSrc The height of the source rectangle
    * @param [in] rop The raster operation code
    */
    virtual bool StretchBlt(int32_t xDest, int32_t yDest, int32_t widthDest, int32_t heightDest,
                            IRender* pSrcRender, int32_t xSrc, int32_t ySrc, int32_t widthSrc, int32_t heightSrc,
                            RopMode rop) = 0;


    /** Displays a bitmap with transparent or semi-transparent pixels; if the source and destination rectangles differ in size, the source bitmap is stretched to match the destination rectangle.
    * @param [in] xDest The x coordinate of the top-left corner of the destination rectangle
    * @param [in] yDest The y coordinate of the top-left corner of the destination rectangle
    * @param [in] widthDest The width of the destination rectangle
    * @param [in] heightDest The height of the destination rectangle
    * @param [in] pSrcRender The source Render object
    * @param [in] xSrc The x coordinate of the top-left corner of the source rectangle
    * @param [in] ySrc The y coordinate of the top-left corner of the source rectangle
    * @param [in] widthSrc The width of the source rectangle
    * @param [in] heightSrc The height of the source rectangle
    * @param [in] alpha The alpha value (0 - 255)
    */
    virtual bool AlphaBlend(int32_t xDest, int32_t yDest, int32_t widthDest, int32_t heightDest,
                            IRender* pSrcRender, int32_t xSrc, int32_t ySrc, int32_t widthSrc, int32_t heightSrc,
                            uint8_t alpha = 255) = 0;

    /** Draw an image (drawn with the nine-patch method)
    * @param [in] rcPaint The full currently drawable area (used to avoid drawing in non-drawable areas to improve drawing performance)
    * @param [in] pBitmap The bitmap interface used for drawing
    * @param [in] rcDest The destination area to draw to
    * @param [in] rcDestCorners The corner information of the destination area, used for nine-patch drawing
    * @param [in] rcSource The source image area to draw from
    * @param [in] rcSourceCorners The corner information of the source image, used for nine-patch drawing
    * @param [in] uFade The alpha (0 - 255)
    * @param [in] pTiledDrawParam Tiling-related parameters; pass nullptr when not drawing tiled
    * @param [in] bWindowShadowMode When drawing with nine-patch, do not draw the middle part (e.g. for window shadows, only the border needs to be drawn, not the middle part)
    */
    virtual void DrawImage(const UiRect& rcPaint, IBitmap* pBitmap, 
                           const UiRect& rcDest, const UiRect& rcDestCorners,
                           const UiRect& rcSource, const UiRect& rcSourceCorners,
                           uint8_t uFade = 255,
                           const TiledDrawParam* pTiledDrawParam = nullptr,
                           bool bWindowShadowMode = false) = 0;
    /** Draw an image (drawn with the nine-patch method), without corner parameters
    */
    virtual void DrawImage(const UiRect& rcPaint, IBitmap* pBitmap, 
                           const UiRect& rcDest,  const UiRect& rcSource, 
                           uint8_t uFade = 255,
                           const TiledDrawParam* pTiledDrawParam = nullptr,
                           bool bWindowShadowMode = false) = 0;

    /** Draw an image
    * @param [in] rcPaint The full currently drawable area (used to avoid drawing in non-drawable areas to improve drawing performance)
    * @param [in] pBitmap The bitmap interface used for drawing
    * @param [in] rcDest The destination area to draw to
    * @param [in] rcSource The source image area to draw from
    * @param [in] uFade The alpha (0 - 255)
    * @param [in] pMatrix The transform matrix interface used when drawing
    */
    virtual void DrawImageRect(const UiRect& rcPaint, IBitmap* pBitmap,
                               const UiRect& rcDest, const UiRect& rcSource,
                               uint8_t uFade = 255, IMatrix* pMatrix = nullptr) = 0;

    /** Draw a line
    * @param [in] pt1 The start point coordinates
    * @param [in] pt2 The end point coordinates
    * @param [in] penColor The pen color value
    * @param [in] nWidth The pen width
    */
    virtual void DrawLine(const UiPointF& pt1, const UiPointF& pt2, UiColor penColor, float fWidth) = 0;

    /** Draw a line, supporting various line styles
    * @param [in] pt1 The start point coordinates
    * @param [in] pt2 The end point coordinates
    * @param [in] pen The pen interface
    */
    virtual void DrawLine(const UiPointF& pt1, const UiPointF& pt2, IPen* pen) = 0;

    /** Draw a rectangle
    * @param [in] rc The rectangle area
    * @param [in] penColor The pen color value
    * @param [in] fWidth The pen width
    * @param [in] bLineInRect If true, ensures the drawn line is strictly confined inside the rc rectangle; otherwise the line's center is aligned with the rc border and part of the line extends beyond the rc rectangle
    */
    virtual void DrawRect(const UiRectF& rc, UiColor penColor, float fWidth, bool bLineInRect = false) = 0;

    /** Draw a rectangle, supporting various line styles
    * @param [in] rc The rectangle area
    * @param [in] pen The pen interface
    * @param [in] bLineInRect If true, ensures the drawn line is strictly confined inside the rc rectangle; otherwise the line's center is aligned with the rc border and part of the line extends beyond the rc rectangle
    */
    virtual void DrawRect(const UiRectF& rc, IPen* pen, bool bLineInRect = false) = 0;

    /** Fill a rectangle with a color
    * @param [in] rc The destination rectangle area
    * @param [in] dwColor The color value
    * @param [in] uFade The alpha (0 - 255)
    */
    virtual void FillRect(const UiRectF& rc, UiColor dwColor, uint8_t uFade = 255) = 0;

    /** Fill a rectangle with a gradient color (gradient colors supported)
    * @param [in] rc The destination rectangle area
    * @param [in] dwColor The first color value
    * @param [in] dwColor2 The second color value
    * @param [in] nColor2Direction The gradient direction of the gradient color; "1": left->right, "2": top->bottom, "3": top-left->bottom-right, "4": top-right->bottom-left
    * @param [in] uFade The alpha (0 - 255)
    */
    virtual void FillRect(const UiRectF& rc, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction, uint8_t uFade = 255) = 0;

    /** Draw a rounded rectangle
    * @param [in] rc The rectangle area
    * @param [in] rx The corner radius width
    * @param [in] ry The corner radius height
    * @param [in] penColor The pen color value
    * @param [in] fWidth The pen width
    */
    virtual void DrawRoundRect(const UiRectF& rc, float rx, float ry, UiColor penColor, float fWidth) = 0;

    /** Draw a rounded rectangle, supporting various line styles
    * @param [in] rc The rectangle area
    * @param [in] rx The corner radius width
    * @param [in] ry The corner radius height
    * @param [in] pen The pen interface
    */
    virtual void DrawRoundRect(const UiRectF& rc, float rx, float ry, IPen* pen) = 0;

    /** Fill a rounded rectangle with a color
    * @param [in] rc The rectangle area
    * @param [in] rx The corner radius width
    * @param [in] ry The corner radius height
    * @param [in] dwColor The color value
    * @param [in] uFade The alpha (0 - 255)
    */
    virtual void FillRoundRect(const UiRectF& rc, float rx, float ry, UiColor dwColor, uint8_t uFade = 255) = 0;

    /** Fill a rounded rectangle with a color (gradient colors supported)
    * @param [in] rc The rectangle area
    * @param [in] rx The corner radius width
    * @param [in] ry The corner radius height
    * @param [in] dwColor The color value
    * @param [in] dwColor2 The second color value
    * @param [in] nColor2Direction The gradient direction of the gradient color; "1": left->right, "2": top->bottom, "3": top-left->bottom-right, "4": top-right->bottom-left
    * @param [in] uFade The alpha (0 - 255)
    */
    virtual void FillRoundRect(const UiRectF& rc, float rx, float ry, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction, uint8_t uFade = 255) = 0;

    /** Draw a curve (part of an ellipse)
    * @param [in] rc The rectangular bounding area of the ellipse containing the arc
    * @param [in] startAngle  The angle between the x axis and the arc start point
    * @param [in] sweepAngle  The angle between the start and end points of the arc; positive is clockwise, negative is counterclockwise
    * @param [in] useCenter If true, includes the center point of the ellipse (only used by the Skia engine)
    * @param [in] pen The pen interface; sets the pen color and pen width
    * @param [in] gradientColor Optional parameter: the gradient color
    * @param [in] gradientRect Optional parameter: the rectangle area setting for the gradient color; only valid when gradientColor is not nullptr
    */
    virtual void DrawArc(const UiRect& rc, float startAngle, float sweepAngle, bool useCenter, 
                         const IPen* pen, 
                         UiColor* gradientColor = nullptr, const UiRect* gradientRect = nullptr) = 0;

    /** Draw a circle
    * @param [in] centerPt The center point coordinates
    * @param [in] radius The radius of the circle
    * @param [in] penColor The pen color value
    * @param [in] fWidth The pen width
    */
    virtual void DrawCircle(const UiPointF& centerPt, float radius, UiColor penColor, float fWidth) = 0;

    /** Draw a circle, supporting various line styles
    * @param [in] centerPt The center point coordinates
    * @param [in] radius The radius of the circle
    * @param [in] pen The pen interface
    */
    virtual void DrawCircle(const UiPointF& centerPt, float radius, IPen* pen) = 0;

    /** Fill a circle
    * @param [in] centerPt The center point coordinates
    * @param [in] radius The radius of the circle
    * @param [in] dwColor The color value
    * @param [in] uFade The alpha (0 - 255)
    */
    virtual void FillCircle(const UiPointF& centerPt, float radius, UiColor dwColor, uint8_t uFade = 255) = 0;

    /** Draw a path
    * @param [in] path The path interface
    * @param [in] pen The pen used to draw the path
    */
    virtual void DrawPath(const IPath* path, const IPen* pen) = 0;

    /** Fill a path
    * @param [in] path The path interface
    * @param [in] brush The brush used to fill the path
    */
    virtual void FillPath(const IPath* path, const IBrush* brush) = 0;

    /** Fill a path (background color gradient supported)
    * @param [in] path The path interface
    * @param [in] rc The rectangle area
    * @param [in] dwColor The first color used to fill the path
    * @param [in] dwColor2 The second color used to fill the path
    * @param [in] nColor2Direction The gradient direction of the gradient color; "1": left->right, "2": top->bottom, "3": top-left->bottom-right, "4": top-right->bottom-left
    */
    virtual void FillPath(const IPath* path, const UiRectF& rc, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction) = 0;

    /** Measure the width and height of the specified text string
    * @param [in] strText The text content
    * @param [in] measureParam The parameters used for measuring
    * @return Returns the width and height of the text string, represented as a rectangle
    */
    virtual UiRect MeasureString(const DString& strText, const MeasureStringParam& measureParam) = 0;

    /** Draw text
    * @param [in] strText The text content
    * @param [in] drawParam The parameters related to text drawing
    */
    virtual void DrawString(const DString& strText, const DrawStringParam& drawParam) = 0;

    /** Measure the width and height of the rich text
    * @param [in] textRect The rectangle area for drawing the text
    * @param [in] szScrollOffset The scroll position of the rectangle area in which the text is drawn
    * @param [in] pRenderFactory The render interface, used to create fonts
    * @param [in] richTextData The formatted text content; returns the areas where the text is drawn
    * @param [out] pRichTextRects If not nullptr, returns the list of rectangle ranges drawn for each item in richTextData
    */
    virtual void MeasureRichText(const UiRect& textRect,
                                 const UiSize& szScrollOffset,
                                 IRenderFactory* pRenderFactory, 
                                 const std::vector<RichTextData>& richTextData,
                                 std::vector<std::vector<UiRect>>* pRichTextRects) = 0;

    /** Measure the width and height of the rich text, and calculate the position of each character
    * @param [in] textRect The rectangle area for drawing the text
    * @param [in] szScrollOffset The scroll position of the rectangle area in which the text is drawn
    * @param [in] pRenderFactory The render interface, used to create fonts
    * @param [in] richTextData The formatted text content
    * @param [in,out] pLineInfoParam If not nullptr, calculates the area of each character
    * @param [out] pRichTextRects If not nullptr, returns the list of rectangle ranges drawn for each item in richTextData
    */
    virtual void MeasureRichText2(const UiRect& textRect,
                                  const UiSize& szScrollOffset,
                                  IRenderFactory* pRenderFactory, 
                                  const std::vector<RichTextData>& richTextData,
                                  RichTextLineInfoParam* pLineInfoParam,
                                  std::vector<std::vector<UiRect>>* pRichTextRects) = 0;

    /** Measure the width and height of the rich text, calculate the position of each character, and create a draw cache
    * @param [in] textRect The rectangle area for drawing the text
    * @param [in] szScrollOffset The scroll position of the rectangle area in which the text is drawn
    * @param [in] pRenderFactory The render interface, used to create fonts
    * @param [in] richTextData The formatted text content
    * @param [in,out] pLineInfoParam If not nullptr, calculates the area of each character
    * @param [out] spDrawRichTextCache Returns the draw cache
    * @param [out] pRichTextRects If not nullptr, returns the list of rectangle ranges drawn for each item in richTextData
    */
    virtual void MeasureRichText3(const UiRect& textRect,
                                  const UiSize& szScrollOffset,
                                  IRenderFactory* pRenderFactory, 
                                  const std::vector<RichTextData>& richTextData,
                                  RichTextLineInfoParam* pLineInfoParam,
                                  std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache,
                                  std::vector<std::vector<UiRect>>* pRichTextRects) = 0;

    /** Draw the rich text
    * @param [in] textRect The rectangle area for drawing the text
    * @param [in] szScrollOffset The scroll position of the rectangle area in which the text is drawn
    * @param [in] pRenderFactory The render interface, used to create fonts
    * @param [in] richTextData The formatted text content
    * @param [in] uFade The alpha (0 - 255)
    * @param [out] pRichTextRects If not nullptr, returns the list of rectangle ranges drawn for each item in richTextData
    */
    virtual void DrawRichText(const UiRect& textRect,
                              const UiSize& szScrollOffset,
                              IRenderFactory* pRenderFactory, 
                              const std::vector<RichTextData>& richTextData,
                              uint8_t uFade = 255,
                              std::vector<std::vector<UiRect>>* pRichTextRects = nullptr) = 0;

    /** Create a draw cache for RichText
    * @param [in] textRect The rectangle area for drawing the text
    * @param [in] szScrollOffset The scroll position of the rectangle area in which the text is drawn
    * @param [in] pRenderFactory The render interface, used to create fonts
    * @param [in] richTextData The formatted text content
    * @param [out] spDrawRichTextCache Returns the draw cache
    */
    virtual bool CreateDrawRichTextCache(const UiRect& textRect,
                                         const UiSize& szScrollOffset,
                                         IRenderFactory* pRenderFactory,
                                         const std::vector<RichTextData>& richTextData,
                                         std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache) = 0;

    /** Determine whether the RichText draw cache is valid
    * @param [in] textRect The rectangle area for drawing the text
    * @param [in] richTextData The formatted text content; returns the areas where the text is drawn
    * @param [out] spDrawRichTextCache Returns the draw cache
    */
    virtual bool IsValidDrawRichTextCache(const UiRect& textRect,
                                          const std::vector<RichTextData>& richTextData,
                                          const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache) = 0;

    /** Update the RichText draw cache (incremental calculation)
    * @param [in] spOldDrawRichTextCache The cache to be updated
    * @param [in] spUpdateDrawRichTextCache The cache for incremental drawing
    * @param [in,out] richTextDataNew The latest full data; the data is swapped into the internal container
    * @param [in] nStartLine The starting line number for recalculation
    * @param [in] modifiedLines The line numbers that were modified
    * @param [in] nModifiedRows The modified text, split into how many lines (logical lines) after calculation
    * @param [in] deletedLines The deleted lines
    * @param [in] nDeletedRows How many logical lines were deleted
    * @param [in] rowRectTopList The top coordinate of each logical line, used to update line coordinates (the index is the logical line, numbered from 0)
    */
    virtual bool UpdateDrawRichTextCache(std::shared_ptr<DrawRichTextCache>& spOldDrawRichTextCache,
                                         const std::shared_ptr<DrawRichTextCache>& spUpdateDrawRichTextCache,
                                         std::vector<RichTextData>& richTextDataNew,
                                         size_t nStartLine,
                                         const std::vector<size_t>& modifiedLines,
                                         size_t nModifiedRows,
                                         const std::vector<size_t>& deletedLines,
                                         size_t nDeletedRows,
                                         const std::vector<int32_t>& rowRectTopList) = 0;

    /** Compare whether the data of two draw caches is identical
    */
    virtual bool IsDrawRichTextCacheEqual(const DrawRichTextCache& first, const DrawRichTextCache& second) const = 0;

    /** Draw the content in the RichText cache (before drawing, use IsValidDrawRichTextCache to check whether the cache is invalid)
    * @param [in] spDrawRichTextCache The cached data
    * @param [in] rcNewTextRect The rectangle area for drawing the text
    * @param [in] szNewScrollOffset The new scroll position
    * @param [in] rowXOffset The horizontal offset list of each line (logical lines)
    * @param [in] uFade The alpha (0 - 255)
    * @param [out] pRichTextRects If not nullptr, returns the list of rectangle ranges drawn for each item in richTextData
    */
    virtual void DrawRichTextCacheData(const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache,
                                       const UiRect& textRect,
                                       const UiSize& szNewScrollOffset,
                                       const std::vector<int32_t>& rowXOffset,
                                       uint8_t uFade,
                                       std::vector<std::vector<UiRect>>* pRichTextRects = nullptr) = 0;

    /** Draw a shadow around the specified rectangle (Gaussian blur; only outer shadows are supported, inner shadows are not)
    * @param [in] rc The rectangle area
    * @param [in] roundSize The corner radius width and height of the shadow
    * @param [in] cpOffset Sets the shadow offset (offset-x and offset-y)
    *                      <offset-x> Sets the horizontal offset; if negative, the shadow is to the left of the rectangle. 
    *                      <offset-y> Sets the vertical offset; if negative, the shadow is above the rectangle.
    * @param [in] nBlurRadius The blur radius; the larger the value, the larger the blur area and the larger and lighter the shadow. If 0, the shadow edges are sharp with no blur effect. It cannot be negative.
    * @param [in] nSpreadRadius The spread radius, i.e. how many pixels the blur area extends beyond the edge of the rc rectangle.
    *                           A positive value expands the shadow; a negative value shrinks it.
    * @param [in] dwColor The shadow color value
    * @param [in] uAlpha The alpha (0 - 255)
    */
    virtual void DrawBoxShadow(const UiRect& rc, 
                               const UiSize& roundSize, 
                               const UiPoint& cpOffset, 
                               int32_t nBlurRadius, 
                               int32_t nSpreadRadius,
                               UiColor dwColor,
                               uint8_t uAlpha) = 0;


    /** Snapshot the bitmap
    *@return Returns the bitmap interface; after it is returned, the caller manages the resources (including releasing them)
    */
    virtual IBitmap* MakeImageSnapshot() = 0;

    /** Set the Alpha of the image in the rectangle area to the specified value alpha (0 - 255)
    * @param [in] rcDirty The rectangle area
    * @param [in] alpha The Alpha value to set
    */
    virtual void ClearAlpha(const UiRect& rcDirty, uint8_t alpha = 0) = 0;

    /** Restore the Alpha value of the image in the rectangle area to alpha (0 - 255)
    * @param [in] rcDirty The rectangle area
    * @param [in] rcShadowPadding The shadow padding (the Padding values for the left/right/top/bottom margins of the rectangle respectively)
    * @param [in] alpha The Alpha value to restore (must be the same as the alpha value passed to ClearAlpha)
    */
    virtual void RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding, uint8_t alpha) = 0;

    /** Restore the Alpha value of the image in the rectangle area to 255
    * @param [in] rcDirty The rectangle area
    * @param [in] rcShadowPadding The shadow padding (the Padding values for the left/right/top/bottom margins of the rectangle respectively)
    */
    virtual void RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding = UiPadding()) = 0;

#ifdef DUILIB_BUILD_FOR_WIN
    /** Get the DC handle; after use, call the ReleaseDC interface to release the resources
    */
    virtual HDC GetRenderDC(HWND hWnd) = 0;

    /** Release DC resources
    * @param [in] hdc The DC handle to release
    */
    virtual void ReleaseRenderDC(HDC hdc) = 0;
#endif

public:
    /** Clear the bitmap data and fill with the specified color (pass the UiColor() parameter to zero out all the bitmap data)
    * @param [in] uiColor The color value to fill
    */
    virtual void Clear(const UiColor& uiColor) = 0;

    /** Clear the data of the specified bitmap area and fill with the specified color (pass the UiColor() parameter to zero out all the bitmap data)
    * @param [in] rcDirty The area to clear
    * @param [in] uiColor The color value to fill
    */
    virtual void ClearRect(const UiRect& rcDirty, const UiColor& uiColor) = 0;

    /** Clone a new object
    */
    virtual std::unique_ptr<IRender> Clone() = 0;

    /** Read the bitmap data within the specified rectangle range (a copy of the target data is made)
    * @param [in] rc The rectangle range in the Render
    * @param [in] dstPixels The start address of the destination buffer to read into
    * @param [in] dstPixelsLen The buffer length of the destination dstPixels; the length must satisfy: dstPixelsLen >= (rc.Width() * rc.Height() * sizeof(uint32_t))
    */
    virtual bool ReadPixels(const UiRect& rc, void* dstPixels, size_t dstPixelsLen) = 0;

    /** Write data into the bitmap (the data is copied into the target)
    * @param [in] srcPixels The start address of the source data buffer
    * @param [in] srcPixelsLen The buffer length of the source data srcPixels; the length must satisfy: srcPixelsLen == (rc.Width() * rc.Height() * sizeof(uint32_t))
    * @param [in] rc The rectangle range in the Render into which the bitmap data is written; rc.Width() is the width of the image data and rc.Height() is the height of the image data
    */
    virtual bool WritePixels(void* srcPixels, size_t srcPixelsLen, const UiRect& rc) = 0;

    /** Write data into the bitmap (the data is copied into the target; only the drawn part is copied)
    * @param [in] srcPixels The start address of the source data buffer
    * @param [in] srcPixelsLen The buffer length of the source data srcPixels; the length must satisfy: srcPixelsLen >= (rc.Width() * rc.Height() * sizeof(uint32_t))
    * @param [in] rc The rectangle range in the Render; rc.Width() is the width of the image data and rc.Height() is the height of the image data
    * @param [in] rcPaint The rectangle range of the drawn part, representing the dirty area; only the intersection of rcPaint and rc is drawn
    */
    virtual bool WritePixels(void* srcPixels, size_t srcPixelsLen, const UiRect& rc, const UiRect& rcPaint) = 0;

    /** Get the current clip region
    * @param [out] clipRects Returns the rectangle data of the clip region; the rectangle coordinates are client coordinates
                             For RenderClipType::kRect, the container has only one element;
                             for RenderClipType::kRegion, the container has multiple elements used to build the Region
    * @return Returns the clip region type
    */
    virtual RenderClipType GetClipInfo(std::vector<UiRect>& clipRects) = 0;

    /** Determine whether the clip region is empty (if empty, no drawing is needed)
    */
    virtual bool IsClipEmpty() const = 0;

    /** Whether it is empty (width or height is 0)
    */
    virtual bool IsEmpty() const = 0;

    /** Set the DPI conversion interface used by the Render
    */
    virtual void SetRenderDpi(const IRenderDpiPtr& spRenderDpi) = 0;

    /** Draw and swap to the screen (the Render implementation is already associated with the window); completes synchronously
    * @param [in] pRenderPaint The callback interface needed for UI drawing
    */
    virtual bool PaintAndSwapBuffers(IRenderPaint* pRenderPaint) = 0;

    /** Set the window shape to a rounded rectangle
    * @param [in] rcWnd The area for which to set the RGN; coordinates are screen coordinates
    * @param [in] rx The corner radius width; must not be 0
    * @param [in] ry The corner radius height; must not be 0
    * @param [in] bRedraw Whether to redraw
    */
    virtual bool SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw) = 0;

    /** Set the window shape to a rectangle
    * @param [in] rcWnd The area for which to set the RGN; coordinates are screen coordinates
    * @param [in] bRedraw Whether to redraw
    */
    virtual bool SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw) = 0;

    /** Clear the window shape setting and restore the system default shape
    * @param [in] bRedraw Whether to redraw
    */
    virtual void ClearWindowRgn(bool bRedraw) = 0;

};

/** Render interface manager, used to create render implementation objects such as Font, Pen, Brush, Path, Matrix, Bitmap, and Render
*/
class DUILIB_API IRenderFactory
{
public:
    virtual ~IRenderFactory() = default;

    /** Create a Font object
    */
    virtual IFont* CreateIFont() = 0;

    /** Create a Pen object
    */
    virtual IPen* CreatePen(UiColor color, float fWidth = 1) = 0;

    /** Create a Brush object
    */
    virtual IBrush* CreateBrush(UiColor corlor) = 0;

    /** Create a Path object
    */
    virtual IPath* CreatePath() = 0;

    /** Create a Matrix object
    */
    virtual IMatrix* CreateMatrix() = 0;

    /** Create a Bitmap object
    */
    virtual IBitmap* CreateBitmap() = 0;

    /** Create a Render object
    * @param [in] spRenderDpi The associated DPI conversion interface
    * @param [in] platformData Platform-specific data; on Windows this value is the window handle
    * @parma [in] backendType The backend drawing type
    */
    virtual IRender* CreateRender(const IRenderDpiPtr& spRenderDpi,
                                  void* platformData = nullptr,
                                  RenderBackendType backendType = RenderBackendType::kRaster_BackendType) = 0;

    /** Get the font manager interface (each factory shares one object)
    */
    virtual IFontMgr* GetFontMgr() const = 0;
};

} // namespace ui

#endif // UI_RENDER_IRENDER_H_
