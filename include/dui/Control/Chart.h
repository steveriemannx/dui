#ifndef UI_CONTROL_CHART_H_
#define UI_CONTROL_CHART_H_

#include "dui/Core/Control.h"
#include "dui/Animation/AnimationPlayer.h"
#include <vector>
#include <functional>

namespace ui {

/** Chart chart type
*/
enum class ChartType
{
    kLine = 0,  // Line chart
    kBar  = 1,  // Bar chart
    kPie  = 2   // Pie chart
};

/** Bar chart arrangement mode
*/
enum class ChartBarMode
{
    kGrouped = 0,  // Grouped (multiple series side by side)
    kStacked = 1   // Stacked (multiple series accumulated)
};

/** Line chart drawing mode
*/
enum class ChartLineMode
{
    kStraight = 0,  // Polyline (connected by straight lines)
    kCurve    = 1   // Smooth curve (Bezier)
};

/** Chart lightweight chart control (inherits from Control, custom painted)
 *  Supports three forms - line chart (line), bar chart (bar) and pie chart (pie); the data is bound through XML attributes or the C++ interface.
 *
 *  Features:
 *  - Multiple series: line/bar charts support overlaying and comparing several data sets, with automatic series colors
 *  - Axes: Y axis tick values, X/Y axis titles, chart title, grid lines
 *  - Bar chart: supports negative values (the baseline adjusts automatically), grouped/stacked modes
 *  - Line chart: polyline/smooth curve, area fill, data point marker switch
 *  - Pie chart: donut mode, percentage display, legend
 *
 *  XML attributes (single series, backward compatible):
 *  - chart_type: "line"/"bar"/"pie", default "line"
 *  - data / data_labels: comma-separated values / category labels
 *  - series_color / axis_color / label_color: series/axis/text colors (semantic color name or color value)
 *  - title: chart title
 *  - x_axis_title / y_axis_title: X/Y axis titles
 *  - show_value: whether to show the data value labels, default true
 *  - show_grid: whether to show the grid lines, default true
 *  - show_axis_values: whether to show the Y axis tick values, default true
 *  - axis_divisions: number of Y axis divisions (1~10), default 4
 *  - legend_visible: whether to show the legend (true by default for pie charts, false for line/bar charts)
 *  - line_width: line width, default 2
 *  - line_mode: "straight"/"curve", polyline/curve, default straight
 *  - area_fill: whether to fill the area below the line, default false
 *  - show_data_points: whether to draw the data point markers (line chart), default true
 *  - bar_mode: "grouped"/"stacked", bar arrangement, default grouped
 *  - donut: whether the pie chart is a donut, default false
 *  - show_percent: whether the pie chart shows percentages, default false
 *
 *  All colors use semantic color names (ThemeColor of global.xml) and adapt to the light and dark themes automatically.
 */
class DUI_API Chart : public Control
{
    typedef Control BaseClass;

public:
    /** A data series
    */
    struct Series
    {
        /** Series name (used by the legend, may be empty)
        */
        std::string name;

        /** Series data
        */
        std::vector<double> data;

        /** Main color of the series (semantic color name or color value; assigned automatically when empty)
        */
        std::string color;
    };

    /** Type of the hit element
    */
    enum class HitType
    {
        kNone = 0,  // No hit
        kPoint = 1, // Line chart data point
        kBar   = 2, // Bar of a bar chart
        kSlice = 3, // Pie chart sector
        kLegend = 4 // Legend item
    };

    /** Hit test result
    */
    struct HitResult
    {
        HitType type = HitType::kNone; // Hit type
        size_t seriesIndex = 0;        // Series index (valid for line points/bars/legend items)
        size_t dataIndex = 0;          // Data index (valid for line points/bars/sectors)
        UiPoint ptHit;                 // Hit position (control coordinates)
    };

    /** Line chart layout parameters (shared by painting and hit testing, so that the coordinates match)
    */
    struct LineLayout
    {
        UiRect rcPlot;              // Plot area
        double dMin = 0.0;          // Lower bound of the Y axis
        double dMax = 0.0;          // Upper bound of the Y axis
        double dRange = 1.0;        // Y axis range
        int32_t nDivisions = 1;     // Actual number of divisions (the value after aligning to nice ticks)
        double viewStart = 0.0;     // Start index of the visible window (fractional)
        double viewEnd = 0.0;       // End index of the visible window (fractional)
        size_t nMaxCount = 0;       // Maximum number of data points

        /** Data index -> X pixel coordinate
        */
        float XToPixel(double index) const {
            double t = (viewEnd <= viewStart) ? 0.5 : ((index - viewStart) / (viewEnd - viewStart));
            return static_cast<float>(rcPlot.left) + static_cast<float>(rcPlot.Width()) * static_cast<float>(t);
        }
        /** Data value -> Y pixel coordinate
        */
        float YToPixel(double value) const {
            double norm = (value - dMin) / dRange;
            return static_cast<float>(rcPlot.bottom) - static_cast<float>(rcPlot.Height()) * static_cast<float>(norm);
        }
        /** X pixel coordinate -> data index
        */
        double PixelToX(int32_t px) const {
            if (rcPlot.Width() <= 0) {
                return viewStart;
            }
            double t = static_cast<double>(px - rcPlot.left) / static_cast<double>(rcPlot.Width());
            return viewStart + t * (viewEnd - viewStart);
        }
    };

public:
    explicit Chart(Window* pWindow);
    Chart(const Chart& r) = delete;
    Chart& operator=(const Chart& r) = delete;
    ~Chart() override;

    /** Get the control type
    */
    std::string GetType() const override;

    /** Set an XML attribute
    */
    void SetAttribute(const std::string& strName, const std::string& strValue) override;

    /** Paint the chart content (axes, series, legend, data labels)
    */
    void PaintStateImages(IRender* pRender) override;

    /** Refresh the title/axis titles/category labels on a language change (linked through text_id)
    */
    void OnLanguageChanged() override;

    // Mouse interaction (overrides the base class virtual functions to implement the tooltip/selection/zoom and pan)
    bool MouseMove(const EventArgs& msg) override;
    bool MouseLeave(const EventArgs& msg) override;
    bool ButtonDown(const EventArgs& msg) override;
    bool ButtonUp(const EventArgs& msg) override;
    bool MouseWheel(const EventArgs& msg) override;

    /** Set whether the data point tooltip is enabled (shows the series/category/value on hover), default true
    */
    void SetEnableTooltip(bool bEnable);

    /** Get whether the data point tooltip is enabled
    */
    bool IsEnableTooltip() const { return m_bEnableTooltip; }

    /** Set whether click selection is enabled (pie sector highlight/offset, single bar highlight), default true
    */
    void SetEnableSelect(bool bEnable);

    /** Get whether click selection is enabled
    */
    bool IsEnableSelect() const { return m_bEnableSelect; }

    /** Get the currently selected element (type == kNone when nothing is selected)
    */
    HitResult GetSelected() const { return m_selected; }

    /** Clear the selection
    */
    void ClearSelection();

    /** Data point click event callback (the EventArgs::wParam parameter is a packed pointer to the hit element, parsed by GetHitFromEvent)
    */
    void AttachPointClick(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Data point hover event callback
    */
    void AttachPointHover(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Parse the hit element from the wParam of a click/hover event
    *   @return The hit result (kNone is returned when args.wParam is empty)
    */
    static HitResult GetHitFromEvent(const EventArgs& args);

public:
    /** Set the chart type
    */
    void SetChartType(ChartType chartType);

    /** Get the chart type
    */
    ChartType GetChartType() const { return m_chartType; }

    /** Set the chart title
    */
    void SetTitle(const std::string& strTitle);

    /** Get the chart title
    */
    const std::string& GetTitle() const { return m_strTitle; }

    /** Set the X axis title
    */
    void SetXAxisTitle(const std::string& strTitle);

    /** Set the Y axis title
    */
    void SetYAxisTitle(const std::string& strTitle);

    /** Set the language ID of the chart title (text_id, multi-language support)
    */
    void SetTitleId(const std::string& strTitleId);

    /** Set the language ID of the X axis title
    */
    void SetXAxisTitleId(const std::string& strTitleId);

    /** Set the language ID of the Y axis title
    */
    void SetYAxisTitleId(const std::string& strTitleId);

    /** Set the language ID of the category labels (multiple comma-separated STRIDs, multi-language support)
    */
    void SetDataLabelsId(const std::string& strLabelsId);

    /** Set the series data (single series, replaces the existing data), compatible with the old interface
    */
    void SetData(const std::vector<double>& data);

    /** Append a data point (single series)
    */
    void AddData(double value);

    /** Clear all series data
    */
    void ClearData();

    /** Get the single-series data (compatible with the old interface, takes the first series)
    */
    const std::vector<double>& GetData() const;

    /** Set the category labels corresponding to the data (X axis categories, shared by all series)
    */
    void SetDataLabels(const std::vector<std::string>& labels);

    /** Get the data labels
    */
    const std::vector<std::string>& GetDataLabels() const { return m_dataLabels; }

    /** Set the multi-series data (replaces all series)
    */
    void SetSeriesData(const std::vector<Series>& series);

    /** Add a series
    */
    void AddSeries(const Series& series);

    /** Get all series
    */
    const std::vector<Series>& GetSeries() const { return m_series; }

    /** Get the series count
    */
    size_t GetSeriesCount() const { return m_series.size(); }

    /** Axis tick label formatting callback (similar to Slider's TickLabelCallback)
     *  @param value The value of the tick (the tick value for the Y axis; the category index for the X axis)
     *  @param index The tick index (starting from 0)
     *  @return The text to display (e.g. "1.2k", "50%", "03:20")
     *  @note Clearing the callback restores the default FormatValue number formatting (the X axis falls back to the data_labels string)
     */
    typedef std::function<std::string(double value, int32_t index)> AxisLabelFormatter;

    /** Set the Y axis tick label formatting callback (clearing it restores the default number formatting)
     */
    void SetYAxisLabelFormatter(const AxisLabelFormatter& callback);

    /** Get the Y axis tick label formatting callback
     */
    const AxisLabelFormatter& GetYAxisLabelFormatter() const { return m_yAxisLabelFormatter; }

    /** Set the X axis category label formatting callback (clearing it restores the default: the data_labels string)
     *  @note For the X axis callback, value is the category index (double) and index is the category index
     */
    void SetXAxisLabelFormatter(const AxisLabelFormatter& callback);

    /** Get the X axis category label formatting callback
     */
    const AxisLabelFormatter& GetXAxisLabelFormatter() const { return m_xAxisLabelFormatter; }

    /** Incremental update: append a data point to the end of the specified series (streaming append for real-time monitoring)
     *  @return Returns true on success (false when seriesIndex is out of range)
     *  @note If the limit set by SetMaxDataPoints is exceeded after the append, the oldest points are dropped automatically
     */
    bool AddDataPoint(size_t seriesIndex, double value);

    /** Incremental update: insert a data point at the specified position of the specified series
     *  @note When dataIndex exceeds the current length it is equivalent to appending at the end
     */
    bool InsertDataPoint(size_t seriesIndex, size_t dataIndex, double value);

    /** Incremental update: modify the data point at the specified position of the specified series in place (the data length does not change)
     */
    bool SetDataPoint(size_t seriesIndex, size_t dataIndex, double value);

    /** Incremental update: remove the data point at the specified position of the specified series
     */
    bool RemoveDataPoint(size_t seriesIndex, size_t dataIndex);

    /** Read the data point at the specified position of the specified series (returns 0.0 when out of range)
     */
    double GetDataPoint(size_t seriesIndex, size_t dataIndex) const;

    /** Get the data of the specified series (read-only reference, returns an empty vector when out of range)
     */
    const std::vector<double>& GetSeriesData(size_t seriesIndex) const;

    /** Clear the data of the specified series (the series itself is kept, its color/name are unchanged)
     */
    bool ClearSeries(size_t seriesIndex);

    /** Remove the specified series (including its data)
     */
    bool RemoveSeries(size_t seriesIndex);

    /** Set the maximum number of data points of the rolling window: when an append/insert exceeds the limit, the oldest points are dropped automatically (0 means unlimited)
     *  Used to bound the memory in real-time monitoring scenarios and avoid unbounded accumulation
     */
    void SetMaxDataPoints(size_t nMax);

    /** Get the maximum number of data points of the rolling window (0 means unlimited)
     */
    size_t GetMaxDataPoints() const { return m_nMaxDataPoints; }

    /** Set the main color of the series (single series, semantic color name or color value)
    */
    void SetSeriesColor(const std::string& strColor);

    /** Set the axis/grid line color (semantic color name or color value)
    */
    void SetAxisColor(const std::string& strColor);

    /** Set the text label color (semantic color name or color value)
    */
    void SetLabelColor(const std::string& strColor);

    /** Set whether to show the data value labels
    */
    void SetShowValue(bool bShowValue);

    /** Set whether to show the legend
    */
    void SetLegendVisible(bool bLegendVisible);

    /** Set whether to show the grid lines
    */
    void SetShowGrid(bool bShowGrid);

    /** Set whether to show the Y axis tick values
    */
    void SetShowAxisValues(bool bShow);

    /** Set the number of Y axis divisions (1~10)
    */
    void SetAxisDivisions(int32_t nDivisions);

    /** Set the line width (line chart)
    */
    void SetLineWidth(int32_t nLineWidth);

    /** Set the line drawing mode (polyline/curve)
    */
    void SetLineMode(ChartLineMode lineMode);

    /** Set whether to fill the area below the line
    */
    void SetAreaFill(bool bAreaFill);

    /** Set whether to draw the data point markers (line chart)
    */
    void SetShowDataPoints(bool bShow);

    /** Set the bar arrangement mode (grouped/stacked)
    */
    void SetBarMode(ChartBarMode barMode);

    /** Set whether the pie chart is a donut
    */
    void SetDonut(bool bDonut);

    /** Set whether the pie chart shows percentages
    */
    void SetShowPercent(bool bShowPercent);

    /** Set the corner radius of the bars (pixels, DPI scaling is applied automatically), 0 means square corners, default 3
    */
    void SetBarRadius(int32_t nRadius);

    /** Set whether the bars use a gradient fill (bright at the top, dark at the bottom), default true
    */
    void SetBarGradient(bool bGradient);

    /** Set whether the line chart draws a glow/shadow effect, default true
    */
    void SetLineGlow(bool bGlow);

    /** Set whether the data update animation is enabled (bar growth/line fade-in), default true
    */
    void SetAnimationEnabled(bool bEnabled);

    /** Set the duration of the data update animation (milliseconds), default 300
    */
    void SetAnimationDuration(int32_t nMs);

private:
    /** Parse a comma-separated numeric string
    */
    static void ParseDataString(const std::string& strData, std::vector<double>& data);

    /** Parse a comma-separated label string
    */
    static void ParseLabelsString(const std::string& strLabels, std::vector<std::string>& labels);

    /** Get the main color of a series (the index-th series); when it fails, one is assigned automatically
    */
    UiColor GetSeriesColor(size_t index) const;

    /** Assign a series color automatically by index (evenly distributed hues)
    */
    static UiColor MakeSeriesColor(size_t index, size_t count);

    /** Generate a same-hue sector color by index (used for the multiple sectors of a pie chart)
    */
    static UiColor MakeSliceColor(UiColor baseColor, size_t index, size_t count);

    /** Generate the dark end of the gradient fill (used for the bar gradient and the line glow)
    */
    static UiColor MakeDarkerColor(UiColor baseColor, uint8_t nScale);

    /** Format a value as a string (keeping a suitable precision)
    */
    static std::string FormatValue(double value);

    /** Format a Y axis tick value: prefer m_yAxisLabelFormatter, otherwise fall back to FormatValue
     */
    std::string FormatAxisValue(double value, int32_t index) const;

    /** Paint text (simplified wrapper)
    */
    void DrawChartText(IRender* pRender, const std::string& strText, const UiRect& rc, uint32_t uFormat);

    /** Paint the title and the axis titles
    */
    void PaintTitles(IRender* pRender, const UiRect& rcChart, const UiRect& rcPlot);

    /** Compute the plot area (excluding the space of the title/legend/axis titles/Y axis tick values)
    */
    UiRect CalcPlotRect(IRender* pRender, const UiRect& rcChart) const;

    /** Measure the width of a single line of text (pixels, DPI scaling already included)
    */
    int32_t MeasureTextWidth(IRender* pRender, const std::string& strText) const;

    /** Estimate the text width from the character count (a conservative value that avoids the problem of MeasureString measuring CJK text too narrow)
    */
    int32_t EstimateTextWidth(const std::string& strText) const;

    /** Take the larger of the measured and the estimated values as the safe width used for the layout
    */
    int32_t GetSafeTextWidth(IRender* pRender, const std::string& strText) const;

    /** Measure the maximum width needed by the Y axis tick value area (pixels, DPI scaling already included)
    */
    int32_t MeasureAxisValueWidth(IRender* pRender) const;

    /** Compute the combined range of all series data (without margins)
    */
    void CalcValueRange(double& dMin, double& dMax) const;

    /** Compute nice axis ticks (the step is 1/2/5 x 10^n, the tick values are integers or rounded decimals)
    *   @param dMin The minimum data value (includes the margin on input, aligned to a nice tick on return)
    *   @param dMax The maximum data value (same as above)
    *   @param nDivisions On input the desired number of divisions, on return the actual number (which may be less than requested)
    */
    void CalcNiceAxis(double& dMin, double& dMax, int32_t& nDivisions) const;

    /** Start the data update animation (bar growth/line fade-in)
    */
    void StartDataAnimation();

    /** Get the current animation progress (0~1, 1 means the animation finished or is disabled)
    */
    double GetAnimProgress() const;

    /** Paint the line chart
    */
    void PaintLine(IRender* pRender, const UiRect& rcChart);

    /** Paint the bar chart
    */
    void PaintBar(IRender* pRender, const UiRect& rcChart);

    /** Paint the pie chart
    */
    void PaintPie(IRender* pRender, const UiRect& rcChart);

    /** Paint the legend (line/bar charts, shown with multiple series)
    */
    void PaintLegend(IRender* pRender, const UiRect& rcChart);

    /** Compute the line chart layout parameters (including the visible window and the Y axis range)
    */
    LineLayout CalcLineLayout(IRender* pRender, const UiRect& rcChart) const;

    /** Hit test: detect the element hit by the mouse position
    *   @param pRender The render interface (used to obtain the font measurements)
    *   @param ptMouse The mouse position (control coordinates)
    *   @return The hit result
    */
    HitResult HitTest(IRender* pRender, const UiPoint& ptMouse);

    /** Build the tooltip text of the hit element
    */
    std::string BuildHitTooltip(const HitResult& hit) const;

    /** Build the display text of the hit element ("series name: value")
    */
    std::string BuildHitLabel(const HitResult& hit) const;

    /** Fire the data point click event (packing the hit result into wParam)
    */
    void SendPointClickEvent(const HitResult& hit);

    /** Fire the data point hover event
    */
    void SendPointHoverEvent(const HitResult& hit);

    /** Get the angle of the hit sector (pie chart, returns -1 when nothing is hit)
    *   @param ptMouse The mouse position (control coordinates)
    *   @param rcPie The bounding rectangle of the pie chart
    *   @param nCenterX/nCenterY The center of the circle
    *   @param fRadius The radius
    *   @param data The data
    *   @param dSum The sum of the data (positive values)
    */
    int32_t HitTestPieSlice(const UiPoint& ptMouse, const UiRect& rcPie,
                            int32_t nCenterX, int32_t nCenterY, float fRadius,
                            const std::vector<double>& data, double dSum) const;

private:
    /** Chart type
    */
    ChartType m_chartType;

    /** Chart title
    */
    std::string m_strTitle;

    /** X axis title
    */
    std::string m_strXAxisTitle;

    /** Y axis title
    */
    std::string m_strYAxisTitle;

    /** Language ID of the chart title (empty means unused)
    */
    std::string m_strTitleId;

    /** Language ID of the X axis title
    */
    std::string m_strXAxisTitleId;

    /** Language ID of the Y axis title
    */
    std::string m_strYAxisTitleId;

    /** Language ID of the category labels (multiple comma-separated STRIDs)
    */
    std::string m_strDataLabelsId;

    /** Category labels (X axis categories, shared by all series)
    */
    std::vector<std::string> m_dataLabels;

    /** Multi-series data (the single-series interface also maps to m_series[0])
    */
    std::vector<Series> m_series;

    /** Main color of the series (the default color of a single series, semantic color name)
    */
    std::string m_strSeriesColor;

    /** Axis/grid line color (semantic color name)
    */
    std::string m_strAxisColor;

    /** Text label color (semantic color name)
    */
    std::string m_strLabelColor;

    /** Whether to show the data value labels
    */
    bool m_bShowValue;

    /** Whether to show the legend
    */
    bool m_bLegendVisible;

    /** Whether to show the grid lines
    */
    bool m_bShowGrid;

    /** Whether to show the Y axis tick values
    */
    bool m_bShowAxisValues;

    /** Number of Y axis divisions
    */
    int32_t m_nAxisDivisions;

    /** Line width
    */
    int32_t m_nLineWidth;

    /** Line drawing mode
    */
    ChartLineMode m_lineMode;

    /** Whether to fill the area below the line
    */
    bool m_bAreaFill;

    /** Whether to draw the data point markers
    */
    bool m_bShowDataPoints;

    /** Bar arrangement mode
    */
    ChartBarMode m_barMode;

    /** Whether the pie chart is a donut
    */
    bool m_bDonut;

    /** Whether the pie chart shows percentages
    */
    bool m_bShowPercent;

    /** Corner radius of the bars (pixels, before DPI scaling)
    */
    int32_t m_nBarRadius;

    /** Whether the bars use a gradient fill
    */
    bool m_bBarGradient;

    /** Whether the line chart draws a glow/shadow
    */
    bool m_bLineGlow;

    /** Whether the data update animation is enabled
    */
    bool m_bAnimationEnabled;

    /** Duration of the data update animation (milliseconds)
    */
    int32_t m_nAnimationDuration;

    /** Animation player of the data update
    */
    std::unique_ptr<AnimationPlayer> m_pAnimPlayer;

    /** Animation progress value (0~100)
    */
    int32_t m_nAnimProgress;

    /** Whether the data point tooltip is enabled
    */
    bool m_bEnableTooltip;

    /** Whether click selection is enabled
    */
    bool m_bEnableSelect;

    /** Currently selected element (type == kNone means nothing is selected)
    */
    HitResult m_selected;

    /** Element hit by the previous hover (used to decide whether the tooltip must be updated)
    */
    HitResult m_lastHover;

    /** Line zoom/pan: visible window (the floating range [0, N-1] of the data indexes)
    */
    double m_viewStart;

    /** End index of the line chart visible window
    */
    double m_viewEnd;

    /** Whether a drag-to-pan is in progress
    */
    bool m_bPanning;

    /** Whether the drag actually produced a movement (used to distinguish "pressed" from "dragged")
    */
    bool m_bPanMoved;

    /** Mouse X where the drag started (control coordinates)
    */
    int32_t m_panStartX;

    /** Visible window start when the drag started
    */
    double m_panStartView;

    /** Y axis tick label formatting callback (the default FormatValue is used when empty)
     */
    AxisLabelFormatter m_yAxisLabelFormatter;

    /** X axis category label formatting callback (the data_labels string is used when empty)
     */
    AxisLabelFormatter m_xAxisLabelFormatter;

    /** Maximum number of data points of the rolling window (0 means unlimited)
     */
    size_t m_nMaxDataPoints;
};

} // namespace ui

#endif // UI_CONTROL_CHART_H_
