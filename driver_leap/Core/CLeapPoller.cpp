#include "stdafx.h"

#include "Core/CLeapPoller.h"

CLeapPoller::CLeapPoller()
{
    m_active = false;
    m_thread = nullptr;
    m_connection = nullptr;
    m_clockSynchronizer = nullptr;
    m_connected = false;
    m_allocator.allocate = CLeapPoller::AllocateMemory;
    m_allocator.deallocate = CLeapPoller::DeallocateMemory;
    m_allocator.state = nullptr;
    m_lastFrame = nullptr;
    m_newFrame = nullptr;
    m_device = nullptr;
}

CLeapPoller::~CLeapPoller()
{
    delete m_lastFrame;
}

bool CLeapPoller::Initialize()
{
    if(!m_active)
    {
        if(LeapCreateConnection(nullptr, &m_connection) == eLeapRS_Success)
        {
            if(LeapOpenConnection(m_connection) == eLeapRS_Success)
            {
                LeapSetAllocator(m_connection, &m_allocator);
                LeapCreateClockRebaser(&m_clockSynchronizer);
                m_lastFrame = new LEAP_TRACKING_EVENT();
                m_newFrame = new LEAP_TRACKING_EVENT();
                m_active = true;
                m_thread = new std::thread(&CLeapPoller::ThreadUpdate, this);
            }
            else
            {
                LeapDestroyConnection(m_connection);
                m_connection = nullptr;
            }
        }
        else m_connection = nullptr;
    }

    return m_active;
}

void CLeapPoller::Terminate()
{
    if(m_active)
    {
        m_active = false;
        m_thread->join();
        m_thread = nullptr;

        if(m_device) LeapCloseDevice(m_device);
        LeapDestroyClockRebaser(m_clockSynchronizer);
        LeapCloseConnection(m_connection);
        LeapDestroyConnection(m_connection);

        m_connection = nullptr;
        m_clockSynchronizer = nullptr;
        m_interpolatedFrameBuffer.clear();
        m_device = nullptr;
        
        delete m_lastFrame;
        m_lastFrame = nullptr;
        delete m_newFrame;
        m_newFrame = nullptr;
    }
}

bool CLeapPoller::IsConnected() const
{
    return m_connected;
}

const LEAP_TRACKING_EVENT* CLeapPoller::GetInterpolatedFrame()
{
    LEAP_TRACKING_EVENT *l_result = nullptr;
    if(m_active)
    {
        if(!m_interpolatedFrameBuffer.empty()) l_result = reinterpret_cast<LEAP_TRACKING_EVENT*>(m_interpolatedFrameBuffer.data());
    }
    return l_result;
}

const LEAP_TRACKING_EVENT* CLeapPoller::GetFrame()
{
    LEAP_TRACKING_EVENT *l_result = nullptr;
    if(m_active) l_result = m_lastFrame;
    return l_result;
}

void CLeapPoller::SetPolicy(uint64_t f_set, uint64_t f_clear)
{
    if(m_active) LeapSetPolicyFlags(m_connection, f_set, f_clear);
}

void CLeapPoller::SetPaused(bool f_state)
{
    if(m_active) LeapSetPause(m_connection, f_state);
}

void CLeapPoller::Update()
{
    if(m_active)
    {
        LeapUpdateRebase(m_clockSynchronizer, std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), LeapGetNow());

        if(m_frameLock.try_lock())
        {
            std::memcpy(m_lastFrame, m_newFrame, sizeof(LEAP_TRACKING_EVENT));
            m_frameLock.unlock();
        }

        LEAP_CONNECTION_INFO l_info;
        if(LeapGetConnectionInfo(m_connection, &l_info) == eLeapRS_Success) m_connected = (l_info.status == eLeapConnectionStatus_Connected);
        else m_connected = false;
    }
}

void CLeapPoller::UpdateInterpolation()
{
    if(m_active)
    {
        int64_t l_targetFrameTime = 0;
        uint64_t l_targetFrameSize = 0U;
        LeapRebaseClock(m_clockSynchronizer, std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), &l_targetFrameTime);

        if(LeapGetFrameSize(m_connection, l_targetFrameTime, &l_targetFrameSize) == eLeapRS_Success)
        {
            // Weird SDK requires weird solutions
            m_interpolatedFrameBuffer.resize(static_cast<size_t>(l_targetFrameSize));
            LeapInterpolateFrame(m_connection, l_targetFrameTime, reinterpret_cast<LEAP_TRACKING_EVENT*>(m_interpolatedFrameBuffer.data()), l_targetFrameSize);
        }
    }
}

void CLeapPoller::ThreadUpdate()
{
    const std::chrono::milliseconds l_threadDelay(1U);
    while(m_active)
    {
        // Poll events
        LEAP_CONNECTION_MESSAGE l_message;
        while(LeapPollConnection(m_connection, 0U, &l_message) == eLeapRS_Success)
        {
            if(l_message.type == eLeapEventType_None) break;
            switch(l_message.type)
            {
                case eLeapEventType_Device:
                {
                    if(!m_device)
                    {
                        if(LeapOpenDevice(l_message.device_event->device, &m_device) != eLeapRS_Success) m_device = nullptr;
                    }
                } break;
                case eLeapEventType_Tracking:
                {
                    m_frameLock.lock();
                    std::memcpy(m_newFrame, l_message.tracking_event, sizeof(LEAP_TRACKING_EVENT));
                    m_frameLock.unlock();
                } break;
            }
        }

        std::this_thread::sleep_for(l_threadDelay);
    }
}

void* CLeapPoller::AllocateMemory(uint32_t size, eLeapAllocatorType typeHint, void *state)
{
    return new uint8_t[size];
}

void CLeapPoller::DeallocateMemory(void *ptr, void *state)
{
    delete[]reinterpret_cast<uint8_t*>(ptr);
}
