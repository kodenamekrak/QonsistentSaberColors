#include "main.hpp"
#include "SettingsViewController.hpp"
#include "ModConfig.hpp"
#include "ColorManager.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/ColorsOverrideSettingsPanelController.hpp"
#include "GlobalNamespace/MenuEnvironmentManager.hpp"
#include "GlobalNamespace/EnvironmentColorManager.hpp"
#include "GlobalNamespace/ColorScheme.hpp"

#include "VRUIControls/VRPointer.hpp"

#include "bsml/shared/BSML.hpp"

static modloader::ModInfo modInfo {MOD_ID, VERSION, 0};

using namespace QonsistentSaberColors;

inline void UpdateColors(const UnityEngine::Color& leftColor = get_LeftColor(), const UnityEngine::Color& rightColor = get_RightColor())
{
    if(getModConfig().Enabled.GetValue())
    {
        UpdateControllerColors(leftColor, rightColor);
        if(getModConfig().ColoredLasers.GetValue())
            UpdateLaserColor(leftColor, rightColor);
    }
}

MAKE_HOOK_MATCH(EnvironmentColorManager_InitColors, &GlobalNamespace::EnvironmentColorManager::InitColors, void, GlobalNamespace::EnvironmentColorManager* self)
{
    EnvironmentColorManager_InitColors(self);
    UpdateColors(self->_colorScheme->get_saberAColor(), self->_colorScheme->get_saberBColor());
}

MAKE_HOOK_MATCH(VRPointer_CreateLaserPointerAndLaserHit, &VRUIControls::VRPointer::RefreshLaserPointerAndLaserHit, void, VRUIControls::VRPointer* self, UnityEngine::EventSystems::PointerEventData* pointerData)
{
    VRPointer_CreateLaserPointerAndLaserHit(self, pointerData);
    if(getModConfig().Enabled.GetValue() && getModConfig().ColoredLasers.GetValue())
        UpdateLaserColor();
}

MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool a, bool b, bool c)
{
    MainMenuViewController_DidActivate(self, a, b, c);
    UpdateColors();
}

MAKE_HOOK_MATCH(ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged, &GlobalNamespace::ColorsOverrideSettingsPanelController::HandleOverrideColorsToggleValueChanged, void, GlobalNamespace::ColorsOverrideSettingsPanelController* self, bool isOn)
{
    ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged(self, isOn);
    UpdateColors();
}

MAKE_HOOK_MATCH(ColorsOverrideSettingsPanelController_HandleEditColorSchemeControllerDidChangeColorScheme, &GlobalNamespace::ColorsOverrideSettingsPanelController::HandleEditColorSchemeControllerDidChangeColorScheme, void, GlobalNamespace::ColorsOverrideSettingsPanelController* self, GlobalNamespace::ColorScheme* colorScheme)
{
    ColorsOverrideSettingsPanelController_HandleEditColorSchemeControllerDidChangeColorScheme(self, colorScheme);
    UpdateColors();
}

MAKE_HOOK_MATCH(ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx, &GlobalNamespace::ColorsOverrideSettingsPanelController::HandleDropDownDidSelectCellWithIdx, void, GlobalNamespace::ColorsOverrideSettingsPanelController* self, HMUI::DropdownWithTableView* dropDownWithTableView, int idx)
{
    ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx(self, dropDownWithTableView, idx);
    UpdateColors();
}

MAKE_HOOK_MATCH(MenuEnvironmentManager_ShowEnvironmentType, &GlobalNamespace::MenuEnvironmentManager::ShowEnvironmentType, void, GlobalNamespace::MenuEnvironmentManager* self, GlobalNamespace::MenuEnvironmentManager::MenuEnvironmentType type)
{
    MenuEnvironmentManager_ShowEnvironmentType(self, type);
    if(type == GlobalNamespace::MenuEnvironmentManager::MenuEnvironmentType::None)
        return;
    UpdatePointers();
    UpdateColors();
}

// Called at the early stages of game loading
MOD_EXPORT void setup(CModInfo* info) {
    *info = modInfo.to_c();

    getModConfig().Init(modInfo);

    INFO("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
MOD_EXPORT void late_load() {
    il2cpp_functions::Init();    

    BSML::Init();
    BSML::Register::RegisterMainMenu<QonsistentSaberColors::SettingsViewController*>("QonsistentSaberColors", "QonsistentSaberColors", "QonsistentSaberColors mod settings");

    INFO("Installing hooks...");
    INSTALL_HOOK(Logger, EnvironmentColorManager_InitColors);
    INSTALL_HOOK(Logger, VRPointer_CreateLaserPointerAndLaserHit);
    INSTALL_HOOK(Logger, MainMenuViewController_DidActivate);
    INSTALL_HOOK(Logger, ColorsOverrideSettingsPanelController_HandleOverrideColorsToggleValueChanged);
    INSTALL_HOOK(Logger, ColorsOverrideSettingsPanelController_HandleEditColorSchemeControllerDidChangeColorScheme);
    INSTALL_HOOK(Logger, ColorsOverrideSettingsPanelController_HandleDropDownDidSelectCellWithIdx);
    INSTALL_HOOK(Logger, MenuEnvironmentManager_ShowEnvironmentType);
    INFO("Installed all hooks!");
}