#pragma once

class CLeapPoller
{
    std::atomic<bool> m_active;
    std::mutex m_frameLock;
    std::thread *m_thread;

    LEAP_CONNECTION m_connection;
    LEAP_CLOCK_REBASER m_clockSynchronizer;
    LEAP_ALLOCATOR m_allocator;
    LEAP_TRACKING_EVENT *m_lastFrame;
    LEAP_TRACKING_EVENT *m_newFrame;
    std::vector<uint8_t> m_interpolatedFrameBuffer;
    LEAP_DEVICE m_device;
    bool m_connected;

    void ThreadUpdate();

    static void* AllocateMemory(uint32_t size, eLeapAllocatorType typeHint, void *state);
    static void DeallocateMemory(void *ptr, void *state);
public:
    CLeapPoller();
    ~CLeapPoller();

    bool Initialize();
    void Terminate();

    bool IsConnected() const;
    const LEAP_TRACKING_EVENT* GetInterpolatedFrame();
    const LEAP_TRACKING_EVENT* GetFrame();

    void SetPolicy(uint64_t f_set, uint64_t f_clear = 0U);
    void SetPaused(bool f_state);

    void Update();
    void UpdateInterpolation();
};


