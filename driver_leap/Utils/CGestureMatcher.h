#pragma once

class CGestureMatcher
{
    static float NormalizeRange(float f_val, float f_min, float f_max);
public:
    enum HandGesture : size_t
    {
        // Finger bends
        HG_ThumbBend = 0U,
        HG_IndexBend,
        HG_MiddleBend,
        HG_RingBend,
        HG_PinkyBend,

        // Simple gestures
        HG_Trigger,
        HG_Grab,
        HG_ThumbPress,

        // Two-handed gesture
        HG_OpisthenarTouch,
        HG_PalmTouch,
        HG_PalmPointX,
        HG_PalmPointY,
        HG_ThumbCrossTouch,
        HG_MiddleCrossTouch,

        HG_Count
    };

    static void GetGestures(const LEAP_HAND *f_hand, std::vector<float> &f_result, const LEAP_HAND *f_oppHand = nullptr);
};
