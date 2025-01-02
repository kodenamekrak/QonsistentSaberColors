#pragma once

#include "UnityEngine/Color.hpp"

namespace QonsistentSaberColors {

    UnityEngine::Color get_LeftColor();
    UnityEngine::Color get_RightColor();
    
    void UpdatePointers();

    void UpdateControllerColors(const UnityEngine::Color& leftColor = get_LeftColor(), const UnityEngine::Color& rightColor = get_RightColor());

    void UpdateLaserColor(const UnityEngine::Color& leftColor = get_LeftColor(), const UnityEngine::Color& rightColor = get_RightColor());
}