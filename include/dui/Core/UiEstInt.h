#ifndef UI_CORE_UIESTINT_H_
#define UI_CORE_UIESTINT_H_

#include "dui/dui_defs.h"
#include <cstdint>

namespace ui
{

/** Estimated length (or width) value of the control
*/
class DUI_API UiEstInt
{
public:
    // Value type definitions
    enum class Type
    {
        Int32   = 0,    //Normal 32-bit integer value, a fixed value
        Stretch = 1     //Stretch (the concrete value is set by the external container)
    };

    // Value type
    Type type : 2;

    // The concrete value:
    //  When type is Int32, it represents a fixed value
    //  When type is Stretch, the value range is (0,100], representing allocation by the parent container according to the percentage
    int32_t value : 30;

public:
    /** Construct a stretch-type value
    */
    static UiEstInt MakeStretch()
    {
        UiEstInt estInt;
        estInt.SetStretch();
        return estInt;
    }

    /** Construct a stretch-type value and specify the value
    */
    static UiEstInt MakeStretch(int32_t iValue)
    {
        UiEstInt estInt;
        estInt.SetStretch(iValue);
        return estInt;
    }

public:
    /** Constructor; defaults to the integer value 0
    */
    UiEstInt(): type(Type::Int32), value(0)
    { }

    /** Constructor; set to an integer value
    */
    explicit UiEstInt(int32_t iValue) : type(Type::Int32), value(iValue)
    { }

    /** Determine whether it is a stretch type
    */
    bool IsStretch() const { return type == Type::Stretch;}

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
        ASSERT((iValue > 0) && (iValue <= 100));
        if ((iValue > 0) && (iValue <= 100)) {
            value = iValue;
        }
        else {
            value = 100;
        }
    }

    /** Determine whether it is the same as another Size
    */
    bool Equals(const UiEstInt& dst) const
    {
        if (IsStretch() && dst.IsStretch()) {
            return value == dst.value;
        }
        else if (IsInt32() && dst.IsInt32()) {
            return value == dst.value;
        }
        return false;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiEstInt& a, const UiEstInt& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiEstInt& a, const UiEstInt& b)
    {
        return !a.Equals(b);
    }
};

}//namespace ui

#endif // UI_CORE_UIESTINT_H_
