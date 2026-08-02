#ifndef UI_CORE_UICOLORS_H_
#define UI_CORE_UICOLORS_H_

#include "duilib/duilib_defs.h"
#include <vector>
#include <string>

namespace ui 
{
namespace UiColors
{
    /** Common color value constants (ARGB format)
    */
    enum
    {
        AliceBlue = 0xFFF0F8FF,     //Alice blue, close to a light blue-gray or steel blue
        AntiqueWhite = 0xFFFAEBD7,  //Antique white
        Aqua = 0xFF00FFFF,          //Light green (aqua)
        Aquamarine = 0xFF7FFFD4,    //Aquamarine, blue-green, between green and blue
        Azure = 0xFFF0FFFF,         //Azure blue
        Beige = 0xFFF5F5DC,         //Beige
        Bisque = 0xFFFFE4C4,        //Orange yellow (bisque)
        Black = 0xFF000000,         //Black
        BlanchedAlmond = 0xFFFFEBCD,//Blanched almond
        Blue = 0xFF0000FF,          //Blue
        BlueViolet = 0xFF8A2BE2,    //Blue-violet
        Brown = 0xFFA52A2A,         //Brown
        BurlyWood = 0xFFDEB887,     //Burlywood
        CadetBlue = 0xFF5F9EA0,     //Cadet blue
        Chartreuse = 0xFF7FFF00,    //Yellow-green (chartreuse)
        Chocolate = 0xFFD2691E,     //Chocolate
        Coral = 0xFFFF7F50,         //Coral
        CornflowerBlue = 0xFF6495ED,//Cornflower blue
        Cornsilk = 0xFFFFF8DC,      //Cornsilk
        Crimson = 0xFFDC143C,       //Dark red (crimson)
        Cyan = 0xFF00FFFF,          //Cyan
        DarkBlue = 0xFF00008B,      //Dark blue
        DarkCyan = 0xFF008B8B,      //Dark cyan
        DarkGoldenrod = 0xFFB8860B, //Dark goldenrod
        DarkGray = 0xFFA9A9A9,      //Dark gray
        DarkGreen = 0xFF006400,     //Dark green
        DarkKhaki = 0xFFBDB76B,     //Dark khaki, deep yellowish brown
        DarkMagenta = 0xFF8B008B,   //Dark magenta
        DarkOliveGreen = 0xFF556B2F,//Dark olive green
        DarkOrange = 0xFFFF8C00,    //Dark orange
        DarkOrchid = 0xFF9932CC,    //Dark orchid (dark purple)
        DarkRed = 0xFF8B0000,       //Dark red
        DarkSalmon = 0xFFE9967A,    //Dark salmon (dark flesh color)
        DarkSeaGreen = 0xFF8FBC8B,  //Dark sea green
        DarkSlateBlue = 0xFF483D8B, //Dark slate blue
        DarkSlateGray = 0xFF2F4F4F, //Dark slate gray
        DarkTurquoise = 0xFF00CED1, //Dark turquoise
        DarkViolet = 0xFF9400D3,    //Dark violet
        DeepPink = 0xFFFF1493,      //Deep pink
        DeepSkyBlue = 0xFF00BFFF,   //Deep sky blue
        DimGray = 0xFF696969,       //Dim gray
        DodgerBlue = 0xFF1E90FF,    //Dodger blue
        Firebrick = 0xFFB22222,     //Firebrick
        FloralWhite = 0xFFFFFAF0,   //Floral white
        ForestGreen = 0xFF228B22,   //Forest green
        Fuchsia = 0xFFFF00FF,       //Fuchsia (purple-red)
        Gainsboro = 0xFFDCDCDC,     //Gainsboro (light gray)
        GhostWhite = 0xFFF8F8FF,    //Ghost white
        Gold = 0xFFFFD700,          //Gold
        Goldenrod = 0xFFDAA520,     //Goldenrod
        Gray = 0xFF808080,          //Gray
        Green = 0xFF008000,         //Green
        GreenYellow = 0xFFADFF2F,   //Green-yellow
        Honeydew = 0xFFF0FFF0,      //Honeydew
        HotPink = 0xFFFF69B4,       //Hot pink
        IndianRed = 0xFFCD5C5C,     //Indian red
        Indigo = 0xFF4B0082,        //Indigo
        Ivory = 0xFFFFFFF0,         //Ivory
        Khaki = 0xFFF0E68C,         //Khaki (yellowish brown)
        Lavender = 0xFFE6E6FA,      //Lavender, light purple
        LavenderBlush = 0xFFFFF0F5, //Lavender blush
        LawnGreen = 0xFF7CFC00,     //Lawn green
        LemonChiffon = 0xFFFFFACD,  //Lemon chiffon
        LightBlue = 0xFFADD8E6,     //Light blue
        LightCoral = 0xFFF08080,    //Light coral
        LightCyan = 0xFFE0FFFF,     //Light cyan
        LightGoldenrodYellow = 0xFFFAFAD2,  //Light goldenrod yellow
        LightGray = 0xFFD3D3D3,     //Light gray
        LightGreen = 0xFF90EE90,    //Light green
        LightPink = 0xFFFFB6C1,     //Light pink
        LightSalmon = 0xFFFFA07A,   //Light salmon (light flesh color)
        LightSeaGreen = 0xFF20B2AA, //Light sea green
        LightSkyBlue = 0xFF87CEFA,  //Light sky blue
        LightSlateGray = 0xFF778899,//Light slate gray
        LightSteelBlue = 0xFFB0C4DE,//Light steel blue
        LightYellow = 0xFFFFFFE0,   //Light yellow
        Lime = 0xFF00FF00,          //Lime
        LimeGreen = 0xFF32CD32,     //Lime green
        Linen = 0xFFFAF0E6,         //Linen
        Magenta = 0xFFFF00FF,       //Magenta
        Maroon = 0xFF800000,        //Maroon (purple-brown)
        MediumAquamarine = 0xFF66CDAA,  //Medium aquamarine
        MediumBlue = 0xFF0000CD,    //Medium blue
        MediumOrchid = 0xFFBA55D3,  //Medium orchid
        MediumPurple = 0xFF9370DB,  //Medium purple
        MediumSeaGreen = 0xFF3CB371,    //Medium sea green
        MediumSlateBlue = 0xFF7B68EE,   //Medium slate blue
        MediumSpringGreen = 0xFF00FA9A, //Medium spring green
        MediumTurquoise = 0xFF48D1CC,   //Medium turquoise
        MediumVioletRed = 0xFFC71585,   //Medium violet red
        MidnightBlue = 0xFF191970,      //Midnight blue
        MintCream = 0xFFF5FFFA,         //Mint cream
        MistyRose = 0xFFFFE4E1,         //Misty rose
        Moccasin = 0xFFFFE4B5,          //Moccasin (deerskin color)
        NavajoWhite = 0xFFFFDEAD,       //Navajo white
        Navy = 0xFF000080,              //Navy blue
        OldLace = 0xFFFDF5E6,           //Old lace (light beige)
        Olive = 0xFF808000,             //Olive
        OliveDrab = 0xFF6B8E23,         //Olive drab (dark yellowish green)
        Orange = 0xFFFFA500,            //Orange
        OrangeRed = 0xFFFF4500,         //Orange red
        Orchid = 0xFFDA70D6,            //Orchid
        PaleGoldenrod = 0xFFEEE8AA,     //Pale goldenrod (light yellow)
        PaleGreen = 0xFF98FB98,         //Pale green
        PaleTurquoise = 0xFFAFEEEE,     //Pale turquoise
        PaleVioletRed = 0xFFDB7093,     //Pale violet red
        PapayaWhip = 0xFFFFEFD5,        //Papaya whip
        PeachPuff = 0xFFFFDAB9,         //Peach puff
        Peru = 0xFFCD853F,              //Peru
        Pink = 0xFFFFC0CB,              //Pink
        Plum = 0xFFDDA0DD,              //Plum
        PowderBlue = 0xFFB0E0E6,        //Powder blue
        Purple = 0xFF800080,            //Purple
        Red = 0xFFFF0000,               //Red
        RosyBrown = 0xFFBC8F8F,     //Rosy brown
        RoyalBlue = 0xFF4169E1,     //Royal blue
        SaddleBrown = 0xFF8B4513,   //Saddle brown
        Salmon = 0xFFFA8072,        //Salmon
        SandyBrown = 0xFFF4A460,    //Sandy brown
        SeaGreen = 0xFF2E8B57,      //Sea green
        SeaShell = 0xFFFFF5EE,      //Seashell
        Sienna = 0xFFA0522D,        //Sienna
        Silver = 0xFFC0C0C0,        //Silver
        SkyBlue = 0xFF87CEEB,       //Sky blue
        SlateBlue = 0xFF6A5ACD,     //Slate blue
        SlateGray = 0xFF708090,     //Slate gray
        Snow = 0xFFFFFAFA,          //Snow white
        SpringGreen = 0xFF00FF7F,   //Spring green
        SteelBlue = 0xFF4682B4,     //Steel blue
        Tan = 0xFFD2B48C,           //Tan (brownish)
        Teal = 0xFF008080,          //Teal
        Thistle = 0xFFD8BFD8,       //Thistle, a kind of purple
        Tomato = 0xFFFF6347,        //Tomato
        Transparent = 0x00FFFFFF,   //Transparent
        Turquoise = 0xFF40E0D0,     //Turquoise
        Violet = 0xFFEE82EE,        //Violet
        Wheat = 0xFFF5DEB3,         //Wheat, light yellow
        White = 0xFFFFFFFF,         //White
        WhiteSmoke = 0xFFF5F5F5,    //White smoke
        Yellow = 0xFFFFFF00,        //Yellow
        YellowGreen = 0xFF9ACD32    //Yellow-green
    };

    /** String constants corresponding to common color values
    */
    void GetUiColorsString(std::vector<std::pair<DString, int32_t>>& uiColors);

}// namespace UiColors
} // namespace ui

#endif // UI_CORE_UICOLORS_H_
