#pragma once

#include "Devices/CLeapController/CLeapController.h"

class CLeapControllerVive final : public CLeapController
{
    CLeapControllerVive(const CLeapControllerVive &that) = delete;
    CLeapControllerVive& operator=(const CLeapControllerVive &that) = delete;

    // CLeapController
    void ActivateInternal() override;
    void UpdateGestures(const LEAP_HAND *f_hand, const LEAP_HAND *f_oppHand) override;
public:
    explicit CLeapControllerVive(unsigned char f_hand);
    ~CLeapControllerVive();
};
