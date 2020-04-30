#pragma once

namespace sh
{
    // From glfw3.h
    enum class ButtonCode
    {
        B1 = 0,
        B2 = 1,
        B3 = 2,
        B4 = 3,
        B5 = 4,
        B6 = 5,
        B7 = 6,
        B8 = 7,
        Last = B8,
        Left = B1,
        Right = B2,
        Middle = B3,
    };

    inline int AsInt(ButtonCode code)
    {
        return static_cast<int>(code);
    }
    inline bool operator==(int i, ButtonCode code)
    {
        return i == static_cast<int>(code);
    }
    inline bool operator==(ButtonCode code, int i)
    {
        return i == code;
    }

}