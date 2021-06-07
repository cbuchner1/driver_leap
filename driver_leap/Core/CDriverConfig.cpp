#include "stdafx.h"

#include "Core/CDriverConfig.h"

#include "Utils/Utils.h"

extern char g_modulePath[];

const std::vector<std::string> g_settingNames
{
    "emulatedController", "leftHand", "rightHand", "orientation", "skeleton", "trackingLevel",
    "desktopOffset", "leftHandOffset", "leftHandOffsetRotation", "rightHandOffset", "rightHandOffsetRotation",
    "handsReset", "interpolation", "velocity"
};

enum ConfigSetting : size_t
{
    CS_EmulatedController = 0U,
    CS_LeftHand,
    CS_RightHand,
    CS_Orientation,
    CS_Skeleton,
    CS_TrackingLevel,
    CS_DesktopOffset,
    CS_LeftHandOffset,
    CS_LeftHandOffsetRotation,
    CS_RightHandOffset,
    CS_RightHandOffsetRotation,
    CS_HandsReset,
    CS_Interpolation,
    CS_Velocity
};

const std::vector<std::string> g_orientationModes
{
    "hmd", "desktop"
};

const std::vector<std::string> g_emulatedControllers
{
    "vive", "index", "oculus"
};

const std::vector<std::string> g_trackingLevels
{
    "partial", "full"
};

unsigned char CDriverConfig::ms_emulatedController = CDriverConfig::EC_Vive;
bool CDriverConfig::ms_leftHand = true;
bool CDriverConfig::ms_rightHand = true;
unsigned char CDriverConfig::ms_orientation = CDriverConfig::OM_HMD;
unsigned char CDriverConfig::ms_trackingLevel = CDriverConfig::TL_Partial;
glm::vec3 CDriverConfig::ms_desktopOffset(0.f, -0.5f, -0.5f);
glm::vec3 CDriverConfig::ms_leftHandOffset(0.f);
glm::quat CDriverConfig::ms_leftHandOffsetRotation(1.f, 0.f, 0.f, 0.f);
glm::vec3 CDriverConfig::ms_rightHandOffset(0.f);
glm::quat CDriverConfig::ms_rightHandOffsetRotation(1.f, 0.f, 0.f, 0.f);
bool CDriverConfig::ms_handsReset = false;
bool CDriverConfig::ms_interpolation = false;
bool CDriverConfig::ms_useVelocity = false;

void CDriverConfig::Load()
{
    std::string l_path(g_modulePath);
    l_path.erase(l_path.begin() + l_path.rfind('\\'), l_path.end());
    l_path.append("\\..\\..\\resources\\settings.xml");

    pugi::xml_document *l_document = new pugi::xml_document();
    if(l_document->load_file(l_path.c_str()))
    {
        const pugi::xml_node l_root = l_document->child("settings");
        if(l_root)
        {
            for(pugi::xml_node l_node = l_root.child("setting"); l_node; l_node = l_node.next_sibling("setting"))
            {
                const pugi::xml_attribute l_attribName = l_node.attribute("name");
                const pugi::xml_attribute l_attribValue = l_node.attribute("value");
                if(l_attribName && l_attribValue)
                {
                    switch(ReadEnumVector(l_attribName.as_string(), g_settingNames))
                    {
                        case ConfigSetting::CS_EmulatedController:
                        {
                            size_t l_tableIndex = ReadEnumVector(l_attribValue.as_string(), g_emulatedControllers);
                            if(l_tableIndex != std::numeric_limits<size_t>::max()) ms_emulatedController = static_cast<unsigned char>(l_tableIndex);
                        } break;
                        case ConfigSetting::CS_LeftHand:
                            ms_leftHand = l_attribValue.as_bool(true);
                            break;
                        case ConfigSetting::CS_RightHand:
                            ms_rightHand = l_attribValue.as_bool(true);
                            break;
                        case ConfigSetting::CS_Orientation:
                        {
                            size_t l_tableIndex = ReadEnumVector(l_attribValue.as_string(), g_orientationModes);
                            if(l_tableIndex != std::numeric_limits<size_t>::max()) ms_orientation = static_cast<unsigned char>(l_tableIndex);
                        } break;
                        case ConfigSetting::CS_TrackingLevel:
                        {
                            const size_t l_tableIndex = ReadEnumVector(l_attribValue.as_string(), g_trackingLevels);
                            if(l_tableIndex != std::numeric_limits<size_t>::max()) ms_trackingLevel = static_cast<unsigned char>(l_tableIndex);
                        } break;

                        case ConfigSetting::CS_DesktopOffset:
                        {
                            std::stringstream l_hmdOffset(l_attribValue.as_string());
                            l_hmdOffset >> ms_desktopOffset.x >> ms_desktopOffset.y >> ms_desktopOffset.z;
                        } break;
                        case ConfigSetting::CS_LeftHandOffset:
                        {
                            std::stringstream l_handOffset(l_attribValue.as_string());
                            l_handOffset >> ms_leftHandOffset.x >> ms_leftHandOffset.y >> ms_leftHandOffset.z;
                        } break;
                        case ConfigSetting::CS_LeftHandOffsetRotation:
                        {
                            std::stringstream l_handOffsetRotation(l_attribValue.as_string());
                            l_handOffsetRotation >> ms_leftHandOffsetRotation.x >> ms_leftHandOffsetRotation.y >> ms_leftHandOffsetRotation.z >> ms_leftHandOffsetRotation.w;
                        } break;
                        case ConfigSetting::CS_RightHandOffset:
                        {
                            std::stringstream l_handOffset(l_attribValue.as_string());
                            l_handOffset >> ms_rightHandOffset.x >> ms_rightHandOffset.y >> ms_rightHandOffset.z;
                        } break;
                        case ConfigSetting::CS_RightHandOffsetRotation:
                        {
                            std::stringstream l_handOffsetRotation(l_attribValue.as_string());
                            l_handOffsetRotation >> ms_rightHandOffsetRotation.x >> ms_rightHandOffsetRotation.y >> ms_rightHandOffsetRotation.z >> ms_rightHandOffsetRotation.w;
                        } break;
                        case ConfigSetting::CS_HandsReset:
                            ms_handsReset = l_attribValue.as_bool(false);
                            break;
                        case ConfigSetting::CS_Interpolation:
                            ms_interpolation = l_attribValue.as_bool(false);
                            break;
                        case ConfigSetting::CS_Velocity:
                            ms_useVelocity = l_attribValue.as_bool(false);
                            break;
                    }
                }
            }
        }
    }
    delete l_document;
}

unsigned char CDriverConfig::GetEmulatedController()
{
    return ms_emulatedController;
}

bool CDriverConfig::IsLeftHandEnabled()
{
    return ms_leftHand;
}

bool CDriverConfig::IsRightHandEnabled()
{
    return ms_rightHand;
}

unsigned char CDriverConfig::GetOrientationMode()
{
    return ms_orientation;
}

unsigned char CDriverConfig::GetTrackingLevel()
{
    return ms_trackingLevel;
}

const glm::vec3& CDriverConfig::GetDesktopOffset()
{
    return ms_desktopOffset;
}

const glm::vec3& CDriverConfig::GetLeftHandOffset()
{
    return ms_leftHandOffset;
}

const glm::quat& CDriverConfig::GetLeftHandOffsetRotation()
{
    return ms_leftHandOffsetRotation;
}

const glm::vec3& CDriverConfig::GetRightHandOffset()
{
    return ms_rightHandOffset;
}

const glm::quat& CDriverConfig::GetRightHandOffsetRotation()
{
    return ms_rightHandOffsetRotation;
}

bool CDriverConfig::IsHandsResetEnabled()
{
    return ms_handsReset;
}

bool CDriverConfig::IsInterpolationEnabled()
{
    return ms_interpolation;
}

bool CDriverConfig::IsVelocityUsed()
{
    return ms_useVelocity;
}
