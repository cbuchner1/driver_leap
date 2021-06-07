#include "stdafx.h"

#include "Devices/CLeapController/CControllerButton.h"

CControllerButton::CControllerButton()
{
    m_handle = vr::k_ulInvalidInputComponentHandle;
    m_inputType = IT_Boolean;
    m_state = false;
    m_value = 0.f;
    m_updated = false;
}

CControllerButton::~CControllerButton()
{
}

vr::VRInputComponentHandle_t CControllerButton::GetHandle() const
{
    return m_handle;
}

vr::VRInputComponentHandle_t& CControllerButton::GetHandleRef()
{
    return m_handle;
}

void CControllerButton::SetInputType(unsigned char f_type)
{
    m_inputType = f_type;
}

unsigned char CControllerButton::GetInputType() const
{
    return m_inputType;
}

void CControllerButton::SetState(bool f_state)
{
    if(m_inputType == IT_Boolean)
    {
        if(m_state != f_state)
        {
            m_state = f_state;
            m_updated = true;
        }
    }
}

bool CControllerButton::GetState() const
{
    return m_state;
}

void CControllerButton::SetValue(float f_value)
{
    if(m_inputType == IT_Float)
    {
        if(m_value != f_value)
        {
            m_value = f_value;
            m_updated = true;
        }
    }
}

float CControllerButton::GetValue() const
{
    return m_value;
}

bool CControllerButton::IsUpdated() const
{
    return m_updated;
}

void CControllerButton::ResetUpdate()
{
    m_updated = false;
}
