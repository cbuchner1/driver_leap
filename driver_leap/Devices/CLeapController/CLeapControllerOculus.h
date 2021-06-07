#pragma once
#include "Devices/CLeapController/CLeapController.h"

class CLeapControllerOculus final : public CLeapController
{
    CLeapControllerOculus(const CLeapControllerOculus &that) = delete;
    CLeapControllerOculus& operator=(const CLeapControllerOculus &that) = delete;

    // CLeapController
    void ActivateInternal() override;
    void UpdateGestures(const LEAP_HAND *f_hand, const LEAP_HAND *f_oppHand) override;
public:
    explicit CLeapControllerOculus(unsigned char f_hand);
    ~CLeapControllerOculus();
};

