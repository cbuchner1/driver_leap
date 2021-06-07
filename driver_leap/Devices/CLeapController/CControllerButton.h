#pragma once

class CControllerButton final
{
    vr::VRInputComponentHandle_t m_handle;
    unsigned char m_inputType;
    float m_value;
    bool m_state;
    bool m_updated;

    CControllerButton(const CControllerButton &that) = delete;
    CControllerButton& operator=(const CControllerButton &that) = delete;
public:
    enum InputType : unsigned char
    {
        IT_None = 0U,
        IT_Boolean,
        IT_Float
    };

    CControllerButton();
    ~CControllerButton();

    vr::VRInputComponentHandle_t GetHandle() const;
    vr::VRInputComponentHandle_t& GetHandleRef();

    void SetInputType(unsigned char f_type);
    unsigned char GetInputType() const;

    void SetState(bool f_state);
    bool GetState() const;

    void SetValue(float f_value);
    float GetValue() const;

    bool IsUpdated() const;
    void ResetUpdate();
};
