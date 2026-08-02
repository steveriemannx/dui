#ifndef UI_CORE_UIFIXEDINT_H_
#define UI_CORE_UIFIXEDINT_H_

#include "duilib/duilib_defs.h"
#include <cstdint>

namespace ui
{

/** Length (or width) value set on the control
*/
class DUILIB_API UiFixedInt
{
public:
    // Value type definitions
    enum class Type
    {
        Int32   = 0,    //Normal 32-bit integer value, a fixed value
        Stretch = 1,    //Stretch (the concrete value is set by the external container)
        Auto    = 2     //Auto (the concrete value is determined by the control's own content, e.g. the size of the background image, the size of the text area, etc.)
    };

    // Value type
    Type type;

    // The concrete value:
    //  When type is Int32, it represents a fixed value
    //  When type is Stretch, the value range is (0,100], representing allocation by the parent container according to the percentage
    int32_t value;

public:
    /** Construct a stretch-type value, with the value 100
    */
    static UiFixedInt MakeStretch()
    {
        UiFixedInt fixedInt;
        fixedInt.SetStretch();
        return fixedInt;
    }

    /** Construct a stretch-type value and specify the value
    * @param [in] iValue Stretch percentage value; for example, 25 represents 25%, meaning allocation by the parent container at 25%
    */
    static UiFixedInt MakeStretch(int32_t iValue)
    {
        UiFixedInt fixedInt;
        fixedInt.SetStretch(iValue);
        return fixedInt;
    }

    /** Construct an auto-type value
    */
    static UiFixedInt MakeAuto()
    {
        UiFixedInt fixedInt;
        fixedInt.SetAuto();
        return fixedInt;
    }

    /** Construct an integer value
    */
    static UiFixedInt MakeInt(int32_t iValue)
    {
        UiFixedInt fixedInt(iValue);
        return fixedInt;
    }

public:
    /** Constructor; defaults to the integer value 0
    */
    UiFixedInt(): type(Type::Int32), value(0)
    { }

    /** Constructor; set to an integer value
    */
    explicit UiFixedInt(int32_t iValue) : type(Type::Int32), value(iValue)
    { }

    /** Determine whether it is a stretch type
    */
    bool IsStretch() const { return type == Type::Stretch;}

    /** Determine whether it is an auto type
    */
    bool IsAuto() const { return type == Type::Auto; }

    /** Determine whether it is a 32-bit integer value type
    */
    bool IsInt32() const { return type == Type::Int32; }

    /** Whether it is a valid value (a numeric type representing a length; must be greater than or equal to zero)
    */
    bool IsValid() const
    {
        if (type == Type::Int32) {
            return value >= 0;
        }
        return true;
    }

    /** Assign a 32-bit integer value
    */
    void SetInt32(int32_t iValue)
    {
        ASSERT(iValue >= 0);
        type = Type::Int32;
        value = iValue >= 0 ? iValue : 0;
    }

    /** Get the 32-bit integer value
    */
    int32_t GetInt32() const 
    { 
        if (type != Type::Int32) {
            return 0;
        }
        return value >= 0 ? value : 0; 
    }

    /** Get the stretch-type value; valid values (0, 100], representing a percentage
    */
    int32_t GetStretchPercentValue() const
    {
        if (type != Type::Stretch) {
            return 0;
        }
        if ((value > 0) && (value <= 100)) {
            return value;
        }
        return 100;
    }

    /** Assign the stretch type
    */
    void SetStretch() 
    { 
        type = Type::Stretch; 
        value = 100;
    }

    /** Assign the stretch type and specify the value
    */
    void SetStretch(int32_t iValue)
    {
        type = Type::Stretch;
        if ((iValue > 0) && (iValue <= 100)) {
            value = iValue;
        }
        else {
            value = 100;
        }
    }

    /** Assign the auto type
    */
    void SetAuto() 
    { 
        type = Type::Auto; 
        value = 0;
    }

    /** Determine whether it is the same as another Size
    */
    bool Equals(const UiFixedInt& dst) const
    {
        if (IsStretch() && dst.IsStretch()) {
            return value == dst.value;
        }
        else if (IsAuto() && dst.IsAuto()) {
            return true;
        }
        else if (IsInt32() && dst.IsInt32()) {
            return value == dst.value;
        }
        return false;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiFixedInt& a, const UiFixedInt& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiFixedInt& a, const UiFixedInt& b)
    {
        return !a.Equals(b);
    }
};

}//namespace ui

#endif // UI_CORE_UIFIXEDINT_H_
