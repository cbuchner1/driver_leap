#include "stdafx.h"

#include "Devices/CLeapController/CLeapControllerOculus.h"

#include "Devices/CLeapController/CControllerButton.h"
#include "Utils/CGestureMatcher.h"

enum TouchButtons : size_t
{
    TB_JoystickClick = 0U,
    TB_JoystickTouch,
    TB_JoystickX,
    TB_JoystickY,
    TB_TriggerTouch,
    TB_TriggerValue,
    TB_GripTouch,
    TB_GripValue,
    TB_ATouch,
    TB_AClick,
    TB_BTouch,
    TB_BClick,
    TB_XTouch,
    TB_XClick,
    TB_YTouch,
    TB_YClick,
    TB_SystemTouch,
    TB_SystemClick,

    TB_Count
};

CLeapControllerOculus::CLeapControllerOculus(unsigned char f_hand)
{
    m_hand = (f_hand%CH_Count);
    m_type = CT_OculusTouch;
    m_serialNumber.assign("WMHD316J600000_Controller_");
    m_serialNumber.append((m_hand == CH_Left) ? "Left" : "Right");
}

CLeapControllerOculus::~CLeapControllerOculus()
{
}

void CLeapControllerOculus::ActivateInternal()
{
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_TrackingSystemName_String, "oculus");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ModelNumber_String, (m_hand == CH_Left) ? "Oculus Rift CV1(Left Controller)" : "Oculus Rift CV1 (Right Controller)");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_SerialNumber_String, m_serialNumber.c_str());
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, (m_hand == CH_Left) ? "oculus_cv1_controller_left" : "oculus_cv1_controller_right");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ManufacturerName_String, "Oculus");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_HardwareRevision_String, "14");
    vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_HardwareRevision_Uint64, 14U);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_Controller);
    //vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_ParentDriver_Uint64, 8589934599U); // Strange value from dump
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ResourceRoot_String, "oculus");

    std::string l_deviceType("oculus/");
    l_deviceType.append(m_serialNumber);
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RegisteredDeviceType_String, l_deviceType.c_str());

    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_InputProfilePath_String, "{oculus}/input/touch_profile.json");
    vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_SupportedButtons_Uint64, 30064771207U);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_Joystick);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_Axis1Type_Int32, vr::k_eControllerAxis_Trigger);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_Axis2Type_Int32, vr::k_eControllerAxis_Trigger);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, (m_hand == CH_Left) ? vr::TrackedControllerRole_LeftHand : vr::TrackedControllerRole_RightHand);
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ControllerType_String, "oculus_touch");
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerHandSelectionPriority_Int32, 0);

    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceOff_String, (m_hand == CH_Left) ? "{oculus}/icons/cv1_left_controller_off.png" : "{oculus}/icons/cv1_right_controller_off.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, (m_hand == CH_Left) ? "{oculus}/icons/cv1_left_controller_searching.gif" : "{oculus}/icons/cv1_right_controller_searching.gif");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, (m_hand == CH_Left) ? "{oculus}/icons/cv1_left_controller_alert_searching.gif" : "{oculus}/icons/cv1_right_controller_alert_searching.gif");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReady_String, (m_hand == CH_Left) ? "{oculus}/icons/cv1_left_controller_ready.png" : "{oculus}/icons/cv1_right_controller_ready.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, (m_hand == CH_Left) ? "{oculus}/icons/cv1_left_controller_ready_alert.png" : "{oculus}/icons/cv1_right_controller_ready_alert.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, (m_hand == CH_Left) ? "{oculus}/icons/cv1_left_controller_error.png" : "{oculus}/icons/cv1_right_controller_error.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, (m_hand == CH_Left) ? "{oculus}/icons/cv1_left_controller_standby.png" : "{oculus}/icons/cv1_right_controller_standby.png");

    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_HasDisplayComponent_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_HasCameraComponent_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_HasDriverDirectModeComponent_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_HasVirtualDisplayComponent_Bool, false);

    // Add buttons
    if(m_buttons.empty())
    {
        for(size_t i = 0U; i < TB_Count; i++) m_buttons.push_back(new CControllerButton());
    }

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/joystick/click", &m_buttons[TB_JoystickClick]->GetHandleRef());
    m_buttons[TB_JoystickClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/joystick/touch", &m_buttons[TB_JoystickTouch]->GetHandleRef());
    m_buttons[TB_JoystickTouch]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/joystick/x", &m_buttons[TB_JoystickX]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
    m_buttons[TB_JoystickX]->SetInputType(CControllerButton::IT_Float);

    vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/joystick/y", &m_buttons[TB_JoystickY]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
    m_buttons[TB_JoystickY]->SetInputType(CControllerButton::IT_Float);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/trigger/touch", &m_buttons[TB_TriggerTouch]->GetHandleRef());
    m_buttons[TB_TriggerTouch]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/trigger/value", &m_buttons[TB_TriggerValue]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
    m_buttons[TB_TriggerValue]->SetInputType(CControllerButton::IT_Float);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/grip/touch", &m_buttons[TB_GripTouch]->GetHandleRef());
    m_buttons[TB_GripTouch]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/grip/value", &m_buttons[TB_GripValue]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
    m_buttons[TB_GripValue]->SetInputType(CControllerButton::IT_Float);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/a/click", &m_buttons[TB_AClick]->GetHandleRef());
    m_buttons[TB_AClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/a/touch", &m_buttons[TB_ATouch]->GetHandleRef());
    m_buttons[TB_ATouch]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/b/click", &m_buttons[TB_BClick]->GetHandleRef());
    m_buttons[TB_BClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/b/touch", &m_buttons[TB_BTouch]->GetHandleRef());
    m_buttons[TB_BTouch]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/x/click", &m_buttons[TB_XClick]->GetHandleRef());
    m_buttons[TB_XClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/x/touch", &m_buttons[TB_XTouch]->GetHandleRef());
    m_buttons[TB_XTouch]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/y/click", &m_buttons[TB_YClick]->GetHandleRef());
    m_buttons[TB_YClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/y/touch", &m_buttons[TB_YTouch]->GetHandleRef());
    m_buttons[TB_YTouch]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/system/click", &m_buttons[TB_SystemClick]->GetHandleRef());
    m_buttons[TB_SystemClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/system/touch", &m_buttons[TB_SystemTouch]->GetHandleRef());
    m_buttons[TB_SystemTouch]->SetInputType(CControllerButton::IT_Boolean);
}

void CLeapControllerOculus::UpdateGestures(const LEAP_HAND *f_hand, const LEAP_HAND *f_oppHand)
{
    if(f_hand)
    {
        std::vector<float> l_gestures;
        CGestureMatcher::GetGestures(f_hand, l_gestures, f_oppHand);

        m_buttons[TB_TriggerValue]->SetValue(l_gestures[CGestureMatcher::HG_Trigger]);
        m_buttons[TB_TriggerTouch]->SetState(l_gestures[CGestureMatcher::HG_Trigger] >= 0.25f);

        m_buttons[TB_GripValue]->SetValue(l_gestures[CGestureMatcher::HG_Grab]);
        m_buttons[TB_GripTouch]->SetState(l_gestures[CGestureMatcher::HG_Grab] >= 0.25f);

        m_buttons[TB_JoystickTouch]->SetValue(l_gestures[CGestureMatcher::HG_ThumbPress] >= 0.5f);
        m_buttons[TB_JoystickClick]->SetValue(l_gestures[CGestureMatcher::HG_ThumbPress] >= 0.85f);
        if(l_gestures[CGestureMatcher::HG_ThumbPress] >= 0.5f)
        {
            m_buttons[TB_JoystickX]->SetValue(l_gestures[CGestureMatcher::HG_PalmPointX]);
            m_buttons[TB_JoystickY]->SetValue(l_gestures[CGestureMatcher::HG_PalmPointY]);
        }
        else
        {
            m_buttons[TB_JoystickX]->SetValue(0.f);
            m_buttons[TB_JoystickY]->SetValue(0.f);
        }

        m_buttons[TB_SystemTouch]->SetState(l_gestures[CGestureMatcher::HG_OpisthenarTouch] >= 0.5f);
        m_buttons[TB_SystemClick]->SetState(l_gestures[CGestureMatcher::HG_OpisthenarTouch] >= 0.75f);

        m_buttons[(m_hand == CH_Left) ? TB_YTouch : TB_BTouch]->SetState(l_gestures[CGestureMatcher::HG_PalmTouch] >= 0.5f);
        m_buttons[(m_hand == CH_Left) ? TB_YClick : TB_BClick]->SetState(l_gestures[CGestureMatcher::HG_PalmTouch] >= 0.75f);

        m_buttons[(m_hand == CH_Left) ? TB_XTouch : TB_ATouch]->SetState(l_gestures[CGestureMatcher::HG_MiddleCrossTouch] >= 0.5f);
        m_buttons[(m_hand == CH_Left) ? TB_XClick : TB_AClick]->SetState(l_gestures[CGestureMatcher::HG_MiddleCrossTouch] >= 0.75f);
    }
}
