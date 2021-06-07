#include "stdafx.h"

#include "CLeapMonitor.h"

const std::chrono::milliseconds g_threadDelay(11U);

CLeapMonitor::CLeapMonitor()
{
    m_active = false;
    m_vrSystem = nullptr;
    m_notificationID = 0U;
    m_overlayHandle = vr::k_ulOverlayHandleInvalid;

    m_leapActive = false;
    m_leapConnection = nullptr;
    m_leapAllocator.allocate = CLeapMonitor::AllocateMemory;
    m_leapAllocator.deallocate = CLeapMonitor::DeallocateMemory;
    m_leapAllocator.state = nullptr;
    m_leapDevice = nullptr;

    m_relayDevice = vr::k_unTrackedDeviceIndexInvalid;
    m_leftHotkey = false;
    m_rightHotkey = false;
    m_reloadHotkey = false;
}
CLeapMonitor::~CLeapMonitor()
{
}

bool CLeapMonitor::Initialize()
{
    if(!m_active)
    {
        vr::EVRInitError eVRInitError;
        m_vrSystem = vr::VR_Init(&eVRInitError, vr::VRApplication_Other);
        if(eVRInitError == vr::VRInitError_None)
        {
            vr::VROverlay()->CreateOverlay("leap_monitor_overlay", "Leap Motion Monitor", &m_overlayHandle);

            for(uint32_t i = 0U; i < vr::k_unMaxTrackedDeviceCount; i++)
            {
                auto test = m_vrSystem->GetUint64TrackedDeviceProperty(i, vr::Prop_VendorSpecific_Reserved_Start);
                if(test == 0x4C4D6F74696F6E)
                {
                    m_relayDevice = i;
                    break;
                }
            }

            if(LeapCreateConnection(nullptr, &m_leapConnection) == eLeapRS_Success)
            {
                if(LeapOpenConnection(m_leapConnection) == eLeapRS_Success)
                {
                    LeapSetAllocator(m_leapConnection, &m_leapAllocator);
                    m_leapActive = true;
                }
                else
                {
                    LeapDestroyConnection(m_leapConnection);
                    m_leapConnection = nullptr;
                }
            }
            else m_leapConnection = nullptr;

            m_active = true;
        }
    }
    return m_active;
}

void CLeapMonitor::Terminate()
{
    if(m_active)
    {
        m_active = false;

        if(m_leapActive)
        {
            if(m_leapDevice) LeapCloseDevice(m_leapDevice);
            LeapCloseConnection(m_leapConnection);
            LeapDestroyConnection(m_leapConnection);
            m_leapConnection = nullptr;
            m_leapDevice = nullptr;
            m_leapActive = false;
        }

        if(m_notificationID) vr::VRNotifications()->RemoveNotification(m_notificationID);
        vr::VROverlay()->DestroyOverlay(m_overlayHandle);
        m_overlayHandle = vr::k_ulOverlayHandleInvalid;
        m_notificationID = 0U;

        vr::VR_Shutdown();
        m_vrSystem = nullptr;
    }

    m_active = false;
}

bool CLeapMonitor::DoPulse()
{
    while(m_vrSystem->PollNextEvent(&m_event, sizeof(vr::VREvent_t)))
    {
        switch(m_event.eventType)
        {
            case vr::VREvent_Quit:
                m_active = false;
                break;
            case vr::VREvent_TrackedDeviceActivated:
            {
                if(m_vrSystem->GetUint64TrackedDeviceProperty(m_event.trackedDeviceIndex, vr::Prop_VendorSpecific_Reserved_Start) == 0x4C4D6F74696F6E) m_relayDevice = m_event.trackedDeviceIndex;
            } break;
            case vr::VREvent_TrackedDeviceDeactivated:
            {
                if(m_relayDevice == m_event.trackedDeviceIndex) m_relayDevice = vr::k_unTrackedDeviceIndexInvalid;
            } break;
        }
    }

    // Process hotkeys if NumLock is active
    if((GetKeyState(VK_NUMLOCK) & 0xFFFF) != 0)
    {
        if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
            bool l_hotkeyState = (GetAsyncKeyState(0x4F) & 0x8000); // Ctrl+O
            if(m_leftHotkey != l_hotkeyState)
            {
                m_leftHotkey = l_hotkeyState;
                if(m_leftHotkey)
                {
                    SendCommand("setting left_hand");
                    SendNotification("Left hand is toggled");
                }
            }

            l_hotkeyState = (GetAsyncKeyState(0x50) & 0x8000); // Ctrl+P
            if(m_rightHotkey != l_hotkeyState)
            {
                m_rightHotkey = l_hotkeyState;
                if(m_rightHotkey)
                {
                    SendCommand("setting right_hand");
                    SendNotification("Right hand is toggled");
                }
            }

            l_hotkeyState = (GetAsyncKeyState(0xDC) & 0x8000); // Ctrl+§
            if(m_reloadHotkey != l_hotkeyState)
            {
                m_reloadHotkey = l_hotkeyState;
                if(m_reloadHotkey)
                {
                    SendCommand("setting reload_config");
                    SendNotification("Configuration is reloaded");
                }
            }
        }
    }

    if(m_leapActive)
    {
        LEAP_CONNECTION_MESSAGE l_message;
        while(LeapPollConnection(m_leapConnection, 0U, &l_message) == eLeapRS_Success)
        {
            if(l_message.type == eLeapEventType_None) break;
            switch(l_message.type)
            {
                case eLeapEventType_Connection:
                    SendNotification("Connected to Leap Service");
                    break;
                case eLeapEventType_ConnectionLost:
                    SendNotification("Connection to Leap Service is lost");
                    break;
                case eLeapEventType_Device:
                {
                    if(!m_leapDevice)
                    {
                        if(LeapOpenDevice(l_message.device_event->device, &m_leapDevice) != eLeapRS_Success) m_leapDevice = nullptr;
                    }
                    SendNotification("New device is connected");
                } break;
                case eLeapEventType_DeviceFailure:
                    SendNotification("Device failure");
                    break;
                case eLeapEventType_Policy:
                    SendNotification("New policies are applied/cleared");
                    break;
                case eLeapEventType_DeviceLost:
                    SendNotification("Device is lost");
                    break;
                case eLeapEventType_DeviceStatusChange:
                    SendNotification("Device status is changed");
                    break;
            }
        }
    }

    std::this_thread::sleep_for(g_threadDelay);
    return m_active;
}

void CLeapMonitor::SendCommand(const char *f_char)
{
    if(m_relayDevice != vr::k_unTrackedDeviceIndexInvalid)
    {
        char l_response[32U];
        vr::VRDebug()->DriverDebugRequest(m_relayDevice, f_char, l_response, 32U);
    }
}

void CLeapMonitor::SendNotification(const char *f_text)
{
    if(m_active)
    {
        if(std::strlen(f_text) != 0U)
        {
            if(m_notificationID) vr::VRNotifications()->RemoveNotification(m_notificationID);
            vr::VRNotifications()->CreateNotification(m_overlayHandle, 500U, vr::EVRNotificationType_Transient, f_text, vr::EVRNotificationStyle_None, nullptr, &m_notificationID);
        }
    }
}

void* CLeapMonitor::AllocateMemory(uint32_t size, eLeapAllocatorType typeHint, void *state)
{
    return new uint8_t[size];
}

void CLeapMonitor::DeallocateMemory(void *ptr, void *state)
{
    delete[]reinterpret_cast<uint8_t*>(ptr);
}
