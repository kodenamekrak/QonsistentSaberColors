#include "main.hpp"
#include "ModConfig.hpp"
#include "ColorManager.hpp"

#include "GlobalNamespace/VRController.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/ColorSchemesSettings.hpp"
#include "GlobalNamespace/SetSaberGlowColor.hpp"
#include "GlobalNamespace/SetSaberFakeGlowColor.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/ColorManager.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MeshRenderer.hpp"

#include "VRUIControls/VRInputModule.hpp"
#include "VRUIControls/VRLaserPointer.hpp"
#include "VRUIControls/VRPointer.hpp"
#include "UnityEngine/XR/XRNode.hpp"

using namespace UnityEngine;
using namespace VRUIControls;
using namespace GlobalNamespace;

UnityW<VRUIControls::VRInputModule> inputModule;
GlobalNamespace::ColorSchemesSettings* colorSchemesSettings;

UnityEngine::Color defaultLeftColor {0.784314, 0.078431, 0.078431, 1.000000}; 
UnityEngine::Color defaultRightColor{0.156863, 0.556863, 0.823529, 1.000000};
UnityEngine::Color defaultLaserColor{0.125490f, 0.752941f, 1.000000f, 0.501961f};

namespace QonsistentSaberColors {

    UnityEngine::Color get_LeftColor()
    {
        if(!colorSchemesSettings || !colorSchemesSettings->overrideDefaultColors || !getModConfig().Enabled.GetValue())
            return defaultLeftColor;

        return colorSchemesSettings->GetSelectedColorScheme()->saberAColor;
    }

    UnityEngine::Color get_RightColor()
    {
        if(!colorSchemesSettings || !colorSchemesSettings->overrideDefaultColors || !getModConfig().Enabled.GetValue())
            return defaultRightColor;

        return colorSchemesSettings->GetSelectedColorScheme()->saberBColor;
    }

    UnityEngine::Color get_LaserColor(const UnityEngine::Color& leftColor = get_LeftColor(), const UnityEngine::Color& rightColor = get_RightColor())
    {
        if(!inputModule || !getModConfig().Enabled.GetValue() || !getModConfig().ColoredLasers.GetValue())
            return defaultLaserColor;
        
        auto parent = inputModule->_vrPointer->_laserPointer->transform->parent->parent->name;
        return parent == "ControllerLeft" ? leftColor : rightColor;
    }

    void UpdatePointers()
    {
        INFO("Updating pointers");

        inputModule = UnityEngine::Resources::FindObjectsOfTypeAll<VRUIControls::VRInputModule*>().front_or_default();

        auto playerDataModel = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::PlayerDataModel*>().front_or_default();
        if(!playerDataModel) {
            Logger.debug("Failed to find instance of PlayerDataModel*");
            colorSchemesSettings = nullptr;
            return;
        }
        colorSchemesSettings = playerDataModel->playerData->colorSchemesSettings;
    }

    void SetControllerColor(GlobalNamespace::VRController* controller, UnityEngine::Color color)
    {
        for(auto glow : controller ->gameObject->GetComponentsInChildren<GlobalNamespace::SetSaberGlowColor*>())
        {
            auto scheme = glow->_colorManager->_colorScheme;
            controller->node == UnityEngine::XR::XRNode::LeftHand ? scheme->_saberAColor = color : scheme->_saberBColor = color; 
            glow->SetColors();
        }

        for(auto fakeGlow : controller->gameObject->GetComponentsInChildren<GlobalNamespace::SetSaberFakeGlowColor*>())
        {
            auto scheme2 = fakeGlow->_colorManager->_colorScheme;
            controller->node == UnityEngine::XR::XRNode::LeftHand ? scheme2->_saberAColor = color : scheme2->_saberBColor = color;
            fakeGlow->SetColors();
        }
    }

    void SetLaserColor(VRUIControls::VRLaserPointer* pointer, UnityEngine::Color color)
    {
        auto renderer = pointer->GetComponentInChildren<UnityEngine::MeshRenderer*>();
        auto matArray = renderer->GetMaterialArray();
        for(auto mat : matArray)
        {
            mat->color = color;
        }
    }

    void UpdateControllerColors(const UnityEngine::Color& leftColor, const UnityEngine::Color& rightColor)
    {
        if(!inputModule || !colorSchemesSettings)
            return;

        SetControllerColor(inputModule->_vrPointer->_leftVRController, leftColor);
        SetControllerColor(inputModule->_vrPointer->_rightVRController, rightColor);
    }

    void UpdateLaserColor(const UnityEngine::Color& leftColor, const UnityEngine::Color& rightColor)
    {
        if(!inputModule)
            return;

        UnityEngine::Color color = get_LaserColor(leftColor, rightColor);
        color.a = 0;

        SetLaserColor(inputModule->_vrPointer->_laserPointer, color);
    }
}