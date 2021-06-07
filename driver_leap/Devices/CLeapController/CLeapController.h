#pragma once

class CControllerButton;

class CLeapController : public vr::ITrackedDeviceServerDriver
{
    static double ms_headPosition[3U];
    static vr::HmdQuaternion_t ms_headRotation;
    
    vr::DriverPose_t m_pose;

    CLeapController(const CLeapController &that) = delete;
    CLeapController& operator=(const CLeapController &that) = delete;

    void ResetControls();
    void UpdateInput();
    void UpdateTransformation(const LEAP_HAND *f_hand);

    // vr::ITrackedDeviceServerDriver
    vr::EVRInitError Activate(uint32_t unObjectId);
    void Deactivate();
    void EnterStandby();
    void* GetComponent(const char* pchComponentNameAndVersion);
    void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);
    vr::DriverPose_t GetPose();
public:
    enum ControllerHand : unsigned char
    {
        CH_Left = 0U,
        CH_Right,

        CH_Count
    };
    enum ControllerType : unsigned char
    {
        CT_ViveWand = 0U,
        CT_IndexKnuckle,
        CT_OculusTouch,

        CT_Count,
        CT_Invalid = 0xFFU
    };

    CLeapController();
    virtual ~CLeapController();

    const std::string& GetSerialNumber() const;

    bool IsEnabled() const;
    void SetEnabled(bool f_state);

    void RunFrame(const LEAP_HAND *f_hand, const LEAP_HAND *f_oppHand);

    static void UpdateHMDCoordinates();
protected:
    uint32_t m_trackedDevice;
    vr::PropertyContainerHandle_t m_propertyContainer;
    vr::VRInputComponentHandle_t m_haptic;

    std::string m_serialNumber;
    unsigned char m_hand;
    unsigned char m_type;
    std::vector<CControllerButton*> m_buttons;

    virtual void ActivateInternal();
    virtual void UpdateGestures(const LEAP_HAND *f_hand, const LEAP_HAND *f_oppHand);
    virtual void UpdateInputInternal();
};
