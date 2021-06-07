#include "stdafx.h"

#include "Devices/CLeapController/CLeapControllerVive.h"

#include "Devices/CLeapController/CControllerButton.h"
#include "Utils/CGestureMatcher.h"

enum ViveButton : size_t
{
    VB_SystemClick = 0U,
    VB_TriggerClick,
    VB_TriggerValue,
    VB_TrackpadX,
    VB_TrackpadY,
    VB_TrackpadTouch,
    VB_TrackpadClick,
    VB_GripClick,
    VB_MenuClick,

    VB_Count
};

CLeapControllerVive::CLeapControllerVive(unsigned char f_hand)
{
    m_hand = (f_hand % CH_Count);
    m_type = CT_ViveWand;
    m_serialNumber.assign("LHR-F94B3BD");
    m_serialNumber.append(std::to_string(static_cast<size_t>(f_hand)));
}

CLeapControllerVive::~CLeapControllerVive()
{
}

void CLeapControllerVive::ActivateInternal()
{
    // Properties
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_TrackingSystemName_String, "lighthouse");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_SerialNumber_String, m_serialNumber.c_str());
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_WillDriftInYaw_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_DeviceIsWireless_Bool, true);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_DeviceIsCharging_Bool, false);
    vr::VRProperties()->SetFloatProperty(m_propertyContainer, vr::Prop_DeviceBatteryPercentage_Float, 1.f); // Always charged

    vr::HmdMatrix34_t l_matrix = { -1.f, 0.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f };
    vr::VRProperties()->SetProperty(m_propertyContainer, vr::Prop_StatusDisplayTransform_Matrix34, &l_matrix, sizeof(vr::HmdMatrix34_t), vr::k_unHmdMatrix34PropertyTag);

    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_Firmware_UpdateAvailable_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_Firmware_ManualUpdate_Bool, false);
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_Firmware_ManualUpdateURL_String, "https://developer.valvesoftware.com/wiki/SteamVR/HowTo_Update_Firmware");
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_DeviceProvidesBatteryStatus_Bool, true);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_DeviceCanPowerOff_Bool, true);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_Controller);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_Firmware_ForceUpdateRequired_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_Identifiable_Bool, true);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_Firmware_RemindUpdate_Bool, false);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_TrackPad);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_Axis1Type_Int32, vr::k_eControllerAxis_Trigger);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, (m_hand == CH_Left) ? vr::TrackedControllerRole_LeftHand : vr::TrackedControllerRole_RightHand);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_HasDisplayComponent_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_HasCameraComponent_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_HasDriverDirectModeComponent_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyContainer, vr::Prop_HasVirtualDisplayComponent_Bool, false);
    vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerHandSelectionPriority_Int32, 0);
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ModelNumber_String, "Vive. Controller MV");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_SerialNumber_String, m_serialNumber.c_str()); // Changed
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, "vr_controller_vive_1_5");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ManufacturerName_String, "HTC");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_TrackingFirmwareVersion_String, "1533720215 htcvrsoftware@firmware-win32 2018-08-08 FPGA 262(1.6/0/0) BL 0 VRC 1533720214 Radio 1532585738");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_HardwareRevision_String, "product 129 rev 1.5.0 lot 2000/0/0 0");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ConnectedWirelessDongle_String, "1E8092840E"); // Changed
    vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_HardwareRevision_Uint64, 2164327680U);
    vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_FirmwareVersion_Uint64, 1533720215U);
    vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_FPGAVersion_Uint64, 262U);
    vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_VRCVersion_Uint64, 1533720214U);
    vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_RadioVersion_Uint64, 1532585738U);
    vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_DongleVersion_Uint64, 1461100729U);
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ResourceRoot_String, "htc");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RegisteredDeviceType_String, (m_hand == CH_Left) ? "htc/vive_controllerLHR-F94B3BD8" : "htc/vive_controllerLHR-F94B3BD9"); // Changed
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_InputProfilePath_String, "{htc}/input/vive_controller_profile.json");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceOff_String, "{htc}/icons/controller_status_off.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, "{htc}/icons/controller_status_searching.gif");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{htc}/icons/controller_status_searching_alert.gif");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReady_String, "{htc}/icons/controller_status_ready.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{htc}/icons/controller_status_ready_alert.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, "{htc}/icons/controller_status_error.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, "{htc}/icons/controller_status_off.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, "{htc}/icons/controller_status_ready_low.png");
    vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ControllerType_String, "vive_controller");

    // Input
    if(m_buttons.empty())
    {
        for(size_t i = 0U; i < VB_Count; i++) m_buttons.push_back(new CControllerButton());
    }

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/system/click", &m_buttons[VB_SystemClick]->GetHandleRef());
    m_buttons[VB_SystemClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/trigger/click", &m_buttons[VB_TriggerClick]->GetHandleRef());
    m_buttons[VB_TriggerClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/trigger/value", &m_buttons[VB_TriggerValue]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
    m_buttons[VB_TriggerValue]->SetInputType(CControllerButton::IT_Float);

    vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/trackpad/x", &m_buttons[VB_TrackpadX]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
    m_buttons[VB_TrackpadX]->SetInputType(CControllerButton::IT_Float);

    vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/trackpad/y", &m_buttons[VB_TrackpadY]->GetHandleRef(), vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
    m_buttons[VB_TrackpadY]->SetInputType(CControllerButton::IT_Float);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/trackpad/touch", &m_buttons[VB_TrackpadTouch]->GetHandleRef());
    m_buttons[VB_TrackpadTouch]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/trackpad/click", &m_buttons[VB_TrackpadClick]->GetHandleRef());
    m_buttons[VB_TrackpadClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/grip/click", &m_buttons[VB_GripClick]->GetHandleRef());
    m_buttons[VB_GripClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/application_menu/click", &m_buttons[VB_MenuClick]->GetHandleRef());
    m_buttons[VB_MenuClick]->SetInputType(CControllerButton::IT_Boolean);

    vr::VRDriverInput()->CreateHapticComponent(m_propertyContainer, "/output/haptic", &m_haptic);
}

void CLeapControllerVive::UpdateGestures(const LEAP_HAND *f_hand, const LEAP_HAND *f_oppHand)
{
    if(f_hand)
    {
        std::vector<float> l_gestures;
        CGestureMatcher::GetGestures(f_hand, l_gestures, f_oppHand);

        m_buttons[VB_TriggerValue]->SetValue(l_gestures[CGestureMatcher::HG_Trigger]);
        m_buttons[VB_TriggerClick]->SetState(l_gestures[CGestureMatcher::HG_Trigger] >= 0.75f);

        m_buttons[VB_GripClick]->SetValue(l_gestures[CGestureMatcher::HG_Grab] >= 0.75f);

        m_buttons[VB_TrackpadTouch]->SetState(l_gestures[CGestureMatcher::HG_ThumbPress] >= 0.5f);
        m_buttons[VB_TrackpadClick]->SetState(l_gestures[CGestureMatcher::HG_ThumbPress] >= 0.85f);
        if(l_gestures[CGestureMatcher::HG_ThumbPress] >= 0.5f)
        {
            m_buttons[VB_TrackpadX]->SetValue(l_gestures[CGestureMatcher::HG_PalmPointX]);
            m_buttons[VB_TrackpadY]->SetValue(l_gestures[CGestureMatcher::HG_PalmPointY]);
        }
        else
        {
            m_buttons[VB_TrackpadX]->SetValue(0.f);
            m_buttons[VB_TrackpadY]->SetValue(0.f);
        }

        m_buttons[VB_SystemClick]->SetState(l_gestures[CGestureMatcher::HG_OpisthenarTouch] >= 0.75f);
        m_buttons[VB_MenuClick]->SetState(l_gestures[CGestureMatcher::HG_PalmTouch] >= 0.75f);
    }
}
