#include "dui/Control/Chart.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Render/IRender.h"
#include "dui/Render/AutoClip.h"
#include "dui/Utils/StringUtil.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace ui
{

namespace
{
    /** Default line width
    */
    constexpr int32_t kDefaultLineWidth = 2;

    /** Default number of divisions on the Y axis
    */
    constexpr int32_t kDefaultAxisDivisions = 4;

    /** Pi
    */
    constexpr double kPi = 3.14159265358979323846;

    /** Automatic series colors (evenly distributed hues, 8 colors cycling)
    */
    const UiColor kAutoSeriesColors[] = {
        UiColor(0xFF4F8BFF), // Blue
        UiColor(0xFF34C77B), // Green
        UiColor(0xFFFF9F43), // Orange
        UiColor(0xFFE74C3C), // Red
        UiColor(0xFF9B59B6), // Purple
        UiColor(0xFF16A085), // Cyan
        UiColor(0xFFF39C12), // Yellow
        UiColor(0xFF34495E), // Dark blue-gray
    };
    constexpr size_t kAutoSeriesColorCount = sizeof(kAutoSeriesColors) / sizeof(kAutoSeriesColors[0]);
}

Chart::Chart(Window* pWindow) :
    BaseClass(pWindow),
    m_chartType(ChartType::kLine),
    m_strSeriesColor("color_accent"),
    m_strAxisColor("border_control_normal"),
    m_strLabelColor("text_default"),
    m_bShowValue(true),
    m_bLegendVisible(false),
    m_bShowGrid(true),
    m_bShowAxisValues(true),
    m_nAxisDivisions(kDefaultAxisDivisions),
    m_nLineWidth(kDefaultLineWidth),
    m_lineMode(ChartLineMode::kStraight),
    m_bAreaFill(false),
    m_bShowDataPoints(true),
    m_barMode(ChartBarMode::kGrouped),
    m_bDonut(false),
    m_bShowPercent(false),
    m_nBarRadius(3),
    m_bBarGradient(true),
    m_bLineGlow(false),
    m_bAnimationEnabled(true),
    m_nAnimationDuration(300),
    m_nAnimProgress(100),
    m_bEnableTooltip(true),
    m_bEnableSelect(true),
    m_viewStart(0.0),
    m_viewEnd(-1.0),
    m_bPanning(false),
    m_bPanMoved(false),
    m_panStartX(0),
    m_panStartView(0.0),
    m_nMaxDataPoints(0)
{
    // Line/bar charts hide the legend by default (it is meaningless for a single series); the pie chart shows it by default
    // The legend default is decided by chart_type in SetAttribute; false is used here, and the pie chart default is enabled explicitly by the caller after construction
    // To avoid ambiguity, the pie chart legend is enabled automatically while parsing chart_type (see SetAttribute)
}

Chart::~Chart()
{
}

std::string Chart::GetType() const
{
    return DUI_CTR_CHART;
}

void Chart::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "chart_type") {
        if (strValue == "bar") {
            SetChartType(ChartType::kBar);
        }
        else if (strValue == "pie") {
            SetChartType(ChartType::kPie);
        }
        else {
            SetChartType(ChartType::kLine);
        }
    }
    else if (strName == "data") {
        std::vector<double> data;
        ParseDataString(strValue, data);
        SetData(data);
    }
    else if (strName == "data_labels") {
        std::vector<std::string> labels;
        ParseLabelsString(strValue, labels);
        SetDataLabels(labels);
    }
    else if ((strName == "series_color") || (strName == "seriescolor")) {
        SetSeriesColor(strValue);
    }
    else if ((strName == "axis_color") || (strName == "axiscolor")) {
        SetAxisColor(strValue);
    }
    else if ((strName == "label_color") || (strName == "labelcolor")) {
        SetLabelColor(strValue);
    }
    else if (strName == "title") {
        SetTitle(strValue);
    }
    else if ((strName == "x_axis_title") || (strName == "xaxistitle")) {
        SetXAxisTitle(strValue);
    }
    else if ((strName == "y_axis_title") || (strName == "yaxistitle")) {
        SetYAxisTitle(strValue);
    }
    else if ((strName == "title_id") || (strName == "titleid")) {
        SetTitleId(strValue);
    }
    else if ((strName == "x_axis_title_id") || (strName == "xaxistitleid")) {
        SetXAxisTitleId(strValue);
    }
    else if ((strName == "y_axis_title_id") || (strName == "yaxistitleid")) {
        SetYAxisTitleId(strValue);
    }
    else if ((strName == "data_labels_id") || (strName == "datalabelsid")) {
        SetDataLabelsId(strValue);
    }
    else if (strName == "show_value") {
        SetShowValue((strValue == "true"));
    }
    else if ((strName == "legend_visible") || (strName == "legendvisible")) {
        SetLegendVisible((strValue == "true"));
    }
    else if ((strName == "show_grid") || (strName == "showgrid")) {
        SetShowGrid((strValue == "true"));
    }
    else if ((strName == "show_axis_values") || (strName == "showaxisvalues")) {
        SetShowAxisValues((strValue == "true"));
    }
    else if ((strName == "axis_divisions") || (strName == "axisdivisions")) {
        SetAxisDivisions(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "line_width") || (strName == "linewidth")) {
        SetLineWidth(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "line_mode") || (strName == "linemode")) {
        if (strValue == "curve") {
            SetLineMode(ChartLineMode::kCurve);
        }
        else {
            SetLineMode(ChartLineMode::kStraight);
        }
    }
    else if ((strName == "area_fill") || (strName == "areafill")) {
        SetAreaFill((strValue == "true"));
    }
    else if ((strName == "show_data_points") || (strName == "showdatapoints")) {
        SetShowDataPoints((strValue == "true"));
    }
    else if ((strName == "bar_mode") || (strName == "barmode")) {
        if (strValue == "stacked") {
            SetBarMode(ChartBarMode::kStacked);
        }
        else {
            SetBarMode(ChartBarMode::kGrouped);
        }
    }
    else if (strName == "donut") {
        SetDonut((strValue == "true"));
    }
    else if ((strName == "show_percent") || (strName == "showpercent")) {
        SetShowPercent((strValue == "true"));
    }
    else if ((strName == "bar_radius") || (strName == "barradius")) {
        SetBarRadius(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "bar_gradient") || (strName == "bargradient")) {
        SetBarGradient((strValue == "true"));
    }
    else if ((strName == "line_glow") || (strName == "lineglow")) {
        SetLineGlow((strValue == "true"));
    }

    else if ((strName == "animation_enabled") || (strName == "animationenabled")) {
        SetAnimationEnabled((strValue == "true"));
    }
    else if ((strName == "animation_duration") || (strName == "animationduration")) {
        SetAnimationDuration(StringUtil::StringToInt32(strValue));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void Chart::SetChartType(ChartType chartType)
{
    if (m_chartType != chartType) {
        m_chartType = chartType;
        // The pie chart enables the legend by default; line and bar charts disable it by default
        if (chartType == ChartType::kPie) {
            m_bLegendVisible = true;
        }
        // Type switch: the meaning of a hit type changes, so clear the selection and the hovering
        m_selected = HitResult();
        m_lastHover = HitResult();
        Invalidate();
    }
}

void Chart::SetTitle(const std::string& strTitle)
{
    if (m_strTitle != strTitle) {
        m_strTitle = strTitle;
        Invalidate();
    }
}

void Chart::SetXAxisTitle(const std::string& strTitle)
{
    if (m_strXAxisTitle != strTitle) {
        m_strXAxisTitle = strTitle;
        Invalidate();
    }
}

void Chart::SetYAxisTitle(const std::string& strTitle)
{
    if (m_strYAxisTitle != strTitle) {
        m_strYAxisTitle = strTitle;
        Invalidate();
    }
}

void Chart::SetTitleId(const std::string& strTitleId)
{
    m_strTitleId = strTitleId;
    if (!m_strTitleId.empty()) {
        SetTitle(GlobalManager::Instance().Lang().GetStringViaID(m_strTitleId));
    }
}

void Chart::SetXAxisTitleId(const std::string& strTitleId)
{
    m_strXAxisTitleId = strTitleId;
    if (!m_strXAxisTitleId.empty()) {
        SetXAxisTitle(GlobalManager::Instance().Lang().GetStringViaID(m_strXAxisTitleId));
    }
}

void Chart::SetYAxisTitleId(const std::string& strTitleId)
{
    m_strYAxisTitleId = strTitleId;
    if (!m_strYAxisTitleId.empty()) {
        SetYAxisTitle(GlobalManager::Instance().Lang().GetStringViaID(m_strYAxisTitleId));
    }
}

void Chart::SetDataLabelsId(const std::string& strLabelsId)
{
    m_strDataLabelsId = strLabelsId;
    if (m_strDataLabelsId.empty()) {
        return;
    }
    // Multiple comma-separated STRIDs, resolved one by one from the language file into category labels
    std::vector<std::string> labels;
    std::list<std::string> parts = StringUtil::Split(m_strDataLabelsId, ",");
    for (const std::string& part : parts) {
        std::string s = part;
        StringUtil::Trim(s);
        if (s.empty()) {
            continue;
        }
        labels.push_back(GlobalManager::Instance().Lang().GetStringViaID(s));
    }
    SetDataLabels(labels);
}

void Chart::OnLanguageChanged()
{
    BaseClass::OnLanguageChanged();
    // Re-resolve the title/axis titles/category labels from the language IDs
    if (!m_strTitleId.empty()) {
        SetTitle(GlobalManager::Instance().Lang().GetStringViaID(m_strTitleId));
    }
    if (!m_strXAxisTitleId.empty()) {
        SetXAxisTitle(GlobalManager::Instance().Lang().GetStringViaID(m_strXAxisTitleId));
    }
    if (!m_strYAxisTitleId.empty()) {
        SetYAxisTitle(GlobalManager::Instance().Lang().GetStringViaID(m_strYAxisTitleId));
    }
    if (!m_strDataLabelsId.empty()) {
        SetDataLabelsId(m_strDataLabelsId);
    }
}

void Chart::SetData(const std::vector<double>& data)
{
    // Single-series interface: maps to m_series[0]
    if (m_series.empty()) {
        m_series.resize(1);
    }
    m_series[0].data = data;
    if (m_series[0].color.empty()) {
        m_series[0].color = m_strSeriesColor;
    }
    // Data changed: clear the selection and reset the zoom/pan view
    m_selected = HitResult();
    m_viewStart = 0.0;
    m_viewEnd = -1.0;
    StartDataAnimation();
    Invalidate();
}

void Chart::AddData(double value)
{
    if (m_series.empty()) {
        m_series.resize(1);
    }
    if (m_series[0].color.empty()) {
        m_series[0].color = m_strSeriesColor;
    }
    m_series[0].data.push_back(value);
    // Rolling window: when the limit is exceeded, the oldest points are dropped (consistent with AddDataPoint)
    if ((m_nMaxDataPoints > 0) && (m_series[0].data.size() > m_nMaxDataPoints)) {
        const size_t nDrop = m_series[0].data.size() - m_nMaxDataPoints;
        m_series[0].data.erase(m_series[0].data.begin(),
                               m_series[0].data.begin() + static_cast<std::ptrdiff_t>(nDrop));
    }
    StartDataAnimation();
    Invalidate();
}

void Chart::ClearData()
{
    m_series.clear();
    m_selected = HitResult();
    m_viewStart = 0.0;
    m_viewEnd = -1.0;
    Invalidate();
}

const std::vector<double>& Chart::GetData() const
{
    static const std::vector<double> kEmptyData;
    if (m_series.empty()) {
        return kEmptyData;
    }
    return m_series[0].data;
}

void Chart::SetDataLabels(const std::vector<std::string>& labels)
{
    m_dataLabels = labels;
    Invalidate();
}

void Chart::SetSeriesData(const std::vector<Series>& series)
{
    m_series = series;
    // Data changed: clear the selection and reset the zoom/pan view
    m_selected = HitResult();
    m_viewStart = 0.0;
    m_viewEnd = -1.0;
    StartDataAnimation();
    Invalidate();
}

void Chart::AddSeries(const Series& series)
{
    m_series.push_back(series);
    StartDataAnimation();
    Invalidate();
}

void Chart::SetYAxisLabelFormatter(const AxisLabelFormatter& callback)
{
    m_yAxisLabelFormatter = callback;
    Invalidate();
}

void Chart::SetXAxisLabelFormatter(const AxisLabelFormatter& callback)
{
    m_xAxisLabelFormatter = callback;
    Invalidate();
}

bool Chart::AddDataPoint(size_t seriesIndex, double value)
{
    if (seriesIndex >= m_series.size()) {
        return false;
    }
    if (m_series[seriesIndex].color.empty()) {
        m_series[seriesIndex].color = m_strSeriesColor;
    }
    m_series[seriesIndex].data.push_back(value);
    // Rolling window: when the limit is exceeded, the oldest points are dropped
    if ((m_nMaxDataPoints > 0) && (m_series[seriesIndex].data.size() > m_nMaxDataPoints)) {
        const size_t nDrop = m_series[seriesIndex].data.size() - m_nMaxDataPoints;
        m_series[seriesIndex].data.erase(
            m_series[seriesIndex].data.begin(),
            m_series[seriesIndex].data.begin() + static_cast<std::ptrdiff_t>(nDrop));
    }
    StartDataAnimation();
    Invalidate();
    return true;
}

bool Chart::InsertDataPoint(size_t seriesIndex, size_t dataIndex, double value)
{
    if (seriesIndex >= m_series.size()) {
        return false;
    }
    std::vector<double>& data = m_series[seriesIndex].data;
    if (dataIndex > data.size()) {
        dataIndex = data.size(); // Insertion at the end is allowed (equivalent to an append)
    }
    if (m_series[seriesIndex].color.empty()) {
        m_series[seriesIndex].color = m_strSeriesColor;
    }
    data.insert(data.begin() + static_cast<std::ptrdiff_t>(dataIndex), value);
    if ((m_nMaxDataPoints > 0) && (data.size() > m_nMaxDataPoints)) {
        const size_t nDrop = data.size() - m_nMaxDataPoints;
        data.erase(data.begin(), data.begin() + static_cast<std::ptrdiff_t>(nDrop));
    }
    StartDataAnimation();
    Invalidate();
    return true;
}

bool Chart::SetDataPoint(size_t seriesIndex, size_t dataIndex, double value)
{
    if (seriesIndex >= m_series.size()) {
        return false;
    }
    std::vector<double>& data = m_series[seriesIndex].data;
    if (dataIndex >= data.size()) {
        return false;
    }
    data[dataIndex] = value;
    // In-place update: it does not trigger the growth animation, only a repaint
    Invalidate();
    return true;
}

bool Chart::RemoveDataPoint(size_t seriesIndex, size_t dataIndex)
{
    if (seriesIndex >= m_series.size()) {
        return false;
    }
    std::vector<double>& data = m_series[seriesIndex].data;
    if (dataIndex >= data.size()) {
        return false;
    }
    data.erase(data.begin() + static_cast<std::ptrdiff_t>(dataIndex));
    StartDataAnimation();
    Invalidate();
    return true;
}

double Chart::GetDataPoint(size_t seriesIndex, size_t dataIndex) const
{
    if (seriesIndex >= m_series.size()) {
        return 0.0;
    }
    const std::vector<double>& data = m_series[seriesIndex].data;
    if (dataIndex >= data.size()) {
        return 0.0;
    }
    return data[dataIndex];
}

const std::vector<double>& Chart::GetSeriesData(size_t seriesIndex) const
{
    static const std::vector<double> kEmptyData;
    if (seriesIndex >= m_series.size()) {
        return kEmptyData;
    }
    return m_series[seriesIndex].data;
}

bool Chart::ClearSeries(size_t seriesIndex)
{
    if (seriesIndex >= m_series.size()) {
        return false;
    }
    m_series[seriesIndex].data.clear();
    m_selected = HitResult();
    Invalidate();
    return true;
}

bool Chart::RemoveSeries(size_t seriesIndex)
{
    if (seriesIndex >= m_series.size()) {
        return false;
    }
    m_series.erase(m_series.begin() + static_cast<std::ptrdiff_t>(seriesIndex));
    m_selected = HitResult();
    Invalidate();
    return true;
}

void Chart::SetMaxDataPoints(size_t nMax)
{
    m_nMaxDataPoints = nMax;
    if (nMax > 0) {
        for (auto& s : m_series) {
            if (s.data.size() > nMax) {
                const size_t nDrop = s.data.size() - nMax;
                s.data.erase(s.data.begin(), s.data.begin() + static_cast<std::ptrdiff_t>(nDrop));
            }
        }
        Invalidate();
    }
}

void Chart::SetSeriesColor(const std::string& strColor)
{
    m_strSeriesColor = strColor;
    // Synchronize with the first series (when its color was not set explicitly or is empty)
    if (!m_series.empty() && (m_series[0].color.empty() || m_series[0].color == "color_accent")) {
        m_series[0].color = strColor;
    }
    Invalidate();
}

void Chart::SetAxisColor(const std::string& strColor)
{
    m_strAxisColor = strColor;
    Invalidate();
}

void Chart::SetLabelColor(const std::string& strColor)
{
    m_strLabelColor = strColor;
    Invalidate();
}

void Chart::SetShowValue(bool bShowValue)
{
    if (m_bShowValue != bShowValue) {
        m_bShowValue = bShowValue;
        Invalidate();
    }
}

void Chart::SetLegendVisible(bool bLegendVisible)
{
    if (m_bLegendVisible != bLegendVisible) {
        m_bLegendVisible = bLegendVisible;
        Invalidate();
    }
}

void Chart::SetShowGrid(bool bShowGrid)
{
    if (m_bShowGrid != bShowGrid) {
        m_bShowGrid = bShowGrid;
        Invalidate();
    }
}

void Chart::SetShowAxisValues(bool bShow)
{
    if (m_bShowAxisValues != bShow) {
        m_bShowAxisValues = bShow;
        Invalidate();
    }
}

void Chart::SetAxisDivisions(int32_t nDivisions)
{
    if (nDivisions < 1) {
        nDivisions = 1;
    }
    if (nDivisions > 10) {
        nDivisions = 10;
    }
    if (m_nAxisDivisions != nDivisions) {
        m_nAxisDivisions = nDivisions;
        Invalidate();
    }
}

void Chart::SetLineWidth(int32_t nLineWidth)
{
    if (nLineWidth < 1) {
        nLineWidth = 1;
    }
    if (m_nLineWidth != nLineWidth) {
        m_nLineWidth = nLineWidth;
        Invalidate();
    }
}

void Chart::SetLineMode(ChartLineMode lineMode)
{
    if (m_lineMode != lineMode) {
        m_lineMode = lineMode;
        Invalidate();
    }
}

void Chart::SetAreaFill(bool bAreaFill)
{
    if (m_bAreaFill != bAreaFill) {
        m_bAreaFill = bAreaFill;
        Invalidate();
    }
}

void Chart::SetShowDataPoints(bool bShow)
{
    if (m_bShowDataPoints != bShow) {
        m_bShowDataPoints = bShow;
        Invalidate();
    }
}

void Chart::SetBarMode(ChartBarMode barMode)
{
    if (m_barMode != barMode) {
        m_barMode = barMode;
        Invalidate();
    }
}

void Chart::SetDonut(bool bDonut)
{
    if (m_bDonut != bDonut) {
        m_bDonut = bDonut;
        Invalidate();
    }
}

void Chart::SetShowPercent(bool bShowPercent)
{
    if (m_bShowPercent != bShowPercent) {
        m_bShowPercent = bShowPercent;
        Invalidate();
    }
}

void Chart::SetBarRadius(int32_t nRadius)
{
    if (nRadius < 0) {
        nRadius = 0;
    }
    if (m_nBarRadius != nRadius) {
        m_nBarRadius = nRadius;
        Invalidate();
    }
}

void Chart::SetBarGradient(bool bGradient)
{
    if (m_bBarGradient != bGradient) {
        m_bBarGradient = bGradient;
        Invalidate();
    }
}

void Chart::SetLineGlow(bool bGlow)
{
    if (m_bLineGlow != bGlow) {
        m_bLineGlow = bGlow;
        Invalidate();
    }
}

void Chart::SetAnimationEnabled(bool bEnabled)
{
    m_bAnimationEnabled = bEnabled;
    if (!bEnabled) {
        m_pAnimPlayer.reset();
        m_nAnimProgress = 100;
    }
}

void Chart::SetAnimationDuration(int32_t nMs)
{
    if (nMs < 0) {
        nMs = 0;
    }
    m_nAnimationDuration = nMs;
}

void Chart::StartDataAnimation()
{
    if (!m_bAnimationEnabled || (m_nAnimationDuration <= 0) || !IsInited()) {
        m_nAnimProgress = 100;
        return;
    }
    // Stop the old animation
    if ((m_pAnimPlayer != nullptr) && m_pAnimPlayer->IsPlaying()) {
        m_pAnimPlayer->SetCompleteCallback(nullptr);
        m_pAnimPlayer->Stop();
    }
    m_nAnimProgress = 0;
    m_pAnimPlayer.reset(new AnimationPlayer);
    m_pAnimPlayer->SetAnimationType(AnimationType::kAnimationNone);
    m_pAnimPlayer->SetTotalMillSeconds(m_nAnimationDuration);
    m_pAnimPlayer->SetFrameIntervalMillSeconds(16);
    m_pAnimPlayer->SetEasingFunctionType(EaseOutQuad);
    m_pAnimPlayer->SetStartValue(0);
    m_pAnimPlayer->SetEndValue(100);

    std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
    m_pAnimPlayer->SetPlayCallback([this, weakFlag](int32_t nValue) {
        if (weakFlag.expired()) {
            return;
        }
        m_nAnimProgress = nValue;
        Invalidate();
    });
    m_pAnimPlayer->SetCompleteCallback([this, weakFlag]() {
        if (weakFlag.expired()) {
            return;
        }
        m_nAnimProgress = 100;
        Invalidate();
    });
    m_pAnimPlayer->Start();
}

double Chart::GetAnimProgress() const
{
    return static_cast<double>(m_nAnimProgress) / 100.0;
}

void Chart::ParseDataString(const std::string& strData, std::vector<double>& data)
{
    data.clear();
    if (strData.empty()) {
        return;
    }
    std::list<std::string> parts = StringUtil::Split(strData, ",");
    for (const std::string& part : parts) {
        std::string s = part;
        StringUtil::Trim(s);
        if (s.empty()) {
            continue;
        }
        data.push_back(StringUtil::StringToDouble(s));
    }
}

void Chart::ParseLabelsString(const std::string& strLabels, std::vector<std::string>& labels)
{
    labels.clear();
    if (strLabels.empty()) {
        return;
    }
    std::list<std::string> parts = StringUtil::Split(strLabels, ",");
    for (const std::string& part : parts) {
        std::string s = part;
        StringUtil::Trim(s);
        labels.push_back(s);
    }
}

UiColor Chart::GetSeriesColor(size_t index) const
{
    // Prefer the color explicitly assigned to the series
    if (index < m_series.size() && !m_series[index].color.empty()) {
        UiColor c = GetUiColor(m_series[index].color);
        if (!c.IsEmpty()) {
            return c;
        }
    }
    // Fall back to the default series color
    UiColor c = GetUiColor(m_strSeriesColor);
    if (!c.IsEmpty()) {
        return c;
    }
    // Last resort: assign one automatically
    return MakeSeriesColor(index, m_series.size());
}

UiColor Chart::MakeSeriesColor(size_t index, size_t /*count*/)
{
    return kAutoSeriesColors[index % kAutoSeriesColorCount];
}

UiColor Chart::MakeSliceColor(UiColor baseColor, size_t index, size_t count)
{
    // Generate colors of the same hue family for multiple sectors from a base color by varying the brightness
    if (count <= 1) {
        return baseColor;
    }
    uint8_t a = baseColor.GetA();
    uint8_t r = baseColor.GetR();
    uint8_t g = baseColor.GetG();
    uint8_t b = baseColor.GetB();

    double t = (count <= 1) ? 0.0 : (static_cast<double>(index) / static_cast<double>(count - 1));
    // The brightness varies between 0.65 and 1.0 while keeping the same hue
    double brightness = 1.0 - 0.35 * t;
    r = static_cast<uint8_t>(r * brightness);
    g = static_cast<uint8_t>(g * brightness);
    b = static_cast<uint8_t>(b * brightness);
    return UiColor(UiColor::MakeARGB(a, r, g, b));
}

UiColor Chart::MakeDarkerColor(UiColor baseColor, uint8_t nScale)
{
    // Generate a darker color (nScale is a percentage from 0 to 255, e.g. 180 = about 70% brightness)
    uint8_t r = static_cast<uint8_t>(static_cast<uint32_t>(baseColor.GetR()) * nScale / 255);
    uint8_t g = static_cast<uint8_t>(static_cast<uint32_t>(baseColor.GetG()) * nScale / 255);
    uint8_t b = static_cast<uint8_t>(static_cast<uint32_t>(baseColor.GetB()) * nScale / 255);
    return UiColor(UiColor::MakeARGB(baseColor.GetA(), r, g, b));
}

std::string Chart::FormatValue(double value)
{
    // Choose a suitable precision for the value: integers are shown as-is, decimals keep 2 places
    if (std::fabs(value - std::round(value)) < 1e-9) {
        return StringUtil::Printf("%lld", static_cast<long long>(value));
    }
    return StringUtil::Printf("%.2f", value);
}

std::string Chart::FormatAxisValue(double value, int32_t index) const
{
    // Y axis tick formatting: prefer the callback, otherwise fall back to the default numeric format
    if (m_yAxisLabelFormatter) {
        return m_yAxisLabelFormatter(value, index);
    }
    return FormatValue(value);
}

void Chart::DrawChartText(IRender* pRender, const std::string& strText, const UiRect& rc, uint32_t uFormat)
{
    if ((pRender == nullptr) || strText.empty()) {
        return;
    }
    UiColor textColor = GetUiColor(m_strLabelColor);
    if (textColor.IsEmpty()) {
        textColor = UiColor(0xFF000000);
    }
    IFont* pFont = GetIFontById("system_regular_12");
    DrawStringParam param;
    param.textRect = rc;
    param.dwTextColor = textColor;
    param.pFont = pFont;
    param.uFormat = uFormat;
    pRender->DrawString(strText, param);
}

int32_t Chart::MeasureTextWidth(IRender* pRender, const std::string& strText) const
{
    if ((pRender == nullptr) || strText.empty()) {
        return 0;
    }
    MeasureStringParam param;
    param.pFont = GetIFontById("system_regular_12");
    param.uFormat = TEXT_SINGLELINE;
    UiRect rc = pRender->MeasureString(strText, param);
    return rc.Width();
}

int32_t Chart::EstimateTextWidth(const std::string& strText) const
{
    // Estimate by character count: a full-width character is about one font size wide, a half-width character about 0.6x (conservative)
    if (strText.empty()) {
        return 0;
    }
    const int32_t nFullChar = Dpi().GetScaleInt(13);
    const int32_t nHalfChar = Dpi().GetScaleInt(8);
    int32_t nWidth = 0;
    for (wchar_t ch : strText) {
        if (ch > 0xFF) {
            nWidth += nFullChar;
        }
        else {
            nWidth += nHalfChar;
        }
    }
    return nWidth;
}

int32_t Chart::GetSafeTextWidth(IRender* pRender, const std::string& strText) const
{
    return std::max(MeasureTextWidth(pRender, strText), EstimateTextWidth(strText));
}

void Chart::CalcValueRange(double& dMin, double& dMax) const
{
    // Compute the combined range of all series data (without margins)
    dMin = 0.0;
    dMax = 0.0;
    bool bFirst = true;
    for (const Series& s : m_series) {
        for (double v : s.data) {
            if (bFirst) {
                dMin = dMax = v;
                bFirst = false;
            }
            else {
                dMin = std::min(dMin, v);
                dMax = std::max(dMax, v);
            }
        }
    }
    if (bFirst) {
        dMin = 0.0;
        dMax = 1.0;
    }
    if (dMax <= dMin) {
        dMax = dMin + 1.0;
    }
}

void Chart::CalcNiceAxis(double& dMin, double& dMax, int32_t& nDivisions) const
{
    // Compute the rounded ticks: the step is 1/2/5 x 10^n, which keeps the tick values neat and readable
    double dRange = dMax - dMin;
    if (dRange <= 0.0) {
        dRange = 1.0;
        dMax = dMin + 1.0;
    }
    int32_t nWantDivisions = std::max(1, nDivisions);
    double rough = dRange / nWantDivisions;
    if (rough <= 0.0) {
        rough = 1.0;
    }
    double mag = std::pow(10.0, std::floor(std::log10(rough)));
    double norm = rough / mag;
    double stepMul = (norm <= 1.5) ? 1.0 : (norm <= 3.5) ? 2.0 : (norm <= 7.5) ? 5.0 : 10.0;
    double step = stepMul * mag;
    double niceMin = std::floor(dMin / step) * step;
    double niceMax = std::ceil(dMax / step) * step;
    dMin = niceMin;
    dMax = niceMax;
    int32_t nActual = static_cast<int32_t>(std::round((niceMax - niceMin) / step));
    nDivisions = std::max(1, std::min(nActual, nWantDivisions * 2));
}

int32_t Chart::MeasureAxisValueWidth(IRender* pRender) const
{
    // Measure the maximum width of the Y axis tick texts (using the nice tick values, consistent with the actual painting)
    if (!m_bShowAxisValues || (m_chartType == ChartType::kPie)) {
        return 0;
    }
    double dMin = 0.0;
    double dMax = 0.0;
    CalcValueRange(dMin, dMax);
    double dRange = dMax - dMin;
    dMax = dMax + dRange * 0.1;
    dMin = dMin - dRange * 0.1;
    int32_t nDivisions = m_nAxisDivisions;
    CalcNiceAxis(dMin, dMax, nDivisions);
    dRange = dMax - dMin;

    int32_t nMaxWidth = 0;
    for (int32_t i = 0; i <= nDivisions; ++i) {
        double val = dMax - dRange * (static_cast<double>(i) / static_cast<double>(nDivisions));
        nMaxWidth = std::max(nMaxWidth, MeasureTextWidth(pRender, FormatAxisValue(val, i)));
    }
    return nMaxWidth;
}

UiRect Chart::CalcPlotRect(IRender* pRender, const UiRect& rcChart) const
{
    UiRect rc = rcChart;
    // Top: space for the title
    if (!m_strTitle.empty()) {
        rc.top += Dpi().GetScaleInt(26);
    }
    // Left: space for the Y axis tick values + the Y axis title (reserved dynamically from the actual text width, so that the tick values are not clipped)
    int32_t nLeftMargin = Dpi().GetScaleInt(4);
    int32_t nAxisValueWidth = MeasureAxisValueWidth(pRender);
    if (nAxisValueWidth > 0) {
        nLeftMargin += nAxisValueWidth + Dpi().GetScaleInt(4);
    }
    if (!m_strYAxisTitle.empty()) {
        nLeftMargin += Dpi().GetScaleInt(18);
    }
    rc.left += nLeftMargin;
    // Right: space for the legend (line/bar charts with multiple series)
    if ((m_chartType != ChartType::kPie) && m_bLegendVisible && (m_series.size() > 1)) {
        rc.right -= Dpi().GetScaleInt(110);
    }
    // Bottom: space for the X axis title + the category labels
    rc.bottom -= Dpi().GetScaleInt(20);
    if (!m_strXAxisTitle.empty()) {
        rc.bottom -= Dpi().GetScaleInt(20);
    }
    return rc;
}

void Chart::PaintTitles(IRender* pRender, const UiRect& rcChart, const UiRect& rcPlot)
{
    // Chart title (centered)
    if (!m_strTitle.empty()) {
        UiRect rcTitle;
        rcTitle.left = rcChart.left;
        rcTitle.right = rcChart.right;
        rcTitle.top = rcChart.top;
        rcTitle.bottom = rcChart.top + Dpi().GetScaleInt(24);
        IFont* pTitleFont = GetIFontById("system_bold_14");
        DrawStringParam param;
        param.textRect = rcTitle;
        param.dwTextColor = GetUiColor(m_strLabelColor);
        param.pFont = pTitleFont;
        param.uFormat = TEXT_HCENTER | TEXT_VCENTER | TEXT_SINGLELINE;
        pRender->DrawString(m_strTitle, param);
    }

    // X axis title (centered, at the bottom)
    if (!m_strXAxisTitle.empty()) {
        UiRect rcAxis;
        rcAxis.left = rcPlot.left;
        rcAxis.right = rcPlot.right;
        rcAxis.top = rcPlot.bottom + Dpi().GetScaleInt(20);
        rcAxis.bottom = rcChart.bottom;
        DrawChartText(pRender, m_strXAxisTitle, rcAxis, TEXT_HCENTER | TEXT_VCENTER | TEXT_SINGLELINE);
    }

    // Y axis title (drawn vertically to the left of the tick values, to avoid overlapping them)
    if (!m_strYAxisTitle.empty()) {
        UiRect rcAxis;
        rcAxis.left = rcChart.left + Dpi().GetScaleInt(2);
        rcAxis.right = rcPlot.left - Dpi().GetScaleInt(4);
        if (m_bShowAxisValues && (m_chartType != ChartType::kPie)) {
            // Exclude the tick value area; the title occupies only the leftmost column
            rcAxis.right = rcAxis.left + Dpi().GetScaleInt(16);
            if (rcAxis.right > rcPlot.left - Dpi().GetScaleInt(4)) {
                rcAxis.right = rcPlot.left - Dpi().GetScaleInt(4);
            }
        }
        rcAxis.top = rcPlot.top;
        rcAxis.bottom = rcPlot.bottom;
        DrawChartText(pRender, m_strYAxisTitle, rcAxis, TEXT_HCENTER | TEXT_VCENTER | TEXT_SINGLELINE | TEXT_END_ELLIPSIS | TEXT_VERTICAL);
    }
}

void Chart::PaintStateImages(IRender* pRender)
{
    BaseClass::PaintStateImages(pRender);
    if (pRender == nullptr) {
        return;
    }

    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);
    if ((rc.Width() <= 0) || (rc.Height() <= 0)) {
        return;
    }
    if (m_series.empty()) {
        return;
    }
    // Check whether there is any data
    bool bHasData = false;
    for (const Series& s : m_series) {
        if (!s.data.empty()) {
            bHasData = true;
            break;
        }
    }
    if (!bHasData) {
        return;
    }

    switch (m_chartType) {
    case ChartType::kLine:
        PaintLine(pRender, rc);
        break;
    case ChartType::kBar:
        PaintBar(pRender, rc);
        break;
    case ChartType::kPie:
        PaintPie(pRender, rc);
        break;
    default:
        break;
    }
}

Chart::LineLayout Chart::CalcLineLayout(IRender* pRender, const UiRect& rcChart) const
{
    LineLayout layout;
    layout.rcPlot = CalcPlotRect(pRender, rcChart);
    if ((layout.rcPlot.Width() <= 0) || (layout.rcPlot.Height() <= 0)) {
        return layout;
    }
    // Compute the data range of all series (combined)
    double dMin = 0.0;
    double dMax = 0.0;
    CalcValueRange(dMin, dMax);
    double dRange = dMax - dMin;
    dMax = dMax + dRange * 0.1;
    dMin = dMin - dRange * 0.1;
    int32_t nDiv = m_nAxisDivisions;
    CalcNiceAxis(dMin, dMax, nDiv);
    layout.dMin = dMin;
    layout.dMax = dMax;
    layout.dRange = dMax - dMin;
    layout.nDivisions = std::max(1, nDiv);

    // Maximum number of data points
    size_t nMaxCount = 0;
    for (const Series& s : m_series) {
        nMaxCount = std::max(nMaxCount, s.data.size());
    }
    layout.nMaxCount = nMaxCount;

    // Visible window (used for zooming and panning)
    if (nMaxCount == 0) {
        layout.viewStart = 0.0;
        layout.viewEnd = 0.0;
    }
    else if ((m_viewEnd <= m_viewStart) || (m_viewEnd <= 0.0)) {
        // Not set or already reset: show everything
        layout.viewStart = 0.0;
        layout.viewEnd = (nMaxCount > 1) ? static_cast<double>(nMaxCount - 1) : 0.0;
    }
    else {
        layout.viewStart = m_viewStart;
        layout.viewEnd = m_viewEnd;
        // Clamp
        double dMaxIndex = (nMaxCount > 1) ? static_cast<double>(nMaxCount - 1) : 0.0;
        if (layout.viewStart < 0.0) {
            layout.viewEnd -= layout.viewStart;
            layout.viewStart = 0.0;
        }
        if (layout.viewEnd > dMaxIndex) {
            layout.viewStart -= (layout.viewEnd - dMaxIndex);
            layout.viewEnd = dMaxIndex;
            if (layout.viewStart < 0.0) {
                layout.viewStart = 0.0;
            }
        }
        if (layout.viewEnd <= layout.viewStart) {
            layout.viewStart = 0.0;
            layout.viewEnd = dMaxIndex;
        }
    }
    return layout;
}

void Chart::SetEnableTooltip(bool bEnable)
{
    m_bEnableTooltip = bEnable;
    if (!bEnable) {
        m_lastHover = HitResult();
    }
}

void Chart::SetEnableSelect(bool bEnable)
{
    m_bEnableSelect = bEnable;
    if (!bEnable) {
        m_selected = HitResult();
        Invalidate();
    }
}

void Chart::ClearSelection()
{
    if (m_selected.type != HitType::kNone) {
        m_selected = HitResult();
        Invalidate();
    }
}

void Chart::AttachPointClick(const EventCallback& callback, EventCallbackID callbackID)
{
    AttachEvent(kEventClick, callback, callbackID);
}

void Chart::AttachPointHover(const EventCallback& callback, EventCallbackID callbackID)
{
    AttachEvent(kEventMouseHover, callback, callbackID);
}

Chart::HitResult Chart::GetHitFromEvent(const EventArgs& args)
{
    HitResult result;
    if (args.wParam != 0) {
        const HitResult* pHit = reinterpret_cast<const HitResult*>(args.wParam);
        if (pHit != nullptr) {
            result = *pHit;
        }
    }
    return result;
}

void Chart::SendPointClickEvent(const HitResult& hit)
{
    EventArgs args;
    args.SetSender(this);
    args.wParam = reinterpret_cast<WPARAM>(&hit);
    SendEvent(kEventClick, args);
}

void Chart::SendPointHoverEvent(const HitResult& hit)
{
    EventArgs args;
    args.SetSender(this);
    args.wParam = reinterpret_cast<WPARAM>(&hit);
    SendEvent(kEventMouseHover, args);
}

std::string Chart::BuildHitLabel(const HitResult& hit) const
{
    std::string strResult;
    if (hit.type == HitType::kLegend) {
        if (hit.seriesIndex < m_series.size()) {
            strResult = m_series[hit.seriesIndex].name;
        }
        return strResult;
    }
    if (hit.seriesIndex >= m_series.size()) {
        return strResult;
    }
    const Series& series = m_series[hit.seriesIndex];
    // Series name
    std::string strName = series.name;
    if (strName.empty()) {
        strName = StringUtil::Printf("Series %zu", hit.seriesIndex + 1);
    }
    // Category
    std::string strCategory;
    if (hit.dataIndex < m_dataLabels.size()) {
        strCategory = m_dataLabels[hit.dataIndex];
    }
    // Value
    double value = 0.0;
    if (hit.dataIndex < series.data.size()) {
        value = series.data[hit.dataIndex];
    }
    if (!strCategory.empty()) {
        strResult = strName + " / " + strCategory + ": " + FormatValue(value);
    }
    else {
        strResult = strName + ": " + FormatValue(value);
    }
    return strResult;
}

std::string Chart::BuildHitTooltip(const HitResult& hit) const
{
    // Pie sector/bar/line point: show "series name\ncategory: value"
    if (hit.type == HitType::kSlice || hit.type == HitType::kBar || hit.type == HitType::kPoint) {
        return BuildHitLabel(hit);
    }
    if (hit.type == HitType::kLegend) {
        return BuildHitLabel(hit);
    }
    return std::string();
}

int32_t Chart::HitTestPieSlice(const UiPoint& ptMouse, const UiRect& /*rcPie*/,
                               int32_t nCenterX, int32_t nCenterY, float fRadius,
                               const std::vector<double>& data, double dSum) const
{
    // First check whether it is inside the circle (including the area outside the inner ring of a donut)
    double dx = static_cast<double>(ptMouse.x - nCenterX);
    double dy = static_cast<double>(ptMouse.y - nCenterY);
    double dist = std::sqrt(dx * dx + dy * dy);
    if (dist > fRadius) {
        return -1;
    }
    if (m_bDonut) {
        float fInnerRadius = fRadius * 0.55f;
        if (dist < fInnerRadius) {
            return -1;
        }
    }
    // Compute the angle (starting from the top, i.e. 270 degrees, clockwise)
    double angleDeg = std::atan2(dy, dx) * 180.0 / kPi; // -180..180, 0 = right, 90 = down
    if (angleDeg < 0.0) {
        angleDeg += 360.0;
    }
    // Convert to an angle measured clockwise from 270 degrees
    double relAngle = angleDeg - 270.0;
    if (relAngle < 0.0) {
        relAngle += 360.0;
    }
    // Iterate over the sectors
    double fStartAngle = 0.0;
    for (size_t i = 0; i < data.size(); ++i) {
        double v = data[i];
        if (v <= 0.0) {
            continue;
        }
        double fSweep = 360.0 * (v / dSum);
        if (relAngle >= fStartAngle && relAngle < (fStartAngle + fSweep)) {
            return static_cast<int32_t>(i);
        }
        fStartAngle += fSweep;
    }
    return -1;
}

Chart::HitResult Chart::HitTest(IRender* pRender, const UiPoint& ptMouse)
{
    HitResult hit;
    if (pRender == nullptr) {
        return hit;
    }
    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);

    if (m_chartType == ChartType::kLine) {
        LineLayout layout = CalcLineLayout(pRender, rc);
        if ((layout.rcPlot.Width() <= 0) || (layout.rcPlot.Height() <= 0)) {
            return hit;
        }
        // Iterate over the visible data points of all series and find the closest hit
        const int32_t nHitRadius = Dpi().GetScaleInt(8);
        double dBestDist = nHitRadius * nHitRadius;
        for (size_t si = 0; si < m_series.size(); ++si) {
            const std::vector<double>& data = m_series[si].data;
            if (data.empty()) {
                continue;
            }
            for (size_t i = 0; i < data.size(); ++i) {
                double idx = static_cast<double>(i);
                if ((idx < layout.viewStart) || (idx > layout.viewEnd)) {
                    continue; // Invisible data point
                }
                float fx = layout.XToPixel(idx);
                float fy = layout.YToPixel(data[i]);
                double dx = static_cast<double>(ptMouse.x) - fx;
                double dy = static_cast<double>(ptMouse.y) - fy;
                double dist = dx * dx + dy * dy;
                if (dist <= dBestDist) {
                    dBestDist = dist;
                    hit.type = HitType::kPoint;
                    hit.seriesIndex = si;
                    hit.dataIndex = i;
                    hit.ptHit = UiPoint(static_cast<int32_t>(fx), static_cast<int32_t>(fy));
                }
            }
        }
        return hit;
    }

    if (m_chartType == ChartType::kBar) {
        // Bar chart hit test: reuse the layout computation performed while painting
        UiRect rcPlot = CalcPlotRect(pRender, rc);
        if ((rcPlot.Width() <= 0) || (rcPlot.Height() <= 0)) {
            return hit;
        }
        if (m_series.empty()) {
            return hit;
        }
        // Compute the data range (including the stacking logic, consistent with PaintBar)
        double dMin = 0.0;
        double dMax = 0.0;
        CalcValueRange(dMin, dMax);
        if (m_barMode == ChartBarMode::kStacked) {
            dMin = 0.0;
            dMax = 0.0;
            size_t nCount = 0;
            for (const Series& s : m_series) {
                nCount = std::max(nCount, s.data.size());
            }
            for (size_t i = 0; i < nCount; ++i) {
                double posSum = 0.0;
                double negSum = 0.0;
                for (const Series& s : m_series) {
                    if (i < s.data.size()) {
                        double v = s.data[i];
                        if (v >= 0.0) {
                            posSum += v;
                        }
                        else {
                            negSum += v;
                        }
                    }
                }
                dMax = std::max(dMax, posSum);
                dMin = std::min(dMin, negSum);
            }
        }
        if (dMax <= dMin) {
            dMax = dMin + 1.0;
        }
        double dRange = dMax - dMin;
        dMax = dMax + dRange * 0.05;
        dMin = dMin - dRange * 0.05;
        int32_t nDiv = m_nAxisDivisions;
        CalcNiceAxis(dMin, dMax, nDiv);
        dRange = dMax - dMin;

        const int32_t nLeft = rcPlot.left;
        const int32_t nRight = rcPlot.right;
        const int32_t nTop = rcPlot.top;
        const int32_t nBottom = rcPlot.bottom;
        const float fPlotWidth = static_cast<float>(nRight - nLeft);
        const float fPlotHeight = static_cast<float>(nBottom - nTop);
        double zeroNorm = (0.0 - dMin) / dRange;
        float fZeroY = static_cast<float>(nBottom) - fPlotHeight * static_cast<float>(zeroNorm);
        fZeroY = std::max(static_cast<float>(nTop), std::min(fZeroY, static_cast<float>(nBottom)));

        size_t nCategoryCount = 0;
        for (const Series& s : m_series) {
            nCategoryCount = std::max(nCategoryCount, s.data.size());
        }
        if (nCategoryCount == 0) {
            return hit;
        }
        const size_t nSeriesCount = m_series.size();
        const float fSlotWidth = fPlotWidth / static_cast<float>(nCategoryCount);

        if (m_barMode == ChartBarMode::kStacked) {
            const float fBarWidth = fSlotWidth * 0.6f;
            for (size_t i = 0; i < nCategoryCount; ++i) {
                float fCenterX = static_cast<float>(nLeft) + fSlotWidth * (static_cast<float>(i) + 0.5f);
                float fBarLeft = fCenterX - fBarWidth / 2.0f;
                float fBarRight = fBarLeft + fBarWidth;
                if ((ptMouse.x >= fBarLeft) && (ptMouse.x <= fBarRight)) {
                    // Hit this bar, determine the series (top-down for positive stacking, bottom-up for negative stacking)
                    float posTop = fZeroY;
                    for (size_t si = 0; si < nSeriesCount; ++si) {
                        if (i >= m_series[si].data.size()) {
                            continue;
                        }
                        double v = m_series[si].data[i];
                        if (v >= 0.0) {
                            float h = fPlotHeight * static_cast<float>(v / dRange);
                            float segTop = posTop - h;
                            if ((ptMouse.y >= segTop) && (ptMouse.y <= posTop)) {
                                hit.type = HitType::kBar;
                                hit.seriesIndex = si;
                                hit.dataIndex = i;
                                hit.ptHit = UiPoint(static_cast<int32_t>(fCenterX), static_cast<int32_t>((segTop + posTop) / 2.0f));
                                return hit;
                            }
                            posTop = segTop;
                        }
                    }
                    float negBottom = fZeroY;
                    for (size_t si = 0; si < nSeriesCount; ++si) {
                        if (i >= m_series[si].data.size()) {
                            continue;
                        }
                        double v = m_series[si].data[i];
                        if (v < 0.0) {
                            float h = fPlotHeight * static_cast<float>((-v) / dRange);
                            float segBottom = negBottom + h;
                            if ((ptMouse.y >= negBottom) && (ptMouse.y <= segBottom)) {
                                hit.type = HitType::kBar;
                                hit.seriesIndex = si;
                                hit.dataIndex = i;
                                hit.ptHit = UiPoint(static_cast<int32_t>(fCenterX), static_cast<int32_t>((negBottom + segBottom) / 2.0f));
                                return hit;
                            }
                            negBottom = segBottom;
                        }
                    }
                    return hit;
                }
            }
        }
        else {
            const float fGroupWidth = fSlotWidth * 0.8f;
            const float fBarWidth = fGroupWidth / static_cast<float>(nSeriesCount);
            for (size_t si = 0; si < nSeriesCount; ++si) {
                for (size_t i = 0; i < m_series[si].data.size(); ++i) {
                    double v = m_series[si].data[i];
                    float fCenterX = static_cast<float>(nLeft) + fSlotWidth * (static_cast<float>(i) + 0.5f);
                    float fGroupLeft = fCenterX - fGroupWidth / 2.0f;
                    float fBarLeft = fGroupLeft + fBarWidth * static_cast<float>(si);
                    float fBarRight = fBarLeft + fBarWidth;
                    float fBarTop, fBarBottom;
                    if (v >= 0.0) {
                        float h = fPlotHeight * static_cast<float>(v / dRange);
                        fBarTop = fZeroY - h;
                        fBarBottom = fZeroY;
                    }
                    else {
                        float h = fPlotHeight * static_cast<float>((-v) / dRange);
                        fBarTop = fZeroY;
                        fBarBottom = fZeroY + h;
                    }
                    if ((ptMouse.x >= fBarLeft) && (ptMouse.x <= fBarRight) &&
                        (ptMouse.y >= fBarTop) && (ptMouse.y <= fBarBottom)) {
                        hit.type = HitType::kBar;
                        hit.seriesIndex = si;
                        hit.dataIndex = i;
                        hit.ptHit = UiPoint(static_cast<int32_t>(fCenterX), static_cast<int32_t>((fBarTop + fBarBottom) / 2.0f));
                        return hit;
                    }
                }
            }
        }
        return hit;
    }

    if (m_chartType == ChartType::kPie) {
        UiRect rcPlot = CalcPlotRect(pRender, rc);
        if ((rcPlot.Width() <= 0) || (rcPlot.Height() <= 0)) {
            return hit;
        }
        if (m_series.empty() || m_series[0].data.empty()) {
            return hit;
        }
        const std::vector<double>& data = m_series[0].data;
        const size_t nCount = data.size();
        double dSum = 0.0;
        for (double v : data) {
            if (v > 0.0) {
                dSum += v;
            }
        }
        if (dSum <= 0.0) {
            return hit;
        }
        // Compute the legend width (consistent with PaintPie, used to determine the pie area)
        int32_t nLegendWidth = 0;
        if (m_bLegendVisible) {
            const int32_t nColorBoxSize = Dpi().GetScaleInt(12);
            int32_t nMaxTextWidth = 0;
            for (size_t i = 0; i < nCount; ++i) {
                std::string strLabel = (i < m_dataLabels.size()) ? m_dataLabels[i] : std::string();
                if (strLabel.empty()) {
                    strLabel = FormatValue(data[i]);
                }
                else {
                    strLabel = strLabel + "  " + FormatValue(data[i]);
                    if (m_bShowPercent) {
                        strLabel = strLabel + StringUtil::Printf(" (%.1f%%)", data[i] / dSum * 100.0);
                    }
                }
                nMaxTextWidth = std::max(nMaxTextWidth, GetSafeTextWidth(pRender, strLabel));
            }
            nLegendWidth = nColorBoxSize + Dpi().GetScaleInt(6) + nMaxTextWidth + Dpi().GetScaleInt(10);
            const int32_t nMaxLegendWidth = std::max(rcPlot.Width() * 60 / 100, Dpi().GetScaleInt(80));
            nLegendWidth = std::min(nLegendWidth, nMaxLegendWidth);
        }
        UiRect rcPieArea = rcPlot;
        rcPieArea.right -= nLegendWidth;
        const int32_t nSide = std::min(rcPieArea.Width(), rcPieArea.Height());
        const int32_t nCenterX = rcPieArea.left + rcPieArea.Width() / 2;
        const int32_t nCenterY = rcPieArea.top + rcPieArea.Height() / 2;
        UiRect rcPie;
        rcPie.left = nCenterX - nSide / 2;
        rcPie.top = nCenterY - nSide / 2;
        rcPie.right = rcPie.left + nSide;
        rcPie.bottom = rcPie.top + nSide;
        const float fRadius = static_cast<float>(nSide) / 2.0f;

        int32_t nSlice = HitTestPieSlice(ptMouse, rcPie, nCenterX, nCenterY, fRadius, data, dSum);
        if (nSlice >= 0) {
            hit.type = HitType::kSlice;
            hit.seriesIndex = 0;
            hit.dataIndex = static_cast<size_t>(nSlice);
            hit.ptHit = ptMouse;
        }
        return hit;
    }

    return hit;
}

void Chart::PaintLine(IRender* pRender, const UiRect& rcChart)
{
    LineLayout layout = CalcLineLayout(pRender, rcChart);
    UiRect rcPlot = layout.rcPlot;
    PaintTitles(pRender, rcChart, rcPlot);
    if ((rcPlot.Width() <= 0) || (rcPlot.Height() <= 0)) {
        return;
    }

    UiColor axisColor = GetUiColor(m_strAxisColor);
    if (axisColor.IsEmpty()) {
        axisColor = UiColor(0xFFD0D0D0);
    }

    //const double dMin = layout.dMin;
    const double dMax = layout.dMax;
    const double dRange = layout.dRange;
    const int32_t nDiv = layout.nDivisions; // The actual number of divisions after aligning to nice ticks

    const int32_t nLeft = rcPlot.left;
    const int32_t nRight = rcPlot.right;
    const int32_t nTop = rcPlot.top;
    const int32_t nBottom = rcPlot.bottom;
    //const float fPlotWidth = static_cast<float>(nRight - nLeft);
    const float fPlotHeight = static_cast<float>(nBottom - nTop);

    // Data update animation progress (the line fades in from the bottom of the plot area)
    const double dProgress = GetAnimProgress();

    // Width of the Y axis tick value texts (measure the tick values that are actually painted, so that the width matches exactly)
    int32_t nAxisValueWidth = 0;
    if (m_bShowAxisValues) {
        for (int32_t i = 0; i <= nDiv; ++i) {
            double val = dMax - dRange * (static_cast<double>(i) / static_cast<double>(nDiv));
            nAxisValueWidth = std::max(nAxisValueWidth, GetSafeTextWidth(pRender, FormatAxisValue(val, i)));
        }
    }

    // Y axis tick values + grid lines
    if (m_bShowAxisValues || m_bShowGrid) {
        for (int32_t i = 0; i <= nDiv; ++i) {
            float fy = static_cast<float>(nTop) + fPlotHeight * static_cast<float>(i) / static_cast<float>(nDiv);
            if (m_bShowGrid && (i > 0) && (i < nDiv)) {
                pRender->DrawLine(UiPointF(static_cast<float>(nLeft), fy),
                                  UiPointF(static_cast<float>(nRight), fy),
                                  axisColor, 1.0f);
            }
            // Y axis tick values (right-aligned on the left side)
            if (m_bShowAxisValues) {
                double val = dMax - dRange * (static_cast<double>(i) / static_cast<double>(nDiv));
                std::string strVal = FormatAxisValue(val, i);
                UiRect rcText;
                rcText.right = nLeft - Dpi().GetScaleInt(4);
                rcText.left = rcText.right - nAxisValueWidth - Dpi().GetScaleInt(4);
                rcText.top = static_cast<int32_t>(fy) - Dpi().GetScaleInt(8);
                rcText.bottom = rcText.top + Dpi().GetScaleInt(16);
                DrawChartText(pRender, strVal, rcText, TEXT_RIGHT | TEXT_VCENTER | TEXT_SINGLELINE);
            }
        }
    }

    if (layout.nMaxCount == 0) {
        return;
    }

    // Paint series by series (clipped to the plot area: the extra points outside the window that are needed for continuity while zooming/panning are not drawn outside the plot area)
    IRenderFactory* pFactory = GlobalManager::Instance().GetRenderFactory();
    std::unique_ptr<AutoClip> spPlotClip = CreateRectClip(pRender, rcPlot, true);
    for (size_t si = 0; si < m_series.size(); ++si) {
        const std::vector<double>& data = m_series[si].data;
        if (data.empty()) {
            continue;
        }
        UiColor seriesColor = GetSeriesColor(si);
        const size_t nCount = data.size();

        // Compute the coordinates of the points (only the points inside the visible window, plus one extra point at each end to keep the line continuous)
        std::vector<size_t> visibleIdx;
        visibleIdx.reserve(nCount);
        for (size_t i = 0; i < nCount; ++i) {
            double idx = static_cast<double>(i);
            if ((idx >= layout.viewStart) && (idx <= layout.viewEnd)) {
                visibleIdx.push_back(i);
            }
            else if (!visibleIdx.empty() && (visibleIdx.back() == i - 1) && (i > 0)) {
                // Visible window boundaries: add the point immediately outside the window at each end to keep the line continuous
                visibleIdx.push_back(i);
            }
        }
        // If the visible window starts in the middle, add a point to the left of the window start
        if (!visibleIdx.empty() && (visibleIdx.front() > 0)) {
            visibleIdx.insert(visibleIdx.begin(), visibleIdx.front() - 1);
        }
        std::vector<UiPointF> points;
        points.reserve(visibleIdx.size());
        for (size_t i : visibleIdx) {
            float fx = layout.XToPixel(static_cast<double>(i));
            float fyTarget = layout.YToPixel(data[i]);
            // Fade-in animation: interpolate from the bottom of the plot area towards the target value
            float fy = static_cast<float>(nBottom) - (static_cast<float>(nBottom) - fyTarget) * static_cast<float>(dProgress);
            points.emplace_back(fx, fy);
        }
        const size_t nVisible = points.size();
        if (nVisible == 0) {
            continue;
        }

        // Area fill (filled below the line down to the baseline)
        if (m_bAreaFill && (nVisible >= 2) && (pFactory != nullptr)) {
            IPath* pPath = pFactory->CreatePath();
            std::unique_ptr<IPath> spPath(pPath);
            if (pPath != nullptr) {
                float baseY = static_cast<float>(nBottom);
                pPath->AddLine(points[0].x, baseY, points[0].x, points[0].y);
                if (m_lineMode == ChartLineMode::kCurve) {
                    for (size_t i = 0; i + 1 < nVisible; ++i) {
                        float cx1 = points[i].x + (points[i + 1].x - points[i].x) * 0.5f;
                        float cx2 = points[i + 1].x - (points[i + 1].x - points[i].x) * 0.5f;
                        pPath->AddBezier(points[i].x, points[i].y, cx1, points[i].y, cx2, points[i + 1].y, points[i + 1].x, points[i + 1].y);
                    }
                }
                else {
                    for (size_t i = 0; i + 1 < nVisible; ++i) {
                        pPath->AddLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
                    }
                }
                pPath->AddLine(points[nVisible - 1].x, points[nVisible - 1].y, points[nVisible - 1].x, baseY);
                pPath->Close();
                // Semi-transparent fill
                UiColor fillColor = UiColor(UiColor::MakeARGB(64, seriesColor.GetR(), seriesColor.GetG(), seriesColor.GetB()));
                IBrush* pBrush = pFactory->CreateBrush(fillColor);
                std::unique_ptr<IBrush> spBrush(pBrush);
                if (pBrush != nullptr) {
                    pRender->FillPath(pPath, pBrush);
                }
            }
        }

        // Paint the line/curve
        if (nVisible >= 2) {
            const float fLineWidth = static_cast<float>(Dpi().GetScaleInt(m_nLineWidth));
            // Glow/shadow: draw a thicker line of the same color at low opacity first, then overlay the main line
            const bool bGlow = m_bLineGlow && (pFactory != nullptr);
            const float fGlowWidth = fLineWidth * 2.0f + 1.0f;
            const UiColor glowColor = UiColor(UiColor::MakeARGB(48, seriesColor.GetR(), seriesColor.GetG(), seriesColor.GetB()));
            if (m_lineMode == ChartLineMode::kCurve) {
                for (size_t i = 0; i + 1 < nVisible; ++i) {
                    // Smooth with Bezier curves: the control points extend horizontally from the midpoints of adjacent points
                    float cx1 = points[i].x + (points[i + 1].x - points[i].x) * 0.5f;
                    float cx2 = points[i + 1].x - (points[i + 1].x - points[i].x) * 0.5f;
                    if (bGlow) {
                        IPath* pGlowPath = pFactory->CreatePath();
                        std::unique_ptr<IPath> spGlowPath(pGlowPath);
                        IPen* pGlowPen = pFactory->CreatePen(glowColor, fGlowWidth);
                        std::unique_ptr<IPen> spGlowPen(pGlowPen);
                        if ((pGlowPath != nullptr) && (pGlowPen != nullptr)) {
                            pGlowPath->AddBezier(points[i].x, points[i].y, cx1, points[i].y, cx2, points[i + 1].y, points[i + 1].x, points[i + 1].y);
                            pRender->DrawPath(pGlowPath, pGlowPen);
                        }
                    }
                    IPath* pPath = pFactory->CreatePath();
                    std::unique_ptr<IPath> spPath(pPath);
                    if (pPath != nullptr) {
                        pPath->AddBezier(points[i].x, points[i].y, cx1, points[i].y, cx2, points[i + 1].y, points[i + 1].x, points[i + 1].y);
                        IPen* pPen = pFactory->CreatePen(seriesColor, fLineWidth);
                        std::unique_ptr<IPen> spPen(pPen);
                        if (pPen != nullptr) {
                            pRender->DrawPath(pPath, pPen);
                        }
                    }
                }
            }
            else {
                for (size_t i = 0; i + 1 < nVisible; ++i) {
                    if (bGlow) {
                        pRender->DrawLine(points[i], points[i + 1], glowColor, fGlowWidth);
                    }
                    pRender->DrawLine(points[i], points[i + 1], seriesColor, fLineWidth);
                }
            }
        }

        // Data points + value labels (only the points inside the visible window are painted)
        if (m_bShowDataPoints) {
            const float fRadius = static_cast<float>(Dpi().GetScaleInt(3));
            for (size_t i = 0; i < nVisible; ++i) {
                // Points outside the window boundaries do not get a marker (to avoid duplicates or out-of-bounds visuals)
                size_t realIdx = visibleIdx[i];
                if ((realIdx < static_cast<size_t>(std::ceil(layout.viewStart))) ||
                    (realIdx > static_cast<size_t>(std::floor(layout.viewEnd)))) {
                    continue;
                }
                pRender->FillCircle(points[i], fRadius, seriesColor, GetAlpha());
            }
        }
        if (m_bShowValue) {
            // Stagger with multiple series: even series put the label above the point, odd series below, to avoid overlapping each other
            // Thin out by data index interval (instead of pixel-based overlap avoidance): while panning, the label set moves smoothly with the window instead of flickering
            const bool bBelow = (si % 2 == 1);
            double dVisibleCount = std::max(1.0, layout.viewEnd - layout.viewStart + 1.0);
            // The minimum label spacing is about 5 half-width characters wide, used to compute the thinning step
            int32_t nMinGap = Dpi().GetScaleInt(44);
            int32_t nStep = std::max(1, static_cast<int32_t>(std::ceil(dVisibleCount * nMinGap / static_cast<double>(rcPlot.Width()))));
            for (size_t i = 0; i < nVisible; ++i) {
                size_t realIdx = visibleIdx[i];
                if ((realIdx < static_cast<size_t>(std::ceil(layout.viewStart))) ||
                    (realIdx > static_cast<size_t>(std::floor(layout.viewEnd)))) {
                    continue;
                }
                // Index-interval thinning: a fixed index subset that follows smoothly while panning
                if ((realIdx % static_cast<size_t>(nStep)) != 0) {
                    continue;
                }
                std::string strValue = FormatValue(data[realIdx]);
                int32_t nTextWidth = GetSafeTextWidth(pRender, strValue);
                if (nTextWidth <= 0) {
                    nTextWidth = Dpi().GetScaleInt(20);
                }
                // Lay out by the actual text width, shifting only without narrowing (the text stays aligned with the data point and is not squeezed or offset by clamping)
                int32_t nLabelLeft = static_cast<int32_t>(points[i].x) - nTextWidth / 2;
                nLabelLeft = std::max(nLabelLeft, rcPlot.left);
                nLabelLeft = std::min(nLabelLeft, rcPlot.right - nTextWidth);
                UiRect rcText;
                rcText.left = nLabelLeft;
                rcText.right = nLabelLeft + nTextWidth;
                if (bBelow) {
                    rcText.top = static_cast<int32_t>(points[i].y) + Dpi().GetScaleInt(4);
                    rcText.bottom = rcText.top + Dpi().GetScaleInt(18);
                }
                else {
                    rcText.top = static_cast<int32_t>(points[i].y) - Dpi().GetScaleInt(20);
                    rcText.bottom = static_cast<int32_t>(points[i].y) - Dpi().GetScaleInt(2);
                }
                DrawChartText(pRender, strValue, rcText, TEXT_LEFT | (bBelow ? TEXT_TOP : TEXT_BOTTOM) | TEXT_SINGLELINE);
            }
        }
    }
    spPlotClip.reset(); // Restore the clipping: the X axis category labels are painted below the plot area

    // X axis category labels (they follow the visible window; the X axis format callback can override the default data_labels)
    if ((!m_dataLabels.empty()) || m_xAxisLabelFormatter) {
        const size_t nLabelCount = !m_dataLabels.empty()
            ? std::min(m_dataLabels.size(), layout.nMaxCount)
            : layout.nMaxCount;
        int32_t nLabelTop = nBottom + Dpi().GetScaleInt(2);
        int32_t nLabelBottom = nBottom + Dpi().GetScaleInt(18);
        // Compute all label rectangles first (measured width + clamped to the chart area)
        std::vector<UiRect> labelRects(nLabelCount);
        for (size_t i = 0; i < nLabelCount; ++i) {
            double idx = static_cast<double>(i);
            if ((idx < layout.viewStart) || (idx > layout.viewEnd)) {
                labelRects[i] = UiRect(); // Invisible, width is 0
                continue;
            }
            // Label text: prefer the X axis format callback, otherwise use data_labels (empty when missing)
            std::string strLabel;
            if (m_xAxisLabelFormatter) {
                strLabel = m_xAxisLabelFormatter(idx, static_cast<int32_t>(i));
            }
            else if (i < m_dataLabels.size()) {
                strLabel = m_dataLabels[i];
            }
            if (strLabel.empty()) {
                labelRects[i] = UiRect(); // No text, skip
                continue;
            }
            float fx = layout.XToPixel(idx);
            int32_t nTextWidth = GetSafeTextWidth(pRender, strLabel);
            if (nTextWidth <= 0) {
                nTextWidth = Dpi().GetScaleInt(20);
            }
            // Keep the full text width and only shift/clamp it (to avoid the edge labels being squeezed and distorted)
            UiRect rc;
            rc.left = std::max(static_cast<int32_t>(fx) - nTextWidth / 2, rcChart.left);
            rc.left = std::min(rc.left, rcChart.right - nTextWidth);
            rc.right = rc.left + nTextWidth;
            rc.top = nLabelTop;
            rc.bottom = nLabelBottom;
            labelRects[i] = rc;
        }
        // Thinning: greedily select labels that do not overlap each other (including the minimum spacing), but always paint the last label (when it overlaps the last selected label, replace that one with the last label)
        std::vector<size_t> drawn;
        const int32_t nMinGap = Dpi().GetScaleInt(6); // Minimum spacing between adjacent labels
        int32_t nLastRight = std::numeric_limits<int32_t>::min();
        for (size_t i = 0; i < nLabelCount; ++i) {
            // Skip the labels outside the visible window (empty rectangles)
            if ((labelRects[i].Width() <= 0) || (labelRects[i].Height() <= 0)) {
                continue;
            }
            if (labelRects[i].left > nLastRight + nMinGap) {
                drawn.push_back(i);
                nLastRight = labelRects[i].right;
            }
        }
        if (!drawn.empty() && (drawn.back() != nLabelCount - 1)) {
            // The last label was not painted: if it is visible and does not overlap an earlier label after replacing the last selected one, replace it
            UiRect rcLast = labelRects[nLabelCount - 1];
            if ((rcLast.Width() > 0) && (rcLast.Height() > 0)) {
                bool bCanReplace = (drawn.size() == 1);
                if (!bCanReplace) {
                    UiRect rcPrev = labelRects[drawn[drawn.size() - 2]];
                    bCanReplace = (rcLast.left > rcPrev.right);
                }
                if (bCanReplace) {
                    drawn.back() = nLabelCount - 1;
                }
            }
        }
        for (size_t i : drawn) {
            // Label text: prefer the X axis format callback, otherwise use data_labels
            std::string strLabel;
            if (m_xAxisLabelFormatter) {
                strLabel = m_xAxisLabelFormatter(static_cast<double>(i), static_cast<int32_t>(i));
            }
            else if (i < m_dataLabels.size()) {
                strLabel = m_dataLabels[i];
            }
            if (strLabel.empty()) {
                continue;
            }
            DrawChartText(pRender, strLabel, labelRects[i], TEXT_HCENTER | TEXT_TOP | TEXT_SINGLELINE | TEXT_END_ELLIPSIS);
        }
    }

    // Multi-series legend
    PaintLegend(pRender, rcChart);
}

void Chart::PaintBar(IRender* pRender, const UiRect& rcChart)
{
    UiRect rcPlot = CalcPlotRect(pRender, rcChart);
    PaintTitles(pRender, rcChart, rcPlot);
    if ((rcPlot.Width() <= 0) || (rcPlot.Height() <= 0)) {
        return;
    }

    // Data update animation progress (bar growth)
    const double dProgress = GetAnimProgress();
    const float fRadius = static_cast<float>(Dpi().GetScaleInt(m_nBarRadius));
    const uint8_t nDarkerScale = 210; // The dark end of the gradient is about 82% brightness (anything darker looks muddy)

    UiColor axisColor = GetUiColor(m_strAxisColor);
    if (axisColor.IsEmpty()) {
        axisColor = UiColor(0xFFD0D0D0);
    }

    // Compute the data range (bar charts support negative values)
    double dMin = 0.0;
    double dMax = 0.0;
    CalcValueRange(dMin, dMax);
    // In stacked mode the stacked positive/negative maximum must be taken into account
    if (m_barMode == ChartBarMode::kStacked) {
        dMin = 0.0;
        dMax = 0.0;
        size_t nCount = 0;
        for (const Series& s : m_series) {
            nCount = std::max(nCount, s.data.size());
        }
        for (size_t i = 0; i < nCount; ++i) {
            double posSum = 0.0;
            double negSum = 0.0;
            for (const Series& s : m_series) {
                if (i < s.data.size()) {
                    double v = s.data[i];
                    if (v >= 0.0) {
                        posSum += v;
                    }
                    else {
                        negSum += v;
                    }
                }
            }
            dMax = std::max(dMax, posSum);
            dMin = std::min(dMin, negSum);
        }
    }
    // Make sure the range is valid, and leave a margin + nice ticks
    if (dMax <= dMin) {
        dMax = dMin + 1.0;
    }
    double dRange = dMax - dMin;
    dMax = dMax + dRange * 0.05;
    dMin = dMin - dRange * 0.05;
    int32_t nDiv = m_nAxisDivisions;
    CalcNiceAxis(dMin, dMax, nDiv);
    dRange = dMax - dMin;

    const int32_t nLeft = rcPlot.left;
    const int32_t nRight = rcPlot.right;
    const int32_t nTop = rcPlot.top;
    const int32_t nBottom = rcPlot.bottom;
    const float fPlotWidth = static_cast<float>(nRight - nLeft);
    const float fPlotHeight = static_cast<float>(nBottom - nTop);

    // Y coordinate of the zero baseline
    double zeroNorm = (0.0 - dMin) / dRange;
    float fZeroY = static_cast<float>(nBottom) - fPlotHeight * static_cast<float>(zeroNorm);
    fZeroY = std::max(static_cast<float>(nTop), std::min(fZeroY, static_cast<float>(nBottom)));

    // Width of the Y axis tick value texts (measure the tick values that are actually painted, so that the width matches exactly)
    int32_t nAxisValueWidth = 0;
    if (m_bShowAxisValues) {
        for (int32_t i = 0; i <= nDiv; ++i) {
            double val = dMax - dRange * (static_cast<double>(i) / static_cast<double>(nDiv));
            nAxisValueWidth = std::max(nAxisValueWidth, GetSafeTextWidth(pRender, FormatAxisValue(val, i)));
        }
    }

    // Y axis ticks + grid
    if (m_bShowAxisValues || m_bShowGrid) {
        for (int32_t i = 0; i <= nDiv; ++i) {
            float fy = static_cast<float>(nTop) + fPlotHeight * static_cast<float>(i) / static_cast<float>(nDiv);
            if (m_bShowGrid && (i > 0) && (i < nDiv)) {
                pRender->DrawLine(UiPointF(static_cast<float>(nLeft), fy),
                                  UiPointF(static_cast<float>(nRight), fy),
                                  axisColor, 1.0f);
            }
            if (m_bShowAxisValues) {
                double val = dMax - dRange * (static_cast<double>(i) / static_cast<double>(nDiv));
                std::string strVal = FormatAxisValue(val, i);
                UiRect rcText;
                rcText.right = nLeft - Dpi().GetScaleInt(4);
                rcText.left = rcText.right - nAxisValueWidth - Dpi().GetScaleInt(4);
                rcText.top = static_cast<int32_t>(fy) - Dpi().GetScaleInt(8);
                rcText.bottom = rcText.top + Dpi().GetScaleInt(16);
                DrawChartText(pRender, strVal, rcText, TEXT_RIGHT | TEXT_VCENTER | TEXT_SINGLELINE);
            }
        }
    }

    // Zero baseline (when there are negative values)
    if (dMin < 0.0) {
        pRender->DrawLine(UiPointF(static_cast<float>(nLeft), fZeroY),
                          UiPointF(static_cast<float>(nRight), fZeroY),
                          axisColor, 1.0f);
    }

    // Get the category count
    size_t nCategoryCount = 0;
    for (const Series& s : m_series) {
        nCategoryCount = std::max(nCategoryCount, s.data.size());
    }
    if (nCategoryCount == 0) {
        return;
    }

    const size_t nSeriesCount = m_series.size();
    const float fSlotWidth = fPlotWidth / static_cast<float>(nCategoryCount);

    if (m_barMode == ChartBarMode::kStacked) {
        // Stacked mode: one bar per category, accumulated per series
        const float fBarWidth = fSlotWidth * 0.6f;
        for (size_t i = 0; i < nCategoryCount; ++i) {
            float fCenterX = static_cast<float>(nLeft) + fSlotWidth * (static_cast<float>(i) + 0.5f);
            float fBarLeft = fCenterX - fBarWidth / 2.0f;
            // Positive stacking (upwards from the zero baseline)
            float posTop = fZeroY;
            for (size_t si = 0; si < nSeriesCount; ++si) {
                if (i >= m_series[si].data.size()) {
                    continue;
                }
                double v = m_series[si].data[i];
                if (v >= 0.0) {
                    float h = fPlotHeight * static_cast<float>(v / dRange) * static_cast<float>(dProgress);
                    float segTop = posTop - h;
                    UiRectF rcBar(fBarLeft, segTop, fBarLeft + fBarWidth, posTop);
                    UiColor barColor = GetSeriesColor(si);
                    // Stacked mode: the segments are not rounded (rounded segments would create visible gaps)
                    if (m_bBarGradient) {
                        pRender->FillRoundRect(rcBar, 0.0f, 0.0f, barColor,
                                               MakeDarkerColor(barColor, nDarkerScale), 2, GetAlpha());
                    }
                    else {
                        pRender->FillRect(rcBar, barColor, GetAlpha());
                    }
                    posTop = segTop;
                }
            }
            // Negative stacking (downwards from the zero baseline)
            float negBottom = fZeroY;
            for (size_t si = 0; si < nSeriesCount; ++si) {
                if (i >= m_series[si].data.size()) {
                    continue;
                }
                double v = m_series[si].data[i];
                if (v < 0.0) {
                    float h = fPlotHeight * static_cast<float>((-v) / dRange) * static_cast<float>(dProgress);
                    float segBottom = negBottom + h;
                    UiRectF rcBar(fBarLeft, negBottom, fBarLeft + fBarWidth, segBottom);
                    UiColor barColor = GetSeriesColor(si);
                    if (m_bBarGradient) {
                        pRender->FillRoundRect(rcBar, 0.0f, 0.0f, barColor,
                                               MakeDarkerColor(barColor, nDarkerScale), 2, GetAlpha());
                    }
                    else {
                        pRender->FillRect(rcBar, barColor, GetAlpha());
                    }
                    negBottom = segBottom;
                }
            }
            // Value label (shows the stacked total)
            if (m_bShowValue) {
                double total = 0.0;
                for (size_t si = 0; si < nSeriesCount; ++si) {
                    if (i < m_series[si].data.size()) {
                        total += m_series[si].data[i];
                    }
                }
                std::string strValue = FormatValue(total);
                UiRect rcText;
                rcText.left = static_cast<int32_t>(fCenterX) - Dpi().GetScaleInt(40);
                rcText.right = static_cast<int32_t>(fCenterX) + Dpi().GetScaleInt(40);
                rcText.top = static_cast<int32_t>(posTop) - Dpi().GetScaleInt(18);
                rcText.bottom = static_cast<int32_t>(posTop);
                DrawChartText(pRender, strValue, rcText, TEXT_HCENTER | TEXT_BOTTOM | TEXT_SINGLELINE);
            }
        }
    }
    else {
        // Grouped mode: multiple bars side by side inside each category
        const float fGroupWidth = fSlotWidth * 0.8f;
        const float fBarWidth = fGroupWidth / static_cast<float>(nSeriesCount);
        for (size_t si = 0; si < nSeriesCount; ++si) {
            for (size_t i = 0; i < m_series[si].data.size(); ++i) {
                double v = m_series[si].data[i];
                float fCenterX = static_cast<float>(nLeft) + fSlotWidth * (static_cast<float>(i) + 0.5f);
                float fGroupLeft = fCenterX - fGroupWidth / 2.0f;
                float fBarLeft = fGroupLeft + fBarWidth * static_cast<float>(si);
                float fBarTop, fBarBottom;
                if (v >= 0.0) {
                    float h = fPlotHeight * static_cast<float>(v / dRange) * static_cast<float>(dProgress);
                    fBarTop = fZeroY - h;
                    fBarBottom = fZeroY;
                }
                else {
                    float h = fPlotHeight * static_cast<float>((-v) / dRange) * static_cast<float>(dProgress);
                    fBarTop = fZeroY;
                    fBarBottom = fZeroY + h;
                }
                UiRectF rcBar(fBarLeft, fBarTop, fBarLeft + fBarWidth, fBarBottom);
                UiColor barColor = GetSeriesColor(si);
                if (m_bBarGradient) {
                    // Bright at the top -> dark at the bottom (direction 2 = top to bottom)
                    pRender->FillRoundRect(rcBar, fRadius, fRadius, barColor,
                                           MakeDarkerColor(barColor, nDarkerScale), 2, GetAlpha());
                }
                else {
                    pRender->FillRoundRect(rcBar, fRadius, fRadius, barColor, GetAlpha());
                }
                // Highlight the selected bar with a stroke
                if (m_bEnableSelect && (m_selected.type == HitType::kBar) &&
                    (m_selected.seriesIndex == si) && (m_selected.dataIndex == i)) {
                    UiColor hlColor = GetUiColor("text_default");
                    if (hlColor.IsEmpty()) {
                        hlColor = UiColor(0xFF000000);
                    }
                    pRender->DrawRoundRect(rcBar, fRadius, fRadius, hlColor, 2.0f);
                }

                // Value label
                if (m_bShowValue) {
                    std::string strValue = FormatValue(v);
                    UiRect rcText;
                    rcText.left = static_cast<int32_t>(fBarLeft) - Dpi().GetScaleInt(20);
                    rcText.right = static_cast<int32_t>(fBarLeft + fBarWidth) + Dpi().GetScaleInt(20);
                    if (v >= 0.0) {
                        rcText.top = static_cast<int32_t>(fBarTop) - Dpi().GetScaleInt(18);
                        rcText.bottom = static_cast<int32_t>(fBarTop);
                    }
                    else {
                        rcText.top = static_cast<int32_t>(fBarBottom);
                        rcText.bottom = static_cast<int32_t>(fBarBottom) + Dpi().GetScaleInt(18);
                    }
                    DrawChartText(pRender, strValue, rcText, TEXT_HCENTER | TEXT_VCENTER | TEXT_SINGLELINE);
                }
            }
        }
    }

    // X axis category labels (the X axis format callback can override the default data_labels)
    if ((!m_dataLabels.empty()) || m_xAxisLabelFormatter) {
        for (size_t i = 0; (i < m_dataLabels.size()) && (i < nCategoryCount); ++i) {
            float fCenterX = static_cast<float>(nLeft) + fSlotWidth * (static_cast<float>(i) + 0.5f);
            UiRect rcLabel;
            rcLabel.left = static_cast<int32_t>(fCenterX) - static_cast<int32_t>(fSlotWidth / 2.0f);
            rcLabel.right = static_cast<int32_t>(fCenterX) + static_cast<int32_t>(fSlotWidth / 2.0f);
            rcLabel.top = nBottom + Dpi().GetScaleInt(2);
            rcLabel.bottom = nBottom + Dpi().GetScaleInt(18);
            std::string strLabel;
            if (m_xAxisLabelFormatter) {
                strLabel = m_xAxisLabelFormatter(static_cast<double>(i), static_cast<int32_t>(i));
            }
            else {
                strLabel = m_dataLabels[i];
            }
            DrawChartText(pRender, strLabel, rcLabel, TEXT_HCENTER | TEXT_TOP | TEXT_SINGLELINE | TEXT_END_ELLIPSIS);
        }
    }

    // Multi-series legend
    PaintLegend(pRender, rcChart);
}

void Chart::PaintPie(IRender* pRender, const UiRect& rcChart)
{
    UiRect rcPlot = CalcPlotRect(pRender, rcChart);
    PaintTitles(pRender, rcChart, rcPlot);
    if ((rcPlot.Width() <= 0) || (rcPlot.Height() <= 0)) {
        return;
    }

    // The pie chart uses only the first series
    if (m_series.empty() || m_series[0].data.empty()) {
        return;
    }
    const std::vector<double>& data = m_series[0].data;
    const size_t nCount = data.size();

    UiColor seriesColor = GetSeriesColor(0);

    // Compute the total (ignoring negative values)
    double dSum = 0.0;
    for (double v : data) {
        if (v > 0.0) {
            dSum += v;
        }
    }
    if (dSum <= 0.0) {
        return;
    }

    // Build the legend texts in advance (shared by measuring and painting, to keep the two code paths consistent)
    std::vector<std::string> legendTexts;
    legendTexts.reserve(nCount);
    for (size_t i = 0; i < nCount; ++i) {
        std::string strLabel = (i < m_dataLabels.size()) ? m_dataLabels[i] : std::string();
        if (strLabel.empty()) {
            strLabel = FormatValue(data[i]);
        }
        else {
            strLabel = strLabel + "  " + FormatValue(data[i]);
            if (m_bShowPercent) {
                double percent = data[i] / dSum * 100.0;
                strLabel = strLabel + StringUtil::Printf(" (%.1f%%)", percent);
            }
        }
        legendTexts.push_back(strLabel);
    }

    // Pie drawing area: reserve the safe width of the legend text on the right (capped at 60% of the plot area width), so that the text is not truncated
    int32_t nLegendWidth = 0;
    if (m_bLegendVisible) {
        const int32_t nColorBoxSize = Dpi().GetScaleInt(12);
        int32_t nMaxTextWidth = 0;
        for (const std::string& strLabel : legendTexts) {
            nMaxTextWidth = std::max(nMaxTextWidth, GetSafeTextWidth(pRender, strLabel));
        }
        nLegendWidth = nColorBoxSize + Dpi().GetScaleInt(6) + nMaxTextWidth + Dpi().GetScaleInt(10);
        const int32_t nMaxLegendWidth = std::max(rcPlot.Width() * 60 / 100, Dpi().GetScaleInt(80));
        nLegendWidth = std::min(nLegendWidth, nMaxLegendWidth);
    }
    UiRect rcPieArea = rcPlot;
    rcPieArea.right -= nLegendWidth;
    if ((rcPieArea.Width() <= 0) || (rcPieArea.Height() <= 0)) {
        return;
    }

    // Inscribed square + margin
    const int32_t nSide = std::min(rcPieArea.Width(), rcPieArea.Height());
    const int32_t nCenterX = rcPieArea.left + rcPieArea.Width() / 2;
    const int32_t nCenterY = rcPieArea.top + rcPieArea.Height() / 2;

    IRenderFactory* pFactory = GlobalManager::Instance().GetRenderFactory();
    // The selected sector offset is fixed at 8% of the circumscribed radius; the effective drawing radius shrinks accordingly, so that the arc stays inside the pie area after the offset
    const float fOuterRadius = static_cast<float>(nSide) / 2.0f;
    const float fSelOffset = fOuterRadius * 0.08f;
    const float fRadius = fOuterRadius - fSelOffset;
    const float fInnerRadius = m_bDonut ? fRadius * 0.55f : 0.0f; // Inner radius of the donut

    // Paint the sectors (starting from 270 degrees, i.e. the top, clockwise)
    float fStartAngle = 270.0f;
    for (size_t i = 0; i < nCount; ++i) {
        double v = data[i];
        if (v <= 0.0) {
            continue;
        }
        float fSweep = static_cast<float>(360.0 * (v / dSum));
        UiColor sliceColor = MakeSliceColor(seriesColor, i, nCount);

        // Selected sector: drawn offset away from the center, forming a detached highlight
        int32_t nOffsetX = 0;
        int32_t nOffsetY = 0;
        if (m_bEnableSelect && (m_selected.type == HitType::kSlice) && (m_selected.dataIndex == i)) {
            float fMidAngle = fStartAngle + fSweep / 2.0f;
            double rad = fMidAngle * kPi / 180.0;
            nOffsetX = static_cast<int32_t>(fSelOffset * static_cast<float>(std::cos(rad)));
            nOffsetY = static_cast<int32_t>(fSelOffset * static_cast<float>(std::sin(rad)));
        }
        const int32_t nCenterXOff = nCenterX + nOffsetX;
        const int32_t nCenterYOff = nCenterY + nOffsetY;

        if (pFactory != nullptr) {
            // Bounding rectangle of the effective drawing radius (the center may carry the selection offset)
            UiRectF rcOuter(static_cast<float>(nCenterXOff) - fRadius, static_cast<float>(nCenterYOff) - fRadius,
                            static_cast<float>(nCenterXOff) + fRadius, static_cast<float>(nCenterYOff) + fRadius);
            IPath* pPath = pFactory->CreatePath();
            std::unique_ptr<IPath> spPath(pPath);
            if (pPath != nullptr) {
                if (m_bDonut) {
                    // Donut: outer arc + inner arc, closed with two arcs (the outer arc clockwise, the inner arc counter-clockwise)
                    pPath->AddArc(rcOuter, fStartAngle, fSweep);
                    // The inner arc goes the opposite way
                    UiRectF rcInner(static_cast<float>(nCenterXOff) - fInnerRadius,
                                    static_cast<float>(nCenterYOff) - fInnerRadius,
                                    static_cast<float>(nCenterXOff) + fInnerRadius,
                                    static_cast<float>(nCenterYOff) + fInnerRadius);
                    pPath->AddArc(rcInner, fStartAngle + fSweep, -fSweep);
                    pPath->Close();
                }
                else {
                    // Full sector: center -> arc start -> arc -> close
                    double startRad = fStartAngle * kPi / 180.0;
                    float startX = static_cast<float>(nCenterXOff) + fRadius * static_cast<float>(std::cos(startRad));
                    float startY = static_cast<float>(nCenterYOff) + fRadius * static_cast<float>(std::sin(startRad));
                    pPath->AddLine(static_cast<float>(nCenterXOff), static_cast<float>(nCenterYOff), startX, startY);
                    pPath->AddArc(rcOuter, fStartAngle, fSweep);
                    pPath->Close();
                }

                IBrush* pBrush = pFactory->CreateBrush(sliceColor);
                std::unique_ptr<IBrush> spBrush(pBrush);
                if (pBrush != nullptr) {
                    pRender->FillPath(pPath, pBrush);
                }
            }
        }
        fStartAngle += fSweep;
    }

    // Data labels (outside the sectors, along the center direction)
    if (m_bShowValue || m_bShowPercent) {
        fStartAngle = 270.0f;
        for (size_t i = 0; i < nCount; ++i) {
            double v = data[i];
            if (v <= 0.0) {
                continue;
            }
            float fSweep = static_cast<float>(360.0 * (v / dSum));
            float fMidAngle = fStartAngle + fSweep / 2.0f;
            double rad = fMidAngle * kPi / 180.0;

            // Label text: value + optional percentage
            std::string strValue;
            if (m_bShowPercent) {
                double percent = v / dSum * 100.0;
                strValue = StringUtil::Printf("%.1f%%", percent);
            }
            else {
                strValue = FormatValue(v);
            }

            // Selected sector: the label moves outside the sector and a leader line connects it (so the percentage of a small sector is no longer squeezed inside)
            const bool bSelectedSlice = m_bEnableSelect && (m_selected.type == HitType::kSlice) && (m_selected.dataIndex == i);
            if (bSelectedSlice) {
                const float fCos = static_cast<float>(std::cos(rad));
                const float fSin = static_cast<float>(std::sin(rad));
                const float fStartR = fOuterRadius * 1.05f;
                const float fEndR = fOuterRadius + Dpi().GetScaleInt(12);
                UiColor lineColor = GetUiColor(m_strLabelColor);
                if (lineColor.IsEmpty()) {
                    lineColor = UiColor(0xFF000000);
                }
                // Leader line: extends from the sector edge to the label
                pRender->DrawLine(UiPointF(static_cast<float>(nCenterX) + fStartR * fCos, static_cast<float>(nCenterY) + fStartR * fSin),
                                  UiPointF(static_cast<float>(nCenterX) + fEndR * fCos, static_cast<float>(nCenterY) + fEndR * fSin),
                                  lineColor, 1.0f);
                // Outside label: placed outwards along the radius, aligned according to the angle
                float fLabelX = static_cast<float>(nCenterX) + (fEndR + Dpi().GetScaleInt(6)) * fCos;
                float fLabelY = static_cast<float>(nCenterY) + (fEndR + Dpi().GetScaleInt(6)) * fSin;
                UiRect rcText;
                rcText.left = static_cast<int32_t>(fLabelX) - Dpi().GetScaleInt(40);
                rcText.right = static_cast<int32_t>(fLabelX) + Dpi().GetScaleInt(40);
                rcText.top = static_cast<int32_t>(fLabelY) - Dpi().GetScaleInt(10);
                rcText.bottom = static_cast<int32_t>(fLabelY) + Dpi().GetScaleInt(10);
                DrawChartText(pRender, strValue, rcText, TEXT_HCENTER | TEXT_VCENTER | TEXT_SINGLELINE);
            }
            else {
                float fLabelRadius = fOuterRadius * (m_bDonut ? 0.78f : 0.7f);
                float fLabelX = static_cast<float>(nCenterX) + fLabelRadius * static_cast<float>(std::cos(rad));
                float fLabelY = static_cast<float>(nCenterY) + fLabelRadius * static_cast<float>(std::sin(rad));
                UiRect rcText;
                rcText.left = static_cast<int32_t>(fLabelX) - Dpi().GetScaleInt(30);
                rcText.right = static_cast<int32_t>(fLabelX) + Dpi().GetScaleInt(30);
                rcText.top = static_cast<int32_t>(fLabelY) - Dpi().GetScaleInt(10);
                rcText.bottom = static_cast<int32_t>(fLabelY) + Dpi().GetScaleInt(10);
                DrawChartText(pRender, strValue, rcText, TEXT_HCENTER | TEXT_VCENTER | TEXT_SINGLELINE);
            }
            fStartAngle += fSweep;
        }
    }

    // Legend
    if (m_bLegendVisible) {
        const int32_t nLegendLeft = rcPieArea.right + Dpi().GetScaleInt(12);
        int32_t nLegendTop = rcPlot.top + Dpi().GetScaleInt(8);
        const int32_t nColorBoxSize = Dpi().GetScaleInt(12);
        for (size_t i = 0; i < nCount; ++i) {
            UiColor sliceColor = MakeSliceColor(seriesColor, i, nCount);
            UiRectF rcColor;
            rcColor.left = static_cast<float>(nLegendLeft);
            rcColor.top = static_cast<float>(nLegendTop) + nColorBoxSize / 2.0f;
            rcColor.right = rcColor.left + nColorBoxSize;
            rcColor.bottom = rcColor.top + nColorBoxSize;
            pRender->FillRoundRect(rcColor, 2.0f, 2.0f, sliceColor, GetAlpha());

            UiRect rcText;
            rcText.left = nLegendLeft + nColorBoxSize + Dpi().GetScaleInt(6);
            rcText.right = rcPieArea.right + nLegendWidth;
            rcText.top = nLegendTop;
            rcText.bottom = nLegendTop + Dpi().GetScaleInt(20);
            DrawChartText(pRender, legendTexts[i], rcText, TEXT_LEFT | TEXT_VCENTER | TEXT_SINGLELINE | TEXT_END_ELLIPSIS);

            nLegendTop += Dpi().GetScaleInt(22);
        }
    }
}

void Chart::PaintLegend(IRender* pRender, const UiRect& rcChart)
{
    // Line/bar charts: shown when there are multiple series and the legend is enabled
    if ((m_chartType == ChartType::kPie) || !m_bLegendVisible || (m_series.size() <= 1)) {
        return;
    }
    const int32_t nLegendLeft = rcChart.right - Dpi().GetScaleInt(108);
    int32_t nLegendTop = rcChart.top + Dpi().GetScaleInt(8);
    const int32_t nColorBoxSize = Dpi().GetScaleInt(12);
    for (size_t si = 0; si < m_series.size(); ++si) {
        UiColor seriesColor = GetSeriesColor(si);
        UiRectF rcColor;
        rcColor.left = static_cast<float>(nLegendLeft);
        rcColor.top = static_cast<float>(nLegendTop) + nColorBoxSize / 2.0f;
        rcColor.right = rcColor.left + nColorBoxSize;
        rcColor.bottom = rcColor.top + nColorBoxSize;
        pRender->FillRoundRect(rcColor, 2.0f, 2.0f, seriesColor, GetAlpha());

        std::string strLabel = m_series[si].name;
        if (strLabel.empty()) {
            strLabel = StringUtil::Printf("Series %zu", si + 1);
        }
        UiRect rcText;
        rcText.left = nLegendLeft + nColorBoxSize + Dpi().GetScaleInt(6);
        rcText.right = rcChart.right;
        rcText.top = nLegendTop;
        rcText.bottom = nLegendTop + Dpi().GetScaleInt(20);
        DrawChartText(pRender, strLabel, rcText, TEXT_LEFT | TEXT_VCENTER | TEXT_SINGLELINE | TEXT_END_ELLIPSIS);

        nLegendTop += Dpi().GetScaleInt(22);
    }
}

bool Chart::MouseMove(const EventArgs& msg)
{
    if (!IsEnabled()) {
        return BaseClass::MouseMove(msg);
    }
    IRender* pRender = (GetWindow() != nullptr) ? GetWindow()->GetRender() : nullptr;
    if (pRender == nullptr) {
        return BaseClass::MouseMove(msg);
    }
    // Line chart drag-to-pan
    if (m_bPanning && (m_chartType == ChartType::kLine)) {
        UiRect rc = GetRect();
        UiPadding rcPadding = GetControlPadding();
        rc.Deflate(rcPadding);
        LineLayout layout = CalcLineLayout(pRender, rc);
        if ((layout.rcPlot.Width() > 0) && (layout.nMaxCount > 1)) {
            int32_t dx = msg.ptMouse.x - m_panStartX;
            if (dx != 0) {
                m_bPanMoved = true;
                double dMaxIndex = static_cast<double>(layout.nMaxCount - 1);
                double dSpan = layout.viewEnd - layout.viewStart;
                double dIndexDelta = (dSpan / static_cast<double>(layout.rcPlot.Width())) * static_cast<double>(dx);
                double dNewStart = m_panStartView - dIndexDelta;
                double dNewEnd = dNewStart + dSpan;
                if (dNewStart < 0.0) {
                    dNewEnd -= dNewStart;
                    dNewStart = 0.0;
                }
                if (dNewEnd > dMaxIndex) {
                    dNewStart -= (dNewEnd - dMaxIndex);
                    dNewEnd = dMaxIndex;
                    if (dNewStart < 0.0) {
                        dNewStart = 0.0;
                    }
                }
                m_viewStart = dNewStart;
                m_viewEnd = dNewEnd;
                Invalidate();
            }
        }
        return true;
    }
    if (!m_bEnableTooltip) {
        return BaseClass::MouseMove(msg);
    }
    // msg.ptMouse is in control coordinates
    HitResult hit = HitTest(pRender, msg.ptMouse);
    if (hit.type == HitType::kNone) {
        if (m_lastHover.type != HitType::kNone) {
            m_lastHover = HitResult();
            // Clear the tooltip
            if (GetWindow() != nullptr) {
                SetToolTipText("");
            }
        }
        return BaseClass::MouseMove(msg);
    }
    // Update only when the hit result changed (to avoid resetting the tooltip at a high rate)
    bool bChanged = (hit.type != m_lastHover.type) ||
                    (hit.seriesIndex != m_lastHover.seriesIndex) ||
                    (hit.dataIndex != m_lastHover.dataIndex);
    m_lastHover = hit;
    if (bChanged) {
        SetToolTipText(BuildHitTooltip(hit));
    }
    return true; // Consume the message
}

bool Chart::MouseLeave(const EventArgs& msg)
{
    m_lastHover = HitResult();
    if (m_bPanning) {
        m_bPanning = false;
        m_bPanMoved = false;
    }
    return BaseClass::MouseLeave(msg);
}

bool Chart::ButtonDown(const EventArgs& msg)
{
    if (!IsEnabled()) {
        return BaseClass::ButtonDown(msg);
    }
    IRender* pRender = (GetWindow() != nullptr) ? GetWindow()->GetRender() : nullptr;
    if (pRender == nullptr) {
        return BaseClass::ButtonDown(msg);
    }
    HitResult hit = HitTest(pRender, msg.ptMouse);

    // Line chart: pressing the left button on an empty area starts panning; pressing on a data point takes the selection path
    if (m_chartType == ChartType::kLine) {
        UiRect rc = GetRect();
        UiPadding rcPadding = GetControlPadding();
        rc.Deflate(rcPadding);
        LineLayout layout = CalcLineLayout(pRender, rc);
        if ((hit.type == HitType::kNone) &&
            (layout.rcPlot.Width() > 0) && (layout.rcPlot.Height() > 0) &&
            (msg.ptMouse.x >= layout.rcPlot.left) && (msg.ptMouse.x <= layout.rcPlot.right) &&
            (msg.ptMouse.y >= layout.rcPlot.top) && (msg.ptMouse.y <= layout.rcPlot.bottom)) {
            m_bPanning = true;
            m_bPanMoved = false;
            m_panStartX = msg.ptMouse.x;
            m_panStartView = layout.viewStart;
            if (GetWindow() != nullptr) {
                GetWindow()->SetCapture();
            }
            return true;
        }
    }

    // Click to select
    if (m_bEnableSelect && (hit.type != HitType::kNone)) {
        bool bSame = (hit.type == m_selected.type) &&
                     (hit.seriesIndex == m_selected.seriesIndex) &&
                     (hit.dataIndex == m_selected.dataIndex);
        if (bSame) {
            m_selected = HitResult(); // Clicking again cancels the selection
        }
        else {
            m_selected = hit;
        }
        Invalidate();
        SendPointClickEvent(hit);
        return true;
    }
    return BaseClass::ButtonDown(msg);
}

bool Chart::ButtonUp(const EventArgs& msg)
{
    if (m_bPanning) {
        m_bPanning = false;
        m_bPanMoved = false;
        Invalidate(); // Restore the value label display
        if (GetWindow() != nullptr) {
            GetWindow()->ReleaseCapture();
        }
        return true;
    }
    return BaseClass::ButtonUp(msg);
}

bool Chart::MouseWheel(const EventArgs& msg)
{
    if (!IsEnabled() || (m_chartType != ChartType::kLine)) {
        return BaseClass::MouseWheel(msg);
    }
    // Line chart mouse-wheel zoom (around the mouse X position)
    IRender* pRender = (GetWindow() != nullptr) ? GetWindow()->GetRender() : nullptr;
    if (pRender == nullptr) {
        return BaseClass::MouseWheel(msg);
    }
    UiRect rc = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rc.Deflate(rcPadding);
    LineLayout layout = CalcLineLayout(pRender, rc);
    if ((layout.rcPlot.Width() <= 0) || (layout.nMaxCount <= 1)) {
        return BaseClass::MouseWheel(msg);
    }
    int32_t wheelDelta = msg.eventData; // Positive = up (zoom in), negative = down (zoom out)
    const double dMaxIndex = static_cast<double>(layout.nMaxCount - 1);
    double dSpan = layout.viewEnd - layout.viewStart;
    // Zoom factor: one wheel step zooms by 20%
    double dFactor = (wheelDelta > 0) ? 0.8 : 1.25;
    double dNewSpan = dSpan * dFactor;
    double dMinSpan = std::max(2.0, dMaxIndex * 0.02);
    double dMaxSpan = dMaxIndex;
    dNewSpan = std::max(dMinSpan, std::min(dNewSpan, dMaxSpan));
    // Zoom around the mouse position
    double dAnchor = layout.PixelToX(msg.ptMouse.x);
    double dAnchorRatio = (dSpan > 0.0) ? ((dAnchor - layout.viewStart) / dSpan) : 0.5;
    double dNewStart = dAnchor - dNewSpan * dAnchorRatio;
    double dNewEnd = dNewStart + dNewSpan;
    if (dNewStart < 0.0) {
        dNewEnd -= dNewStart;
        dNewStart = 0.0;
    }
    if (dNewEnd > dMaxIndex) {
        dNewStart -= (dNewEnd - dMaxIndex);
        dNewEnd = dMaxIndex;
        if (dNewStart < 0.0) {
            dNewStart = 0.0;
        }
    }
    m_viewStart = dNewStart;
    m_viewEnd = dNewEnd;
    Invalidate();
    return true;
}

} // namespace ui
