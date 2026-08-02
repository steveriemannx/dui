#ifndef UI_CORE_UITYPES_H_
#define UI_CORE_UITYPES_H_

#include "duilib/Core/UiSize.h"
#include "duilib/Core/UiSize64.h"
#include "duilib/Core/UiSize16.h"
#include "duilib/Core/UiPoint.h"
#include "duilib/Core/UiPointF.h"
#include "duilib/Core/UiRect.h"
#include "duilib/Core/UiRectF.h"
#include "duilib/Core/UiColor.h"
#include "duilib/Core/UiFont.h"
#include "duilib/Core/UiPadding.h"
#include "duilib/Core/UiMargin.h"
#include "duilib/Core/UiFixedInt.h"
#include "duilib/Core/UiEstInt.h"
#include "duilib/Core/UiString.h"
#include <string>
#include <memory>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

namespace ui
{
    /** Data type of the loaded image
    */
    enum class UiImageDataType
    {
        kBGRA,  //In BGRA format; the bit order from high to low is [bit 3: A, bit 2: R, bit 1: G, bit 0: B] (used on the Windows platform)
        kRGBA   //In RGBA format; the bit order from high to low is [bit 3: A, bit 2: B, bit 1: G, bit 0: R] (used on other platforms)
    };

    /** Data of the loaded image
    */
    struct UiImageData
    {
        /** Bitmap data; the data length of each frame of the image is fixed as: the image data length is (m_imageHeight*m_imageWidth*4)
        */
        std::vector<uint8_t> m_imageData;

        /** Bitmap data format
        */
        UiImageDataType m_imageDataType;

        /** Image width
        */
        uint32_t m_imageWidth = 0;

        /** Image height
        */
        uint32_t m_imageHeight = 0;
    };

    /** Parameters related to tiled drawing when drawing an image
    */
    struct TiledDrawParam
    {
        bool m_bTiledX = false;       //Whether to tile horizontally
        bool m_bTiledY = false;       //Whether to tile vertically

        bool m_bFullTiledX = false;   //If true, when drawing with horizontal tiling, ensure the image is drawn completely; this parameter only takes effect when bTiledX is true
        bool m_bFullTiledY = false;   //If true, when drawing with vertical tiling, ensure the image is drawn completely; this parameter only takes effect when bTiledY is true

        int32_t m_nTiledMarginX = 0;  //The horizontal spacing of the image when tiled drawing; only takes effect when bTiledX is valid (not DPI-scaled)
        int32_t m_nTiledMarginY = 0;  //The vertical spacing of the image when tiled drawing; only takes effect when bTiledY is valid (not DPI-scaled)

        UiPadding m_rcTiledPadding;   //When tiled drawing, the inner padding within the target area (the padding combined with TiledMargin can form a grid) (not DPI-scaled)
    };

    /** The type of estimated image size
    */
    enum class EstimateImageType
    {
        kBoth,          //Estimate both the width and the height
        kWidthOnly,     //Estimate only the width
        kHeightOnly     //Estimate only the height
    };

    /** The size set on the control
    */
    class UiFixedSize
    {
    public:
        /** Width
        */
        UiFixedInt cx;

        /** Height
        */
        UiFixedInt cy;
    };

    /** Estimated control size (compared to UiFixedSize, there is no Auto type)
    */
    class UiEstSize
    {
    public:
        /** Width
        */
        UiEstInt cx;

        /** Height
        */
        UiEstInt cy;
    };

    /** Result of estimating the control size
    */
    class UiEstResult
    {
    public:
        /** The available rectangle size during this estimation (this parameter affects the estimation result)
        */
        UiSize m_szAvailable;

        /** The estimated size of the control (length and width), equivalent to the cached value after estimation
        */
        UiEstSize m_szEstimateSize;
    };

    /** Generate UiEstInt from UiFixedInt
    */
    inline UiEstInt MakeEstInt(const UiFixedInt& fixedInt)
    {
        UiEstInt estInt;
        if (fixedInt.IsStretch()) {
            estInt.SetStretch(fixedInt.GetStretchPercentValue());
        }
        else if (fixedInt.IsInt32()) {
            estInt.SetInt32(fixedInt.GetInt32());
        }
        else {
            estInt.SetInt32(0);
        }
        return estInt;
    }

    /** Calculate the length value of the stretch type
    */
    inline int32_t CalcStretchValue(const UiEstInt& estInt, int32_t nAvailable)
    {
        if (estInt.IsStretch()) {
            int32_t nStretchValue = estInt.GetStretchPercentValue(); //Represents a percentage value; the value range is (0, 100]
            if ((nStretchValue > 0) && (nStretchValue < 100)) {
                // Return the value calculated by percentage, rounded
                return static_cast<int32_t>(nAvailable * nStretchValue / 100.0f + 0.5f);
            }
            else {
                // Return the original value
                return nAvailable;
            }
        }
        // In other cases, return the original value (fault tolerance)
        return nAvailable;
    }

    /** Calculate the length value of the stretch type
    */
    inline int32_t CalcStretchValue(const UiFixedInt& fixedInt, int32_t nAvailable)
    {
        if (fixedInt.IsStretch()) {
            int32_t nStretchValue = fixedInt.GetStretchPercentValue(); //Represents a percentage value; the value range is (0, 100]
            if ((nStretchValue > 0) && (nStretchValue < 100)) {
                // Return the value calculated by percentage, rounded
                return static_cast<int32_t>(nAvailable * nStretchValue / 100.0f + 0.5f);
            }
            else {
                // Return the original value
                return nAvailable;
            }
        }
        // In other cases, return the original value (fault tolerance)
        return nAvailable;
    }

    /** Generate UiEstSize from UiFixedSize
    */
    inline UiEstSize MakeEstSize(const UiFixedSize& fixedSize)
    {
        UiEstSize estSize;
        estSize.cx = MakeEstInt(fixedSize.cx);
        estSize.cy = MakeEstInt(fixedSize.cy);
        return estSize;
    }

    /** Generate UiEstSize from UiSize
    */
    inline UiEstSize MakeEstSize(const UiSize& size)
    {
        UiEstSize estSize;
        estSize.cx = UiEstInt(size.cx);
        estSize.cy = UiEstInt(size.cy);
        return estSize;
    }

    /** Generate UiEstSize from UiSize
    */
    inline UiSize MakeSize(const UiEstSize& estSize)
    {
        ASSERT(estSize.cx.IsInt32() && estSize.cy.IsInt32());
        UiSize size(estSize.cx.GetInt32(), estSize.cy.GetInt32());
        return size;
    }

    /** Convert a 64-bit integer value to a 32-bit integer value
    */
    inline int32_t TruncateToInt32(int64_t x)
    {
        ASSERT((x >= INT32_MIN) && (x <= INT32_MAX) );
        x = x < INT32_MAX ? x : INT32_MAX;
        x = x > INT32_MIN ? x : INT32_MIN;
        return static_cast<int32_t>(x);
    }

    /** Convert a 32-bit signed integer value to a 16-bit unsigned integer value
    */
    inline uint16_t TruncateToUInt16(uint32_t x)
    {
        ASSERT(x <= UINT16_MAX);
        x = x < UINT16_MAX ? x : UINT16_MAX;
        return static_cast<uint16_t>(x);
    }

    /** Convert a 32-bit unsigned integer value to a 16-bit unsigned integer value
    */
    inline uint16_t TruncateToUInt16(int32_t x)
    {
        ASSERT((x >= 0) && (x <= UINT16_MAX));
        x = x > 0 ? x : 0;
        x = x < UINT16_MAX ? x : UINT16_MAX;
        return static_cast<uint16_t>(x);
    }

    /** Convert a 32-bit signed integer value to a 16-bit unsigned integer value
    */
    inline int16_t TruncateToInt16(int32_t x)
    {
        ASSERT((x >= 0) && (x <= INT16_MAX));
        x = x > 0 ? x : 0;
        x = x < INT16_MAX ? x : INT16_MAX;
        return static_cast<int16_t>(x);
    }

    /** Convert a 32-bit integer value to an 8-bit integer value
    */
    inline int8_t TruncateToInt8(int32_t x)
    {
        ASSERT((x >= INT8_MIN) && (x <= INT8_MAX));
        x = x < INT8_MAX ? x : INT8_MAX;
        x = x > INT8_MIN ? x : INT8_MIN;
        return static_cast<int8_t>(x);
    }

    /** Convert a 32-bit integer value to an 8-bit integer value
    */
    inline uint8_t TruncateToUInt8(int32_t x)
    {
        ASSERT((x >= 0) && (x <= UINT8_MAX));
        x = x > 0 ? x : 0;
        x = x < UINT8_MAX ? x : UINT8_MAX;
        return static_cast<uint8_t>(x);
    }

    /** Wrapper for std::ceil (differs from the std implementation for negative numbers)
    */
    inline float CEILF(float fValue)
    {
        if (fValue > 0) {
            return std::ceil(fValue);
        }
        else {
            return -std::ceil(-fValue);
        }        
    }

    /** Determine whether two floating-point numbers are the same
    */
    inline bool IsFloatEqual(float x, float y)
    {
        return std::fabs(x - y) < 0.00001f;
    }

}//namespace ui

#endif // UI_CORE_UITYPES_H_
