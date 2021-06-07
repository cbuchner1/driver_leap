#include "stdafx.h"

#include "Core/CServerDriver.h"
#include "Core/CLeapPoller.h"
#include "Devices/CLeapController/CLeapControllerVive.h"
#include "Devices/CLeapController/CLeapControllerIndex.h"
#include "Devices/CLeapController/CLeapControllerOculus.h"
#include "Devices/CLeapStation.h"

#include "Core/CDriverConfig.h"
#include "Utils/Utils.h"

extern char g_modulePath[];

const std::vector<std::string> g_debugRequests
{
    "setting"
};
enum DebugRequest : size_t
{
    DR_Setting = 0U
};

const std::vector<std::string> g_settingCommands
{
    "left_hand", "right_hand", "reload_config"
};
enum SettingCommand : size_t
{
    SC_LeftHand = 0U,
    SC_RightHand,
    SC_ReloadConfig
};

const char* const CServerDriver::ms_interfaces[]
{
    vr::ITrackedDeviceServerDriver_Version,
        vr::IServerTrackedDeviceProvider_Version,
        nullptr
};

CServerDriver::CServerDriver()
{
    m_leapPoller = nullptr;
    m_connectionState = false;
    for(size_t i = 0U; i < LCH_Count; i++) m_controllers[i] = nullptr;
    m_leapStation = nullptr;
}

CServerDriver::~CServerDriver()
{
}

// vr::IServerTrackedDeviceProvider
vr::EVRInitError CServerDriver::Init(vr::IVRDriverContext *pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    CDriverConfig::Load();

    // Relay device for events from leap_monitor
    m_leapStation = new CLeapStation(this);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_leapStation->GetSerialNumber().c_str(), vr::TrackedDeviceClass_TrackingReference, m_leapStation);

    switch(CDriverConfig::GetEmulatedController())
    {
        case CDriverConfig::EC_Vive:
        {
            if(CDriverConfig::IsLeftHandEnabled()) m_controllers[LCH_Left] = new CLeapControllerVive(CLeapController::CH_Left);
            if(CDriverConfig::IsRightHandEnabled()) m_controllers[LCH_Right] = new CLeapControllerVive(CLeapController::CH_Right);
        } break;
        case CDriverConfig::EC_Index:
        {
            if(CDriverConfig::IsLeftHandEnabled()) m_controllers[LCH_Left] = new CLeapControllerIndex(CLeapController::CH_Left);
            if(CDriverConfig::IsRightHandEnabled()) m_controllers[LCH_Right] = new CLeapControllerIndex(CLeapController::CH_Right);
        } break;
        case CDriverConfig::EC_Oculus:
        {
            if(CDriverConfig::IsLeftHandEnabled()) m_controllers[LCH_Left] = new CLeapControllerOculus(CLeapController::CH_Left);
            if(CDriverConfig::IsRightHandEnabled()) m_controllers[LCH_Right] = new CLeapControllerOculus(CLeapController::CH_Right);
        } break;
    }

    for(size_t i = 0U; i < LCH_Count; i++)
    {
        if(m_controllers[i]) vr::VRServerDriverHost()->TrackedDeviceAdded(m_controllers[i]->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, m_controllers[i]);
    }

    m_leapPoller = new CLeapPoller();
    if(m_leapPoller->Initialize())
    {
        m_leapPoller->SetPolicy(eLeapPolicyFlag_AllowPauseResume);
        if(CDriverConfig::GetOrientationMode() == CDriverConfig::OM_HMD) m_leapPoller->SetPolicy(eLeapPolicyFlag_OptimizeHMD);
    }
    //m_connectionState = true;

    // Start utility app that closes itself on SteamVR shutdown
    std::string l_path(g_modulePath);
    l_path.erase(l_path.begin() + l_path.rfind('\\'), l_path.end());

    std::string l_appPath(l_path);
    l_appPath.append("\\leap_monitor.exe");

    STARTUPINFOA l_infoProcess = { 0 };
    PROCESS_INFORMATION l_monitorInfo = { 0 };
    l_infoProcess.cb = sizeof(STARTUPINFOA);
    CreateProcessA(l_appPath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, l_path.c_str(), &l_infoProcess, &l_monitorInfo);

    return vr::VRInitError_None;
}

void CServerDriver::Cleanup()
{
    for(size_t i = 0U; i < LCH_Count; i++)
    {
        delete m_controllers[i];
        m_controllers[i] = nullptr;
    }
    delete m_leapStation;
    m_leapStation = nullptr;

    m_leapPoller->Terminate();
    delete m_leapPoller;
    m_leapPoller = nullptr;

    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* CServerDriver::GetInterfaceVersions()
{
    return ms_interfaces;
}

void CServerDriver::RunFrame()
{
    CLeapController::UpdateHMDCoordinates();
    m_leapPoller->Update();

    if(m_connectionState != m_leapPoller->IsConnected())
    {
        m_connectionState = m_leapPoller->IsConnected();
        m_leapStation->SetTrackingState(m_connectionState ? CLeapStation::TS_Connected : CLeapStation::TS_Search);
        for(size_t i = 0U; i < LCH_Count; i++)
        {
            if(m_controllers[i]) m_controllers[i]->SetEnabled(m_connectionState);
        }
    }

    LEAP_HAND *l_hands[LCH_Count] = { nullptr };
    if(m_connectionState)
    {
        if(CDriverConfig::IsInterpolationEnabled()) m_leapPoller->UpdateInterpolation();

        const LEAP_TRACKING_EVENT *l_frame = (CDriverConfig::IsInterpolationEnabled() ? m_leapPoller->GetInterpolatedFrame() : m_leapPoller->GetFrame());
        if(l_frame)
        {
            for(size_t i = 0U; i < l_frame->nHands; i++)
            {
                if(!l_hands[l_frame->pHands[i].type]) l_hands[l_frame->pHands[i].type] = &l_frame->pHands[i];
            }
        }
    }

    // Update devices
    for(size_t i = 0U; i < LCH_Count; i++)
    {
        if(m_controllers[i]) m_controllers[i]->RunFrame(l_hands[i],l_hands[(i+1)%LCH_Count]);
    }
    m_leapStation->RunFrame();
}

bool CServerDriver::ShouldBlockStandbyMode()
{
    return false;
}

void CServerDriver::EnterStandby()
{
}

void CServerDriver::LeaveStandby()
{
}

// CServerDriver
void CServerDriver::TryToPause()
{
    if(m_leapPoller)
    {
        bool l_pause = false;
        for(size_t i = 0U; i < LCH_Count; i++)
        {
            if(m_controllers[i]) l_pause = (l_pause || !m_controllers[i]->IsEnabled());
        }
        m_leapPoller->SetPaused(l_pause);
    }
}

void CServerDriver::ProcessExternalMessage(const char *f_message)
{
    std::stringstream l_stream(f_message);
    std::string l_event;

    l_stream >> l_event;
    if(!l_stream.fail() && !l_event.empty())
    {
        switch(ReadEnumVector(l_event, g_debugRequests))
        {
            case DR_Setting:
            {
                std::string l_settingCommand;
                l_stream >> l_settingCommand;
                if(!l_stream.fail() && !l_settingCommand.empty())
                {
                    size_t l_setting = ReadEnumVector(l_settingCommand, g_settingCommands);
                    switch(l_setting)
                    {
                        case SC_LeftHand: case SC_RightHand:
                        {
                            if(m_connectionState && m_controllers[l_setting])
                            {
                                bool l_enabled = m_controllers[l_setting]->IsEnabled();
                                m_controllers[l_setting]->SetEnabled(!l_enabled);
                                TryToPause();
                            }
                        } break;
                        case SC_ReloadConfig:
                        {
                            CDriverConfig::Load();

                            // Change orientation mode
                            if(CDriverConfig::GetOrientationMode() == CDriverConfig::OM_HMD) m_leapPoller->SetPolicy(eLeapPolicyFlag_OptimizeHMD);
                            else m_leapPoller->SetPolicy(0U, eLeapPolicyFlag_OptimizeHMD);
                        } break;
                    }
                }
            } break;
        }
    }
}
