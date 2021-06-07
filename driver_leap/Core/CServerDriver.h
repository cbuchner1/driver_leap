#pragma once

class CLeapPoller;
class CLeapController;
class CLeapStation;

class CServerDriver final : public vr::IServerTrackedDeviceProvider
{
    enum LeapControllerHand : size_t
    {
        LCH_Left = 0U,
        LCH_Right = 1U,

        LCH_Count
    };

    static const char* const ms_interfaces[];

    bool m_connectionState;
    CLeapPoller *m_leapPoller;
    CLeapController *m_controllers[LCH_Count];
    CLeapStation *m_leapStation;

    CServerDriver(const CServerDriver &that) = delete;
    CServerDriver& operator=(const CServerDriver &that) = delete;

    void TryToPause();

    // vr::IServerTrackedDeviceProvider
    vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext);
    void Cleanup();
    const char* const* GetInterfaceVersions();
    void RunFrame();
    bool ShouldBlockStandbyMode();
    void EnterStandby();
    void LeaveStandby();
public:
    CServerDriver();
    ~CServerDriver();

    void ProcessExternalMessage(const char *f_message);
};
