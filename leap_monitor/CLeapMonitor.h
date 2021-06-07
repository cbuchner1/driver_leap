#pragma once

class CLeapMonitor final
{
    enum GameProfile : unsigned char
    {
        GP_Default = 0U,
        GP_VRChat
    };

    bool m_active;
    vr::IVRSystem *m_vrSystem;
    vr::VRNotificationId m_notificationID;
    vr::VROverlayHandle_t m_overlayHandle;
    vr::VREvent_t m_event;

    bool m_leapActive;
    LEAP_CONNECTION m_leapConnection;
    LEAP_ALLOCATOR m_leapAllocator;
    LEAP_DEVICE m_leapDevice;

    uint32_t m_relayDevice;
    bool m_leftHotkey;
    bool m_rightHotkey;
    bool m_reloadHotkey;

    CLeapMonitor(const CLeapMonitor &that) = delete;
    CLeapMonitor& operator=(const CLeapMonitor &that) = delete;

    void SendCommand(const char *f_cmd);

    static void* AllocateMemory(uint32_t size, eLeapAllocatorType typeHint, void *state);
    static void DeallocateMemory(void *ptr, void *state);
public:
    CLeapMonitor();
    ~CLeapMonitor();

    bool Initialize();
    void Terminate();
    bool DoPulse();

    void SendNotification(const char *f_text);
};
