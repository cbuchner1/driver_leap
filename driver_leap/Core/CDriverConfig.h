#pragma once

class CDriverConfig final
{
    static unsigned char ms_emulatedController;
    static bool ms_leftHand;
    static bool ms_rightHand;
    static unsigned char ms_orientation;
    static unsigned char ms_trackingLevel;
    static glm::vec3 ms_desktopOffset;
    static glm::vec3 ms_leftHandOffset;
    static glm::quat ms_leftHandOffsetRotation;
    static glm::vec3 ms_rightHandOffset;
    static glm::quat ms_rightHandOffsetRotation;
    static bool ms_handsReset;
    static bool ms_interpolation;
    static bool ms_useVelocity;

    CDriverConfig() = delete;
    ~CDriverConfig() = delete;
    CDriverConfig(const CDriverConfig &that) = delete;
    CDriverConfig& operator=(const CDriverConfig &that) = delete;
public:
    enum EmulatedController : unsigned char
    {
        EC_Vive = 0U,
        EC_Index,
        EC_Oculus
    };
    enum OrientationMode : unsigned char
    {
        OM_HMD = 0U,
        OM_Desktop
    };
    enum TrackingLevel : unsigned char
    {
        TL_Partial = 0U,
        TL_Full
    };

    static void Load();

    static unsigned char GetEmulatedController();
    static bool IsLeftHandEnabled();
    static bool IsRightHandEnabled();
    static unsigned char GetOrientationMode();
    static unsigned char GetTrackingLevel();

    static const glm::vec3& GetDesktopOffset();
    static const glm::vec3& GetLeftHandOffset();
    static const glm::quat& GetLeftHandOffsetRotation();
    static const glm::vec3& GetRightHandOffset();
    static const glm::quat& GetRightHandOffsetRotation();

    static bool IsHandsResetEnabled();
    static bool IsInterpolationEnabled();
    static bool IsVelocityUsed();
};
