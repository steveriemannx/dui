#include "dui/Animation/EasingFunctions.h"
#include <map>
#include <cmath>

////////////////////////////////////////////////////////////////
//Reference for easing effects: https://easings.net/

namespace ui
{
// Define the PI constant (if not defined by the system)
#ifndef PI
#define PI 3.14159265358979323846
#endif

/**
 * @brief Linear easing function (floating-point version, normalized output)
 * @param t Normalized time (0=start, 1=end); the function clamps it to 0~1 internally
 * @return double The eased value (0~1, identical to the input t)
 */
static double easeLinear(double t)
{
    // Boundary protection: ensure t is within 0~1 to avoid out-of-range values
    t = std::fmax(0.0, std::fmin(1.0, t));
    return t; // Core of linear easing: output = input
}

/**
* @brief Sine ease-in: starts slow, gradually accelerates
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInSine(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return 1.0 - cos(t * PI / 2.0); // Standard formula: 1 - cos(πt/2)
}

/**
* @brief Sine ease-out: starts fast, gradually decelerates
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeOutSine(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return sin(t * PI / 2.0); // Standard formula: sin(πt/2), the original t is not modified
}

/**
* @brief Sine ease-in-out: slow at start, fast in the middle, slow at the end
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInOutSine(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return -0.5 * (cos(PI * t) - 1.0); // Standard formula: -0.5*(cos(πt)-1)
}

/**
* @brief Quadratic ease-in (t²)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInQuad(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return t * t;
}

/**
* @brief Quadratic ease-out (t*(2-t))
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeOutQuad(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return t * (2.0 - t);
}

/**
* @brief Quadratic ease-in-out
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInOutQuad(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    if (t < 0.5) {
        return 2.0 * t * t;
    }
    else {
        t -= 0.5;
        return 1.0 - 2.0 * t * t; // Standard formula, to avoid exceeding the 0~1 range
    }
}

/**
* @brief Cubic ease-in (t³)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInCubic(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return t * t * t;
}

/**
* @brief Cubic ease-out (1+(t-1)³)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeOutCubic(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    double temp = t - 1.0; // Temporary variable, the original t is not modified
    return 1.0 + temp * temp * temp;
}

/**
* @brief Cubic ease-in-out
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInOutCubic(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    if (t < 0.5) {
        return 4.0 * t * t * t; // First half: 4t³
    }
    else {
        double temp = 2.0 * t - 2.0; // Temporary variable, the original t is not modified
        return 1.0 + 0.5 * temp * temp * temp; // Second half: 1 - 4(1-t)³
    }
}

/**
* @brief Quartic ease-in (t⁴)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInQuart(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    t *= t;
    return t * t;
}

/**
* @brief Quartic ease-out (1-(t-1)⁴)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeOutQuart(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    double temp = t - 1.0; // Temporary variable, the original t is not modified
    temp *= temp;
    return 1.0 - temp * temp;
}

/**
* @brief Quartic ease-in-out
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInOutQuart(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    if (t < 0.5) {
        double temp = 2.0 * t;
        temp *= temp;
        return 0.5 * temp * temp;
    }
    else {
        double temp = 2.0 * (t - 1.0);
        temp *= temp;
        return 1.0 - 0.5 * temp * temp;
    }
}

/**
* @brief Quintic ease-in (t⁵)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInQuint(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    double t2 = t * t;
    return t * t2 * t2; // t*t²*t² = t⁵
}

/**
* @brief Quintic ease-out (1+(t-1)⁵)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeOutQuint(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    double temp = t - 1.0; // Temporary variable, the original t is not modified
    double temp2 = temp * temp;
    return 1.0 + temp * temp2 * temp2;
}

/**
* @brief Quintic ease-in-out
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInOutQuint(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    if (t < 0.5) {
        double temp = 2.0 * t;
        double temp2 = temp * temp;
        return 0.5 * temp * temp2 * temp2; // 16t⁵ / 2 = 8t⁵
    }
    else {
        double temp = 2.0 * (t - 1.0);
        double temp2 = temp * temp;
        return 1.0 + 0.5 * temp * temp2 * temp2; // 1 - 8(1-t)⁵
    }
}

/**
* @brief Exponential ease-in ((2^(8t)-1)/255)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInExpo(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return t == 0.0 ? 0.0 : (pow(2.0, 8.0 * t) - 1.0) / 255.0;
}

/**
* @brief Exponential ease-out (1-2^(-8t))
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeOutExpo(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return t == 1.0 ? 1.0 : 1.0 - pow(2.0, -8.0 * t);
}

/**
* @brief Exponential ease-in-out
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInOutExpo(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    if (t == 0.0 || t == 1.0) {
        return t;
    }
    if (t < 0.5) {
        return (pow(2.0, 16.0 * t) - 1.0) / 510.0;
    }
    else {
        return 1.0 - 0.5 * pow(2.0, -16.0 * (t - 0.5));
    }
}

/**
* @brief Circular ease-in (1-sqrt(1-t²))
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInCirc(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return 1.0 - sqrt(1.0 - t * t);
}

/**
* @brief Circular ease-out (sqrt(1-(t-1)²))
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeOutCirc(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    double temp = t - 1.0;
    return sqrt(1.0 - temp * temp);
}

/**
* @brief Circular ease-in-out
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInOutCirc(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    if (t < 0.5) {
        double temp = 2.0 * t;
        return 0.5 * (1.0 - sqrt(1.0 - temp * temp));
    }
    else {
        double temp = 2.0 * (t - 1.0);
        return 0.5 * (sqrt(1.0 - temp * temp) + 1.0);
    }
}

/**
* @brief Back ease-in (moves backward first, then forward)
* @param t Normalized time (0~1)
* @return The transformed easing value (may briefly be <0)
*/
static double easeInBack(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    const double c1 = 1.70158; // Classic back coefficient
    const double c3 = c1 + 1.0;
    return c3 * t * t * t - c1 * t * t;
}

/**
* @brief Back ease-out (overshoots the target, then springs back)
* @param t Normalized time (0~1)
* @return The transformed easing value (may briefly be >1)
*/
static double easeOutBack(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    double temp = t - 1.0;
    return 1.0 + c3 * temp * temp * temp + c1 * temp * temp;
}

/**
* @brief Back ease-in-out
* @param t Normalized time (0~1)
* @return The transformed easing value (may briefly exceed 0~1)
*/
static double easeInOutBack(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    const double c1 = 1.70158;
    const double c2 = c1 * 1.525; // Ease-in-out coefficient
    if (t < 0.5) {
        double temp = 2.0 * t;
        return 0.5 * (temp * temp * ((c2 + 1.0) * temp - c2));
    }
    else {
        double temp = 2.0 * (t - 1.0);
        return 0.5 * (2.0 + temp * temp * ((c2 + 1.0) * temp + c2));
    }
}

/**
* @brief Elastic ease-in (simulates elastic stretching)
* @param t Normalized time (0~1)
* @return The transformed easing value (may oscillate)
*/
static double easeInElastic(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    const double c4 = (2.0 * PI) / 3.0; // Elastic coefficient
    if (t == 0.0 || t == 1.0) {
        return t;
    }
    return -pow(2.0, 10.0 * t - 10.0) * sin((t * 10.0 - 10.75) * c4);
}

/**
* @brief Elastic ease-out (simulates elastic rebound)
* @param t Normalized time (0~1)
* @return The transformed easing value (may oscillate)
*/
static double easeOutElastic(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    const double c4 = (2.0 * PI) / 3.0;
    if (t == 0.0 || t == 1.0) {
        return t;
    }
    return pow(2.0, -10.0 * t) * sin((t * 10.0 - 0.75) * c4) + 1.0;
}

/**
* @brief Elastic ease-in-out
* @param t Normalized time (0~1)
* @return The transformed easing value (may oscillate)
*/
static double easeInOutElastic(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    const double c5 = (2.0 * PI) / 4.5;
    if (t == 0.0 || t == 1.0) {
        return t;
    }
    if (t < 0.5) {
        return -0.5 * pow(2.0, 20.0 * t - 10.0) * sin((20.0 * t - 11.125) * c5);
    }
    else {
        return pow(2.0, -20.0 * t + 10.0) * sin((20.0 * t - 11.125) * c5) * 0.5 + 1.0;
    }
}

/**
* @brief Bounce ease-out (simulates a bouncing rise)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeOutBounce(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    const double n1 = 7.5625; // Bounce coefficient
    const double d1 = 2.75;    // Bounce divisor
    if (t < 1.0 / d1) {
        return n1 * t * t;
    }
    else if (t < 2.0 / d1) {
        t -= 1.5 / d1;
        return n1 * t * t + 0.75;
    }
    else if (t < 2.5 / d1) {
        t -= 2.25 / d1;
        return n1 * t * t + 0.9375;
    }
    else {
        t -= 2.625 / d1;
        return n1 * t * t + 0.984375;
    }
}

/**
* @brief Bounce ease-in (simulates a bouncing landing)
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInBounce(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    return 1.0 - easeOutBounce(1.0 - t); // Reuses the ease-out logic, implemented in reverse
}

/**
* @brief Bounce ease-in-out
* @param t Normalized time (0~1)
* @return The transformed easing value (0~1)
*/
static double easeInOutBounce(double t) {
    t = std::fmax(0.0, std::fmin(1.0, t));
    if (t < 0.5) {
        return (1.0 - easeOutBounce(1.0 - 2.0 * t)) * 0.5;
    }
    else {
        return (easeOutBounce(2.0 * t - 1.0) + 1.0) * 0.5;
    }
}

/**
 * @brief Get the easing function pointer according to the enum value
 * @param function The easing function enum value
 * @return The pointer of the corresponding easing function, or nullptr if not found
 */
EasingFunction EasingFunctions::GetEasingFunction(EasingFunctionType function)
{
    // Static map: initialized only once, for better performance
    static std::map<EasingFunctionType, EasingFunction> easingFunctions;
    if (easingFunctions.empty()) {
        // Initialize the mapping between the enum values and the functions
        easingFunctions.insert(std::make_pair(EaseLinear,        easeLinear));
        easingFunctions.insert(std::make_pair(EaseInSine,        easeInSine));
        easingFunctions.insert(std::make_pair(EaseOutSine,       easeOutSine));
        easingFunctions.insert(std::make_pair(EaseInOutSine,     easeInOutSine));
        easingFunctions.insert(std::make_pair(EaseInQuad,        easeInQuad));
        easingFunctions.insert(std::make_pair(EaseOutQuad,       easeOutQuad));
        easingFunctions.insert(std::make_pair(EaseInOutQuad,     easeInOutQuad));
        easingFunctions.insert(std::make_pair(EaseInCubic,       easeInCubic));
        easingFunctions.insert(std::make_pair(EaseOutCubic,      easeOutCubic));
        easingFunctions.insert(std::make_pair(EaseInOutCubic,    easeInOutCubic));
        easingFunctions.insert(std::make_pair(EaseInQuart,       easeInQuart));
        easingFunctions.insert(std::make_pair(EaseOutQuart,      easeOutQuart));
        easingFunctions.insert(std::make_pair(EaseInOutQuart,    easeInOutQuart));
        easingFunctions.insert(std::make_pair(EaseInQuint,       easeInQuint));
        easingFunctions.insert(std::make_pair(EaseOutQuint,      easeOutQuint));
        easingFunctions.insert(std::make_pair(EaseInOutQuint,    easeInOutQuint));
        easingFunctions.insert(std::make_pair(EaseInExpo,        easeInExpo));
        easingFunctions.insert(std::make_pair(EaseOutExpo,       easeOutExpo));
        easingFunctions.insert(std::make_pair(EaseInOutExpo,     easeInOutExpo));
        easingFunctions.insert(std::make_pair(EaseInCirc,        easeInCirc));
        easingFunctions.insert(std::make_pair(EaseOutCirc,       easeOutCirc));
        easingFunctions.insert(std::make_pair(EaseInOutCirc,     easeInOutCirc));
        easingFunctions.insert(std::make_pair(EaseInBack,        easeInBack));
        easingFunctions.insert(std::make_pair(EaseOutBack,       easeOutBack));
        easingFunctions.insert(std::make_pair(EaseInOutBack,     easeInOutBack));
        easingFunctions.insert(std::make_pair(EaseInElastic,     easeInElastic));
        easingFunctions.insert(std::make_pair(EaseOutElastic,    easeOutElastic));
        easingFunctions.insert(std::make_pair(EaseInOutElastic,  easeInOutElastic));
        easingFunctions.insert(std::make_pair(EaseInBounce,      easeInBounce));
        easingFunctions.insert(std::make_pair(EaseOutBounce,     easeOutBounce));
        easingFunctions.insert(std::make_pair(EaseInOutBounce,   easeInOutBounce));
    }

    // Look up the corresponding function
    auto it = easingFunctions.find(function);
    return it == easingFunctions.end() ? nullptr : it->second;
}

EasingFunctionType EasingFunctions::GetEasingFunctionType(DString easingFunctionName)
{
    // Static map: initialized only once, for better performance
    static std::map<DString, EasingFunctionType> easingFunctions;
    if (easingFunctions.empty()) {
        // Initialize the mapping between the enum values and the functions
        easingFunctions.insert(std::make_pair(_T("EaseLinear"), EaseLinear));
        easingFunctions.insert(std::make_pair(_T("EaseInSine"), EaseInSine));
        easingFunctions.insert(std::make_pair(_T("EaseOutSine"), EaseOutSine));
        easingFunctions.insert(std::make_pair(_T("EaseInOutSine"), EaseInOutSine));
        easingFunctions.insert(std::make_pair(_T("EaseInQuad"), EaseInQuad));
        easingFunctions.insert(std::make_pair(_T("EaseOutQuad"), EaseOutQuad));
        easingFunctions.insert(std::make_pair(_T("EaseInOutQuad"), EaseInOutQuad));
        easingFunctions.insert(std::make_pair(_T("EaseInCubic"), EaseInCubic));
        easingFunctions.insert(std::make_pair(_T("EaseOutCubic"), EaseOutCubic));
        easingFunctions.insert(std::make_pair(_T("EaseInOutCubic"), EaseInOutCubic));
        easingFunctions.insert(std::make_pair(_T("EaseInQuart"), EaseInQuart));
        easingFunctions.insert(std::make_pair(_T("EaseOutQuart"), EaseOutQuart));
        easingFunctions.insert(std::make_pair(_T("EaseInOutQuart"), EaseInOutQuart));
        easingFunctions.insert(std::make_pair(_T("EaseInQuint"), EaseInQuint));
        easingFunctions.insert(std::make_pair(_T("EaseOutQuint"), EaseOutQuint));
        easingFunctions.insert(std::make_pair(_T("EaseInOutQuint"), EaseInOutQuint));
        easingFunctions.insert(std::make_pair(_T("EaseInExpo"), EaseInExpo));
        easingFunctions.insert(std::make_pair(_T("EaseOutExpo"), EaseOutExpo));
        easingFunctions.insert(std::make_pair(_T("EaseInOutExpo"), EaseInOutExpo));
        easingFunctions.insert(std::make_pair(_T("EaseInCirc"), EaseInCirc));
        easingFunctions.insert(std::make_pair(_T("EaseOutCirc"), EaseOutCirc));
        easingFunctions.insert(std::make_pair(_T("EaseInOutCirc"), EaseInOutCirc));
        easingFunctions.insert(std::make_pair(_T("EaseInBack"), EaseInBack));
        easingFunctions.insert(std::make_pair(_T("EaseOutBack"), EaseOutBack));
        easingFunctions.insert(std::make_pair(_T("EaseInOutBack"), EaseInOutBack));
        easingFunctions.insert(std::make_pair(_T("EaseInElastic"), EaseInElastic));
        easingFunctions.insert(std::make_pair(_T("EaseOutElastic"), EaseOutElastic));
        easingFunctions.insert(std::make_pair(_T("EaseInOutElastic"), EaseInOutElastic));
        easingFunctions.insert(std::make_pair(_T("EaseInBounce"), EaseInBounce));
        easingFunctions.insert(std::make_pair(_T("EaseOutBounce"), EaseOutBounce));
        easingFunctions.insert(std::make_pair(_T("EaseInOutBounce"), EaseInOutBounce));
    }

    if (!easingFunctionName.empty()) {
        if ((easingFunctionName[0] >= _T('a')) && (easingFunctionName[0] <= _T('z'))) {
            //Convert the first character to uppercase (the first character of the passed-in parameter may be lowercase)
            easingFunctionName[0] = easingFunctionName[0] - (_T('a') - _T('A'));
        }
    }
    // Look up the corresponding function
    auto it = easingFunctions.find(easingFunctionName);
    return it == easingFunctions.end() ? EaseLinear : it->second;
}

EasingFunctions::EasingFunctions(int32_t nStartValue, int32_t nEndValue, int32_t nFrameCount, EasingFunctionType easingFunctionType) :
    m_nStartValue(nStartValue),
    m_nEndValue(nEndValue),
    m_nFrameCount(nFrameCount)
{
    m_easingFunction = GetEasingFunction(easingFunctionType);
    ASSERT(m_easingFunction != nullptr);
    if (m_easingFunction == nullptr) {
        m_easingFunction = easeLinear;
    }
    ASSERT(m_nFrameCount > 0);
    if (m_nFrameCount < 1) {
        m_nFrameCount = 1;
    }
}

EasingFunctions::~EasingFunctions()
{

}

/** Easing interpolation function: eliminates floating-point precision errors, with optional precision truncation
 * @details The core function is to map the normalized time t in the 0~1 range to the numeric range [start, end] through the easing function.
 *          Key optimization: when t=1, the end value is forcibly returned, completely eliminating the precision errors caused by floating-point arithmetic.
 *
 * @param start The start value of the interpolation (e.g., initial coordinate, initial opacity, initial volume, etc.)
 * @param end The target value of the interpolation (e.g., target coordinate, target opacity, target volume, etc.)
 * @param t The normalized time value, theoretically in the 0~1 range (0=interpolation start, 1=interpolation end); the function clamps it to 0~1 internally
 * @param func The easing function pointer, an optional parameter (default nullptr); when provided, the specified easing effect is used, otherwise linear interpolation is used
 * @return double The final value after interpolation, without floating-point precision errors (when t=1, end is returned accurately)
 */
static double InterpolateFunction(double start, double end, double t, EasingFunction func = nullptr)
{
    // Boundary clamping: ensure t is always within 0~1 to avoid out-of-range results
    t = (t < 0) ? 0 : (t > 1.0) ? 1.0 : t;

    double result = 0.0;
    if (!func) {
        // Without an easing function, use linear interpolation
        result = start + (end - start) * t;
    }
    else {
        // With an easing function, first compute the eased t value, then map it to the [start, end] range
        double easedT = func(t);
        result = start + (end - start) * easedT;
    }

    // Key optimization: when t=1, forcibly return the target value, eliminating all precision errors accumulated by floating-point arithmetic
    if (t >= 1.0) {
        result = end;
    }
    else if (t <= 0.0) {
        result = start;
    }
    return result;
}

int32_t EasingFunctions::GetEasingValue(int32_t nCurrentFrame) const
{
    int32_t nEasingValue = m_nEndValue;
    ASSERT(nCurrentFrame >= 0);
    if (nCurrentFrame < 0) {
        return nEasingValue;
    }
    if (nCurrentFrame >= m_nFrameCount) {
        return nEasingValue;
    }
    if ((m_nEndValue == m_nStartValue) || (m_nFrameCount <= 0)) {
        return nEasingValue;
    }

    double start = static_cast<double>(m_nStartValue);
    double end = static_cast<double>(m_nEndValue);
    double t = static_cast<double>(nCurrentFrame) / static_cast<double>(m_nFrameCount);
    double fValue = InterpolateFunction(start, end, t, m_easingFunction);
    if (!std::isnan(fValue) && !std::isinf(fValue)) {
        nEasingValue = static_cast<int32_t>(std::round(fValue));
    }
    return nEasingValue;
}

int32_t EasingFunctions::GetStartValue() const
{
    return m_nStartValue;
}

int32_t EasingFunctions::GetEndValue() const
{
    return m_nEndValue;
}

int32_t EasingFunctions::GetFrameCount() const
{
    return m_nFrameCount;
}

} //namespace ui
