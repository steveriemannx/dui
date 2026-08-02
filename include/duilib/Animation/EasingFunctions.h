#ifndef UI_ANIMATION_EASING_FUNCTIONS_H_
#define UI_ANIMATION_EASING_FUNCTIONS_H_

////////////////////////////////////////////////////////////////
//Reference for easing effects: https://easings.net/

#include "duilib/duilib_defs.h"

namespace ui
{
/** Enum of the easing function types
 * Covers the common easing curves: sine, quadratic, cubic, quartic, quintic, exponential, circular, back, elastic, bounce
 * Each curve has three forms: In (ease-in), Out (ease-out), InOut (ease-in-out)
 */
enum DUILIB_API EasingFunctionType: uint8_t
{
    EaseLinear,          // Linear function

    EaseInSine,          // Sine ease-in
    EaseOutSine,         // Sine ease-out
    EaseInOutSine,       // Sine ease-in-out
    EaseInQuad,          // Quadratic ease-in
    EaseOutQuad,         // Quadratic ease-out
    EaseInOutQuad,       // Quadratic ease-in-out
    EaseInCubic,         // Cubic ease-in
    EaseOutCubic,        // Cubic ease-out
    EaseInOutCubic,      // Cubic ease-in-out
    EaseInQuart,         // Quartic ease-in
    EaseOutQuart,        // Quartic ease-out
    EaseInOutQuart,      // Quartic ease-in-out
    EaseInQuint,         // Quintic ease-in
    EaseOutQuint,        // Quintic ease-out
    EaseInOutQuint,      // Quintic ease-in-out
    EaseInExpo,          // Exponential ease-in
    EaseOutExpo,         // Exponential ease-out
    EaseInOutExpo,       // Exponential ease-in-out
    EaseInCirc,          // Circular ease-in
    EaseOutCirc,         // Circular ease-out
    EaseInOutCirc,       // Circular ease-in-out
    EaseInBack,          // Back ease-in (moves backward first, then forward)
    EaseOutBack,         // Back ease-out (overshoots the target, then springs back)
    EaseInOutBack,       // Back ease-in-out
    EaseInElastic,       // Elastic ease-in
    EaseOutElastic,      // Elastic ease-out
    EaseInOutElastic,    // Elastic ease-in-out
    EaseInBounce,        // Bounce ease-in
    EaseOutBounce,       // Bounce ease-out
    EaseInOutBounce      // Bounce ease-in-out
};

/** Forward declaration of the function pointer type of the easing functions
*/
typedef double(*EasingFunction)(double);

/** Wrapper of the interpolation function of the easing functions (for use by the application layer)
*/
class DUILIB_API EasingFunctions
{
public:
    /** Get the corresponding easing function pointer according to the enum type
     * @param function The easing function enum value
     * @return The pointer of the corresponding easing function, or nullptr if it does not exist
     */
    static EasingFunction GetEasingFunction(EasingFunctionType function);

    /** Get the easing function type according to the string name of the easing function
    * @param [in] easingFunctionName The name of the easing function; the string name is exactly the same as the enum value definition of EasingFunctionType
    */
    static EasingFunctionType GetEasingFunctionType(DString easingFunctionName);

public:
    /** Constructor
    * @param [in] nStartValue The start value
    * @param [in] nEndValue The end value
    * @param [in] nFrameCount The total frame count of the animation
    * @param [in] easingFunctionType The easing function type to use
    */
    EasingFunctions(int32_t nStartValue, int32_t nEndValue, int32_t nFrameCount, EasingFunctionType easingFunctionType);
    ~EasingFunctions();

public:
    /** Get the eased value
    * @param [in] nCurrentFrame The index of the current frame, starting from 0; the valid range is [0, nFrameCount], a closed interval
    * @return Returns the eased value; the valid range is [nStartValue, nEndValue], a closed interval
    */
    int32_t GetEasingValue(int32_t nCurrentFrame) const;

    /** Get the start value of the animation playback
    */
    int32_t GetStartValue() const;

    /** Get the end value of the animation playback
    */
    int32_t GetEndValue() const;

    /** Get the total frame count of the animation
    */
    int32_t GetFrameCount() const;

private:
    /** Start value (may be width, height, opacity, etc.)
    */
    int32_t m_nStartValue;

    /** End value (may be width, height, opacity, etc.)
    */
    int32_t m_nEndValue;

    /** Total frame count of the animation
    */
    int32_t m_nFrameCount;

    /** The easing function used by the animation
    */
    EasingFunction m_easingFunction;
};

} //namespace ui

#endif //#define UI_ANIMATION_EASING_FUNCTIONS_H_
