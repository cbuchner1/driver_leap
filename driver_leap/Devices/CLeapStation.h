#pragma once

class CServerDriver;

class CLeapStation final : public vr::ITrackedDeviceServerDriver
{
    vr::DriverPose_t m_pose;
    vr::PropertyContainerHandle_t m_propertyHandle;
    uint32_t m_trackedDevice;

    CServerDriver *m_serverDriver;
    std::string m_serialNumber;

    CLeapStation(const CLeapStation &that) = delete;
    CLeapStation& operator=(const CLeapStation &that) = delete;

    // vr::ITrackedDeviceServerDriver
    vr::EVRInitError Activate(uint32_t unObjectId);
    void Deactivate();
    void EnterStandby();
    void* GetComponent(const char* pchComponentNameAndVersion);
    void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);
    vr::DriverPose_t GetPose();
public:
    enum TrackingState : unsigned char
    {
        TS_Connected = 0U,
        TS_Search
    };

    explicit CLeapStation(CServerDriver *f_server);
    ~CLeapStation();

    const std::string& GetSerialNumber() const;

    void SetTrackingState(TrackingState f_state);

    void RunFrame();
};
